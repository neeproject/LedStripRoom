#include <Adafruit_NeoPixel.h>
#include <ESP32CAN.h>
#include <CAN_config.h>

#include <cstdint>  //Potem usun¹æ, potrzebne tylko do VS, aby podœwietlaæ inty.


#define NUM_LEDS 306	//iloœæ diod na tv licz¹c z dolnym paskiem dolnym któego nie ma
#define TIMEOUT 3000

Adafruit_NeoPixel ledontv(NUM_LEDS, 12, NEO_GRB + NEO_KHZ800); //ledontv/pin=9/type
Adafruit_NeoPixel  ledcupboard(150, 13, NEO_GRB + NEO_KHZ800); //ledoncupboard/pin/type
//Adafruit_NeoPixel    ledheater(100, 14, NEO_GRB + NEO_KHZ800); //ledongrzejnik/pin/type

//CAN
CAN_device_t CAN_cfg;

//Ambilight data
byte MESSAGE_PREAMBLE[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09 }; //Rozpoczêcie wycieczki danych z usb
byte bufferwithUSB[3];	//Plecaki danych przychodz¹cych z usb
uint8_t current_preamble_position = 0;  //Liczenie wycieczkowiczów z usb
unsigned long last_serial_available = -1L;  //Odlczanie czasu wycieczki

//Kopiowanie plecaków 
uint32_t colorledinnow[NUM_LEDS];	//Kolory ledów odczytanych z seriala


uint8_t sendoncan = 1;		//Czy wysy³aæ dane po CAN?
uint8_t sendondownled = 1;	//Czy podœwietlaæ dó³ szafki ambilight?


//Sprawdzanie które ledy aktualizowaæ
uint8_t ListUpdateLedStrip[3] = { 0,0,0 };  //TV //Szafka //Grzejnik

void LedStripShowUpdate(uint8_t NrLedStrip) {
  switch (NrLedStrip) {
    case 0: ledontv.show();     break;
    case 1: ledcupboard.show(); break;
    case 2: ledheater.show();   break;
  }
}
void WhichLedStripUpdate() {
  for (uint8_t i = 0; i < 3; i++) {
    if (ListUpdateLedStrip[i]) {
      LedStripShowUpdate(i);
      ListUpdateLedStrip[i] = 0;
    }
  }
}


//Kolorowanie ledów
void LedFillColor(uint32_t ColorFill, uint16_t FirstLed, uint16_t CountLed, uint8_t NrLedStrip) {
  switch (NrLedStrip) {
    case 0:   ledontv.fill(ColorFill, FirstLed, CountLed);     break;
    case 1:   ledcupboard.fill(ColorFill, FirstLed, CountLed); break;
    case 2:   ledheater.fill(ColorFill, FirstLed, CountLed);   break;
  }
  ListUpdateLedStrip[NrLedStrip] = 1;
}


//ModeBlack
uint32_t ModeBlackPreviousTime = 0;  //mills wczeœniejszy black moda
uint16_t ModeBlackDelayTime = 65000; //co ile wykonywaæ black moda 65 sekund

void ModeBlack() {
  if ((millis() - ModeBlackPreviousTime) > ModeBlackDelayTime) {  //wykonuj co 1200ms.
    ModeBlackPreviousTime = millis();
    ledontv.clear();
    ledcupboard.clear();
    ledheater.clear();
  }
  if (Serial.available() > 0){  //Jesli zaczn¹ lecieæ dane z seriala, w³¹cz ambilight
    ActualMode = 1; //Ustaw ModeAmbilight
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

//Pakowanie kolorków z Ambilight na sufit
uint8_t AmbiCANarrayRGB[5][3];
void PakowankoColorsOnArraySufitu(uint8_t ledNum, uint8_t ReadBlue, uint8_t ReadGreen, uint8_t ReadRed) {
  if (ledNum == 64) {  //Wczeœniej 54< X <153
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
      if (ledNum == 104) {  //Wczeœniej 54< X <153
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
  if (waitForPreamble(TIMEOUT)) //jeœli nie skoñczy czasu
  {
    for (int ledNum = 0; ledNum < NUM_LEDS + 1; ledNum++)
    {
      if (Serial.readBytes((char*)bufferwithUSB, 3) < 3) return;

      if (ledNum < NUM_LEDS)  //Czy przesy³aj¹ siê jeszcze kolory?
      {
        uint8_t ReadBlue = bufferwithUSB[0];
        uint8_t ReadGreen = bufferwithUSB[1];
        uint8_t ReadRed = bufferwithUSB[2];
        colorledinnow[ledNum] = ledontv.Color(ReadRed, ReadGreen, ReadBlue);	//pakowanie color z seriala do array

        //Czy wysy³aæ dane po can?
        if (sendoncan) {
          PakowankoColorsOnArraySufitu(ledNum, ReadRed, ReadGreen, ReadBlue); //Pakowanie color z seriala do array na sufit
        }

      }
      else if (ledNum == NUM_LEDS)  //Czy przes³ano ju¿ wszystkie kolory?
      {
        if (bufferwithUSB[0] == 85 && bufferwithUSB[1] == 204 && bufferwithUSB[2] == 165)	//sprawdzenie czy to na pewno koniec
        {

          //Czy wysy³aæ dane po can?
          if (sendoncan) {
            for (uint8_t i = 0; i < 5; i++) {
              WyslijCanAmbilight(i, AmbiCANarrayRGB[i][0], AmbiCANarrayRGB[i][1], AmbiCANarrayRGB[i][2]);
            }
          }

          //Pêtla dotycz¹ca ambilighta z taœmami lokalnymi
          for (int ledNum = 0; ledNum < NUM_LEDS; ledNum++) //Pêtla zapalaj¹ca ledy 
          {
            ledontv.setPixelColor(ledNum, colorledinnow[ledNum]);	//zapalanie led z array, setpixel chyba szybsze bêdzie

            //Czy wysy³aæ dane do leda na dole szafki?
            if (sendondownled && (ledNum > 207) && (ledNum <= NUM_LEDS)) {	//if is numer leda w przedziale 207-305
              int ledCupNum = ledNum - 182;   //22 diody po prawej + 22 diody po lewej, a z przodu szafki jest 106 diod.
              ledcupboard.setPixelColor(ledCupNum, colorledinnow[ledNum]);
            }
          }

          ledontv.show();

          if (sendondownled) {
            ledcupboard.show();
          }
        }
      }
    }
  }
}


//Animacje
void ModeAnimation() {

}



//Whatismode aktualnie?
uint8_t ActualMode = 0;

void WhatisMode() {
  switch (ActualMode) {
    case 0: ModeBlack();     break;   //Dorobiæ ³adn¹ animacje w³¹czaj¹ca blacka
    case 1: ModeAmbilight(); break;   //Ambilight na co? TV, Szafka, Grzejnik, Sufit?
    case 2: ModeAnimation(); break;   //Animacje na co? to samo ^
  }
}

//Oczekiwanie na serial
bool waitForPreamble(int timeout)	//wykrywanie po³¹czenia Serial - Komputer
{
  last_serial_available = millis();
  current_preamble_position = 0;
  while (current_preamble_position < 10) {	//while siê nie na³aduje do 10
    if (Serial.available() > 0) {							//and wykryto przesy³
      last_serial_available = millis();

      if (Serial.read() == MESSAGE_PREAMBLE[current_preamble_position]) {	//if (przesy³ == [1-10])
        current_preamble_position++;									//przesy³++
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

  Serial.begin(576000);
}

//Loopek
void loop()
{
  //wyzwalacz CANa zmieniaj¹cy mode
  WhatisMode();
  WhichLedStripUpdate();  //Aktualizowanie ledów
}



//Pozosta³o ogarn¹æ to:











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

    if (millis() - musicNewMaxTime > 17000) {		//3 if od zbierania new danych minêlo 12s.
      musicVolumeMax = musicVolumeMaxNew;
      musicVolumeMaxNew = 5;
      musicNewMaxTime = millis();
    }
    else

      if (millis() - musicNewMaxTime > 4000) {		//2 if od ustalenia nowego volume max minê³o 5s
        if (musicVolumeMaxNew < musicValInput) {
          musicVolumeMaxNew = musicValInput;
        }
      }

  musicValueRange = map(musicValInput, 0, musicVolumeMax, 0, 100);	//read, 0,readMax, 0,100

  musicCanSendOneTrack(musicValueRange);	//Analog muzyki w % od 0 do 100.
}

void musicCanSendOneTrack(int musiconehz) {	//wysy³anie muzyki po CAN
  CAN_frame_t tx_frame;
  tx_frame.FIR.B.FF = CAN_frame_std;	//oznaczenie zwyk³ej ramki
  tx_frame.MsgID = 0x099;	//music canal
  tx_frame.FIR.B.DLC = 2;
  tx_frame.data.u8[0] = musicStyleLed;	//Styl ledów
  tx_frame.data.u8[1] = musiconehz;	//sygna³ hz
  tx_frame.data.u8[2] = musiconehz;	//sygna³ hz
  //delay(1);
  ESP32Can.CANWriteFrame(&tx_frame);
}
*/