#include <Adafruit_NeoPixel.h>
#include <ESP32CAN.h>
#include <CAN_config.h>

#include <cstdint>


#define NUM_LEDS 306	//ilo�� diod na tv licz�c z dolnym paskiem dolnym kt�ego nie ma
#define TIMEOUT 3000

Adafruit_NeoPixel ledontv(NUM_LEDS, 12, NEO_GRB + NEO_KHZ800); //ledontv/pin=9/type
Adafruit_NeoPixel  ledcupboard(150, 13, NEO_GRB + NEO_KHZ800); //ledoncupboard/pin/type
Adafruit_NeoPixel    ledheater(59, 14, NEO_GRB + NEO_KHZ800); //ledongrzejnik/pin/type

Adafruit_NeoPixel    ledwardrobeon(284, 27, NEO_GRB + NEO_KHZ800); //led on wardrobe/pin/type
Adafruit_NeoPixel    ledwardrobein(118, 26, NEO_GRB + NEO_KHZ800); //led in wardrobe/pin/type
Adafruit_NeoPixel    ledwardrobetube(49, 25, NEO_GRB + NEO_KHZ800); //led in tube wardrobe/pin/type
Adafruit_NeoPixel    ledondoor(171, 33, NEO_GRB + NEO_KHZ800); //led on door/pin/type

//CAN
CAN_device_t CAN_cfg;

//Ambilight data
byte MESSAGE_PREAMBLE[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09 }; //Rozpocz�cie wycieczki danych z usb
byte bufferwithUSB[3];	//Plecaki danych przychodz�cych z usb
uint8_t current_preamble_position = 0;  //Liczenie wycieczkowicz�w z usb
unsigned long last_serial_available = -1L;  //Odlczanie czasu wycieczki

//Kopiowanie plecak�w 
uint32_t colorledinnow[NUM_LEDS];	//Kolory led�w odczytanych z seriala


uint8_t ActualMode = 0; //Whatismode aktualnie?

uint8_t sendoncan = 1;		//Czy wysy�a� dane po CAN?
uint8_t sendoncupled = 1;	//Czy pod�wietla� d� szafki ambilight?
uint8_t sendonheatled = 0;  //Czy wysy�a� dane pod grzejnik?
uint8_t sendonwardrobe = 0;  //Czy wysy�a� dane na szafe?
uint8_t sendondoor = 0;  //Czy wysy�a� dane na drzwi?

//Sprawdzanie kt�re ledy aktualizowa�
uint8_t ListUpdateLedStrip[7] = { 0,0,0,0,0,0,0 };  //TV //Szafka //Grzejnik

void LedStripShowUpdate(uint8_t NrLedStrip) {
  switch (NrLedStrip) {
    case 0: ledontv.show();     break;
    case 1: ledcupboard.show(); break;
    case 2: ledheater.show();   break;
    case 3: ledwardrobeon.show();   break;
    case 4: ledondoor.show();       break;
    case 5: ledwardrobein.show();   break;
    case 6: ledwardrobetube.show(); break;
  }
}
void WhichLedStripUpdate() {
  for (uint8_t i = 0; i < 7; i++) {
    if (ListUpdateLedStrip[i]) {
      LedStripShowUpdate(i);
      ListUpdateLedStrip[i] = 0;
    }
  }
}


//Kolorowanie led�w
void LedFillColor(uint32_t ColorFill, uint16_t FirstLed, uint16_t CountLed, uint8_t NrLedStrip) {
  switch (NrLedStrip) {
    case 0:   ledontv.fill(ColorFill, FirstLed, CountLed);     break;
    case 1:   ledcupboard.fill(ColorFill, FirstLed, CountLed); break;
    case 2:   ledheater.fill(ColorFill, FirstLed, CountLed);   break;
    case 3:   ledwardrobeon.fill(ColorFill, FirstLed, CountLed);   break;
    case 4:   ledondoor.fill(ColorFill, FirstLed, CountLed);   break;
    case 5:   ledwardrobein.fill(ColorFill, FirstLed, CountLed);   break;
    case 6:   ledwardrobetube.fill(ColorFill, FirstLed, CountLed);   break;
  }
  ListUpdateLedStrip[NrLedStrip] = 1;
}


//ModeBlack
uint32_t ModeBlackPreviousTime = 0;  //mills wcze�niejszy black moda
uint16_t ModeBlackDelayTime = 65000; //co ile wykonywa� black moda 65 sekund

void ModeBlack() {
  if ((millis() - ModeBlackPreviousTime) > ModeBlackDelayTime) {  //wykonuj co 1200ms.
    ModeBlackPreviousTime = millis();
    LedFillColor(0, 0, 0, 0);
    LedFillColor(0, 0, 0, 1);
    LedFillColor(0, 0, 0, 2);
    LedFillColor(0, 0, 0, 3);
    LedFillColor(0, 0, 0, 4);
  }
  if (Serial.available() > 0){  //Jesli zaczn� lecie� dane z seriala, w��cz ambilight
    ActualMode = 20; //Ustaw ModeAmbilight
  }
}

uint8_t BlackModeIncommingBright = 0;
uint32_t BlackModeIncommingTime = 0;
uint16_t BlackModeIncommingTimeDelay = 50;
bool BlackModeIncommingArrow = 0;

void BlackModeIncomming() {
  if ((millis() - BlackModeIncommingTime) > BlackModeIncommingTimeDelay) {
    BlackModeIncommingTime = millis();

    if (BlackModeIncommingArrow == 0) {
      BlackModeIncommingBright++;
      if (BlackModeIncommingBright == 100) { BlackModeIncommingArrow = 1; }
    }
    else {
      BlackModeIncommingBright--;
      if (BlackModeIncommingBright == 0) {
        ActualMode = 0; //Ustaw BlackMode
        BlackModeIncommingArrow = 0;
      }
    }
    uint32_t BlackModeIncommingColor = ledontv.ColorHSV(0, 255, BlackModeIncommingBright);  //przeskok koloru,nasycenie,jasno��
    for (uint8_t i = 0; i < 4; i++) {
      LedFillColor(BlackModeIncommingColor, 0, 0, i);
    }
  }
}

//Can Send Ambilight Color
void WyslijCanAmbilight(uint8_t NrLedinStrip, uint8_t ColorR, uint8_t ColorG, uint8_t ColorB) { //RGB+Nr_Paska
  CAN_frame_t tx_frame;
  tx_frame.FIR.B.FF = CAN_frame_std;
  tx_frame.MsgID = 0x020;
  tx_frame.FIR.B.DLC = 4;
  tx_frame.data.u8[0] = NrLedinStrip;
  tx_frame.data.u8[1] = ColorR;
  tx_frame.data.u8[2] = ColorG;
  tx_frame.data.u8[3] = ColorB;
  //delay(1);
  ESP32Can.CANWriteFrame(&tx_frame);
}

//Pakowanie kolork�w z Ambilight na sufit
uint8_t AmbiCANarrayRGB[5][3];  //sufit

void PakowankoColorsOnArraySufitu(uint8_t ledNum, uint8_t ReadRed, uint8_t ReadGreen, uint8_t ReadBlue) {
  if (ledNum == 64) {  //Wcze�niej 54< X <153
    AmbiCANarrayRGB[4][0] = ReadRed;
    AmbiCANarrayRGB[4][1] = ReadGreen;
    AmbiCANarrayRGB[4][2] = ReadBlue;
  }
  else
    if (ledNum == 84) {  //+20
      AmbiCANarrayRGB[3][0] = ReadRed;
      AmbiCANarrayRGB[3][1] = ReadGreen;
      AmbiCANarrayRGB[3][2] = ReadBlue;
    }
    else
      if (ledNum == 104) {  //Wcze�niej 54< X <153
        AmbiCANarrayRGB[2][0] = ReadRed;
        AmbiCANarrayRGB[2][1] = ReadGreen;
        AmbiCANarrayRGB[2][2] = ReadBlue;
      }
      else
        if (ledNum == 124) {  //-20
          AmbiCANarrayRGB[1][0] = ReadRed;
          AmbiCANarrayRGB[1][1] = ReadGreen;
          AmbiCANarrayRGB[1][2] = ReadBlue;
        }
        else
          if (ledNum == 143) {
            AmbiCANarrayRGB[0][0] = ReadRed;
            AmbiCANarrayRGB[0][1] = ReadGreen;
            AmbiCANarrayRGB[0][2] = ReadBlue;
          }
}

//AmbilightMode
void ModeAmbilight()	// Mode Ambilight
{
  if (waitForPreamble(TIMEOUT)) //je�li nie sko�czy czasu
  {
    for (int ledNum = 0; ledNum < NUM_LEDS + 1; ledNum++)
    {
      if (Serial.readBytes((char*)bufferwithUSB, 3) < 3) return;

      if (ledNum < NUM_LEDS)  //Czy przesy�aj� si� jeszcze kolory?
      {
        uint8_t ReadBlue = bufferwithUSB[0];
        uint8_t ReadGreen = bufferwithUSB[1];
        uint8_t ReadRed = bufferwithUSB[2];
        colorledinnow[ledNum] = ledontv.Color(ReadRed, ReadGreen, ReadBlue);	//pakowanie color z seriala do array

        //Czy wysy�a� dane po can?
        if (sendoncan) {
          PakowankoColorsOnArraySufitu(ledNum, ReadRed, ReadGreen, ReadBlue); //Pakowanie color z seriala do array na sufit
        }

      }
      else if (ledNum == NUM_LEDS)  //Czy przes�ano ju� wszystkie kolory?
      {
        if (bufferwithUSB[0] == 85 && bufferwithUSB[1] == 204 && bufferwithUSB[2] == 165)	//sprawdzenie czy to na pewno koniec
        {

          //Czy wysy�a� dane po can?
          if (sendoncan) {
            for (uint8_t i = 0; i < 5; i++) {
              WyslijCanAmbilight(i, AmbiCANarrayRGB[i][0], AmbiCANarrayRGB[i][1], AmbiCANarrayRGB[i][2]);
            }
          }

          //P�tla dotycz�ca ambilighta z ta�mami lokalnymi
          for (int ledNum = 0; ledNum < NUM_LEDS; ledNum++) //P�tla zapalaj�ca ledy 
          {
            ledontv.setPixelColor(ledNum, colorledinnow[ledNum]);	//zapalanie led z array, setpixel chyba szybsze b�dzie

            //Czy wysy�a� dane na du�� szafe?
            if (sendonwardrobe && (ledNum >= 0) && (ledNum <= 55)) {
              if (sendonwardrobe==1) {
                for (uint8_t i = 0; i < 3; i++) {
                  ledwardrobeon.setPixelColor(ledNum * 3 + i, colorledinnow[ledNum]); //tutaj szafka
                }
              }else if (sendonwardrobe==2) {
                for (uint8_t i = 0; i < 5; i++) {
                  ledwardrobeon.setPixelColor(ledNum * 5 + i, colorledinnow[ledNum]); //tutaj ca�a prawie szafka
                }
              }
            }

            //Czy wysy�a� dane ambilight na drzwi?
            if (sendondoor && (ledNum >= 0) && (ledNum <= 55)) {
              for (uint8_t i = 0; i < 3; i++) {
                ledondoor.setPixelColor(ledNum * 3 + i, colorledinnow[ledNum]); //tutaj szafka
              }
            }


            //Czy wysy�a� dane do leda na dole szafki?
            if (sendoncupled && (ledNum > 208) && (ledNum <= NUM_LEDS)) {	//if is numer leda w przedziale 207-305
              int ledCupNum = ledNum - 182;   //22 diody po prawej + 22 diody po lewej, a z przodu szafki jest 106 diod.
              ledcupboard.setPixelColor(ledCupNum, colorledinnow[ledNum]);
            }
          }

          if (sendonheatled) {
            for (uint8_t i = 0; i < 20; i++) {
              ledheater.setPixelColor(i, colorledinnow[200]); //0-19
              ledheater.setPixelColor(i+20, colorledinnow[205]);  //20-39
              ledheater.setPixelColor(i+40, colorledinnow[210]);  //40-59, ale jest 58.
            }
            ledheater.show();
          }

          ledontv.show();

          if (sendonwardrobe) {
            ledwardrobeon.show(); //du�a szafa
          }
          if (sendoncupled) {
            ledcupboard.show(); //szafka pod tv
          }
          if (sendondoor) {
            ledondoor.show(); //drzwi
          }

        }
      }
    }
  }
  else {
    ActualMode = 19;
  }
}


//Animacje
//Animacje
//Animacje


//Pocz�tek animacji Smooth
uint16_t SmoothColorHSV = 0;    //0-65535 = kolor HSV
uint16_t SmoothJumpHSV = 10;  //Przeskok koloru HSV
uint8_t SmoothBrightHSV = 255;        //jasno�� HSV

void AnimateSmoothAll() {
  SmoothColorHSV += (SmoothJumpHSV * 10);
  uint32_t ChangeColorSmoothHSV = ledontv.ColorHSV(SmoothColorHSV, 255, SmoothBrightHSV);  //przeskok koloru,nasycenie,jasno��
  for (uint8_t i = 0; i < 5; i++) {
    LedFillColor(ChangeColorSmoothHSV, 0, 0, i);
  }
}


void KolorujJedenPasek(uint8_t ColorR, uint8_t ColorG, uint8_t ColorB, uint8_t NrLedStrip) {
  uint32_t KolorujWszystkieTasmyRGB = ledontv.Color(ColorR, ColorG, ColorB);
  LedFillColor(KolorujWszystkieTasmyRGB, 0, 0, NrLedStrip);
}

void KolorujWszystkieTasmy(uint8_t ColorR, uint8_t ColorG, uint8_t ColorB) {
  uint32_t KolorujWszystkieTasmyRGB = ledontv.Color(ColorR, ColorG, ColorB);
  for (uint8_t i = 0; i < 5; i++) {
    LedFillColor(KolorujWszystkieTasmyRGB, 0, 0, i);
  }
}



//Pocz�tek animacji Rainbow
uint16_t RainbowColorStartAllHSV = 0;    //0-65535 = kolor HSV
uint16_t RainbowJumpAllHSV = 10;  //To wp�ynie na pr�dko�� przej�cia kolor�w
uint16_t RainbowJumpOneHSV = 100;  //Wi�ksza warto�� to wi�cej kolor�w na ta�mach
uint8_t RainbowBrightAllHSV = 255;        //jasno�� HSV

void AnimateRainbow(uint16_t StartColorRainbowHSV, uint8_t RainbowBrightHSV, uint8_t NrLedStrip) {
  switch (NrLedStrip) {
  case 0: //TV
    for (uint8_t i = 0; i < 208; i++) {
      uint32_t Color32Rainbow = ledontv.ColorHSV(StartColorRainbowHSV + (RainbowJumpOneHSV * i), 255, RainbowBrightHSV);
      LedFillColor(Color32Rainbow, i, 1, 0);
    }
    break;
  case 1: //CUP
    for (uint8_t i = 0; i < 150; i++) {
      uint32_t Color32Rainbow = ledcupboard.ColorHSV(StartColorRainbowHSV + (RainbowJumpOneHSV * i), 255, RainbowBrightHSV);
      LedFillColor(Color32Rainbow, i, 1, 1);
    }
    break;
  case 2: //HEAT
    for (uint8_t i = 0; i < 88; i++) {
      uint32_t Color32Rainbow = ledontv.ColorHSV(StartColorRainbowHSV + (RainbowJumpOneHSV * i), 255, RainbowBrightHSV);
      LedFillColor(Color32Rainbow, i, 1, 2);
    }
    break;
  }
}

void AnimateRainbowSyncAll() {

  AnimateRainbow(RainbowColorStartAllHSV, RainbowBrightAllHSV, 0);
  AnimateRainbow(RainbowColorStartAllHSV + (181 * RainbowJumpOneHSV), RainbowBrightAllHSV, 1);
  AnimateRainbow(RainbowColorStartAllHSV + (208 * RainbowJumpOneHSV), RainbowBrightAllHSV, 2);

  RainbowColorStartAllHSV += RainbowJumpAllHSV;
}



uint32_t PreviousTimeAnimation = 0;
uint16_t DelayTimeAnimation = 20;

void HeadVoidAnimation() {
  if ((millis() - PreviousTimeAnimation) > DelayTimeAnimation) {
    PreviousTimeAnimation = millis();

    switch (ActualMode) {
    case 1: AnimateSmoothAll();
      break;
    case 2: AnimateRainbowSyncAll();
      break;
    }
  }

  //Animacje z delay w void 
  switch (ActualMode) {
  case 3: //AnimateDisappeLed();
    break;
  }

}


//Whatismode aktualnie?

void WhatisMode() {
  switch (ActualMode) {
    case 0: ModeBlack();     break;   //Dorobi� �adn� animacje w��czaj�ca blacka
    case 1: case 2: case 3:
      HeadVoidAnimation(); break;
    case 19: BlackModeIncomming(); break;
    case 20: ModeAmbilight(); break;   //Ambilight na co? TV, Szafka, Grzejnik, Sufit?
  }
}

//Oczekiwanie na serial
bool waitForPreamble(int timeout)	//wykrywanie po��czenia Serial - Komputer
{
  last_serial_available = millis();
  current_preamble_position = 0;
  while (current_preamble_position < 10) {	//while si� nie na�aduje do 10
    if (Serial.available() > 0) {							//and wykryto przesy�
      last_serial_available = millis();

      if (Serial.read() == MESSAGE_PREAMBLE[current_preamble_position]) {	//if (przesy� == [1-10])
        current_preamble_position++;									//przesy�++
      }
      else {
        current_preamble_position = 0;									//od nowa
      }
    }
    if (millis() - last_serial_available > timeout) {
      return false;
    }
  }
  return true;
}


void Can_reader() //Odbieranie danych z Cana
{
  CAN_frame_t rx_frame;
  unsigned long cMillisInReadCan = millis();

  if (xQueueReceive(CAN_cfg.rx_queue, &rx_frame, 3 * portTICK_PERIOD_MS) == pdTRUE) {

    if (rx_frame.MsgID == 0x015) {	//Informacja tylko do szafki TV www kolory
      switch (rx_frame.FIR.B.DLC) {   //Sprawdzanie d�ugo�ci ci�gu
        case 3:
          KolorujWszystkieTasmy(rx_frame.data.u8[0], rx_frame.data.u8[1], rx_frame.data.u8[2]);  //RGB
          break;
        case 4:
          KolorujJedenPasek(rx_frame.data.u8[0], rx_frame.data.u8[1], rx_frame.data.u8[2], rx_frame.data.u8[3]); //RGB+Nr_Paska
          break;
      }
    }

    if (rx_frame.MsgID == 0x025) {	//Informacja tylko dla szafki TV RGB www - Animacje
      switch (rx_frame.FIR.B.DLC) {   //Sprawdzanie d�ugo�ci ci�gu
      case 2:
        switch (rx_frame.data.u8[0]) {
          case 0: ActualMode = rx_frame.data.u8[1];  //Typ animacji  //0 0 - wylaczenie  //0 1 =Animacja 1 //0 2=Animacja 2
            break;
          case 1: DelayTimeAnimation = rx_frame.data.u8[1]; //default 20 // Czas powt�rzenia animacji 1, 2, gdzie g��wny delay jest
            break;
            //animacja 1
          case 10: SmoothJumpHSV = rx_frame.data.u8[1] * 5;   //default 10  //skok kolor�w
            break;
          case 11: SmoothBrightHSV = rx_frame.data.u8[1]; //default 255 //jasno�� animacji 1
            break;
            //animacja 2
          case 20: RainbowJumpAllHSV = rx_frame.data.u8[1] * 5;   //default 10  //predkosc przesuwania kolor�w animacji 2
            break;
          case 21: RainbowJumpOneHSV = rx_frame.data.u8[1] * 5;   //default 100 //o ile jednostek wi�cej ma �wieci� kolejny led animacji 2
            break;
          case 22: RainbowBrightAllHSV = rx_frame.data.u8[1]; //default 255 //jasno�� animacji 2
            break;
            //animacja 3
            /*
          case 30: DisappeLedActive = rx_frame.data.u8[1];   //default 35 //ilo�� led w animacji 3
            break;
          case 31: DisappeNewPixelDelayTime = rx_frame.data.u8[1] * 100;   //default 5*100=500ms //Czas co ile dodawa� now� diod�
            break;
          case 32: DisappeDelayTime = rx_frame.data.u8[1] * 10;   //default 9*10=90ms //Czas co ile aktualizowa� jasno�� diody
            break;
          case 33: DisappeColorBrightMaxValue = rx_frame.data.u8[1];   //default 249 //Jasno�� max animacji 3
            break;
          case 34: DisappeColorRandom = rx_frame.data.u8[1];   //def 1 //Czy kolor animacji 3 ma by� losowany? Je�li nie to 0
            break;
          case 35: DisappeLedColorStatic = rx_frame.data.u8[1] * 255; //Ustawianie koloru statycznego hsv
            break;*/
            //animacja 4

          case 50: sendoncan = rx_frame.data.u8[1]; //default 1 //Czy wysy�a� dane z Ambilight przez CAN?
            break;
          case 51: sendoncupled = rx_frame.data.u8[1]; //default 1 //Czy wysy�a� dane z Ambilight do szafki?
            break;
          case 52: sendonheatled = rx_frame.data.u8[1]; //default 0 //Czy wysy�a� dane pod grzejnik?
            break;
          case 53: sendonwardrobe = rx_frame.data.u8[1]; //default 0 //Czy wysy�a� dane na du�� szafe?
            break;
          case 54: sendondoor = rx_frame.data.u8[1]; //default 0 //Czy wysy�a� dane na drzwi?
            break;
        }
      }
    }

    //AmbilightTV on off
    if (rx_frame.MsgID == 0x022 || rx_frame.MsgID == 0x023) { //22 dla sufitu i tv szafki //23 dla samej szafki

      switch (rx_frame.data.u8[0]) {
      case 0:
        ActualMode = 0;  //wylaczyc wszystko
        break;
      case 1:
        ActualMode = 20; //wlaczyc ambilight
        break;  //case 2 jest pominiety, bo jest tylko dla sufitu
      case 3:
        ActualMode = 1; //zrobic inny styl - Smooth
        break;
      case 4:
        ActualMode = 2; //zrobic inny styl - Rainbow
        break;
      case 5:
        ActualMode = 3; //zrobic inny styl - Disapled
        break;
      }
    }


  }

}


//Setup po prostu
void setup()
{
  CAN_cfg.speed = CAN_SPEED_125KBPS; //CAN_SPEED_125KBPS;
  CAN_cfg.tx_pin_id = GPIO_NUM_22;
  CAN_cfg.rx_pin_id = GPIO_NUM_23;
  CAN_cfg.rx_queue = xQueueCreate(10, sizeof(CAN_frame_t));
  ESP32Can.CANInit();

  ledontv.begin();
  ledcupboard.begin();
  ledheater.begin();

  ledwardrobeon.begin();
  ledwardrobein.begin();
  ledwardrobetube.begin();

  Serial.begin(576000);
}

//Loopek
void loop()
{
  Can_reader(); //Odbieranie danych z Cana
  WhatisMode();
  WhichLedStripUpdate();  //Aktualizowanie led�w
}



//Pozosta�o ogarn�� to:











//==================================
/*

#define musicReadPin 21

int musicStyleLed = 0;
int musicVolumeMax = 5;
int musicVolumeMaxNew = 5;
int musicValueRange = 0;
unsigned long musicNewMaxTime;



void MusicRead() {	//Odczytywanie muzyki
  int musicValInput = analogRead(musicReadPin);	//0-1023

  if (musicVolumeMax <= musicValInput) {		//1 ustalanie maksymalnej volume max
    musicVolumeMax = musicValInput;
    musicVolumeMaxNew = 5;
    musicNewMaxTime = millis();
  }
  else

    if (millis() - musicNewMaxTime > 17000) {		//3 if od zbierania new danych min�lo 12s.
      musicVolumeMax = musicVolumeMaxNew;
      musicVolumeMaxNew = 5;
      musicNewMaxTime = millis();
    }
    else

      if (millis() - musicNewMaxTime > 4000) {		//2 if od ustalenia nowego volume max min�o 5s
        if (musicVolumeMaxNew < musicValInput) {
          musicVolumeMaxNew = musicValInput;
        }
      }

  musicValueRange = map(musicValInput, 0, musicVolumeMax, 0, 100);	//read, 0,readMax, 0,100

  musicCanSendOneTrack(musicValueRange);	//Analog muzyki w % od 0 do 100.
}

void musicCanSendOneTrack(int musiconehz) {	//wysy�anie muzyki po CAN
  CAN_frame_t tx_frame;
  tx_frame.FIR.B.FF = CAN_frame_std;	//oznaczenie zwyk�ej ramki
  tx_frame.MsgID = 0x099;	//music canal
  tx_frame.FIR.B.DLC = 2;
  tx_frame.data.u8[0] = musicStyleLed;	//Styl led�w
  tx_frame.data.u8[1] = musiconehz;	//sygna� hz
  tx_frame.data.u8[2] = musiconehz;	//sygna� hz
  //delay(1);
  ESP32Can.CANWriteFrame(&tx_frame);
}
*/