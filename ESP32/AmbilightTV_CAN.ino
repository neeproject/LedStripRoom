#include <Adafruit_NeoPixel.h>
#include <ESP32CAN.h>
#include <CAN_config.h>

#include <cstdint>  //Potem usun��, potrzebne tylko do VS, aby pod�wietla� inty.


#define NUM_LEDS 306	//ilo�� diod na tv licz�c z dolnym paskiem dolnym kt�ego nie ma
#define TIMEOUT 3000

Adafruit_NeoPixel ledontv(NUM_LEDS, 12, NEO_GRB + NEO_KHZ800); //ledontv/pin=9/type
Adafruit_NeoPixel  ledcupboard(150, 13, NEO_GRB + NEO_KHZ800); //ledoncupboard/pin/type
//Adafruit_NeoPixel    ledheater(100, 14, NEO_GRB + NEO_KHZ800); //ledongrzejnik/pin/type

//CAN
CAN_device_t CAN_cfg;

//Ambilight data
byte MESSAGE_PREAMBLE[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09 }; //Rozpocz�cie wycieczki danych z usb
byte bufferwithUSB[3];	//Plecaki danych przychodz�cych z usb
uint8_t current_preamble_position = 0;  //Liczenie wycieczkowicz�w z usb
unsigned long last_serial_available = -1L;  //Odlczanie czasu wycieczki

//Kopiowanie plecak�w 
uint32_t colorledinnow[NUM_LEDS];	//Kolory led�w odczytanych z seriala


uint8_t sendoncan = 1;		//Czy wysy�a� dane po CAN?
uint8_t sendondownled = 1;	//Czy pod�wietla� d� szafki ambilight?


//Sprawdzanie kt�re ledy aktualizowa�
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


//Kolorowanie led�w
void LedFillColor(uint32_t ColorFill, uint16_t FirstLed, uint16_t CountLed, uint8_t NrLedStrip) {
  switch (NrLedStrip) {
    case 0:   ledontv.fill(ColorFill, FirstLed, CountLed);     break;
    case 1:   ledcupboard.fill(ColorFill, FirstLed, CountLed); break;
    case 2:   ledheater.fill(ColorFill, FirstLed, CountLed);   break;
  }
  ListUpdateLedStrip[NrLedStrip] = 1;
}


//ModeBlack
uint32_t ModeBlackPreviousTime = 0;  //mills wcze�niejszy black moda
uint16_t ModeBlackDelayTime = 65000; //co ile wykonywa� black moda 65 sekund

void ModeBlack() {
  if ((millis() - ModeBlackPreviousTime) > ModeBlackDelayTime) {  //wykonuj co 1200ms.
    ModeBlackPreviousTime = millis();
    ledontv.clear();
    ledcupboard.clear();
    ledheater.clear();
  }
  if (Serial.available() > 0){  //Jesli zaczn� lecie� dane z seriala, w��cz ambilight
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

//Pakowanie kolork�w z Ambilight na sufit
uint8_t AmbiCANarrayRGB[5][3];
void PakowankoColorsOnArraySufitu(uint8_t ledNum, uint8_t ReadBlue, uint8_t ReadGreen, uint8_t ReadRed) {
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

            //Czy wysy�a� dane do leda na dole szafki?
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
    case 0: ModeBlack();     break;   //Dorobi� �adn� animacje w��czaj�ca blacka
    case 1: ModeAmbilight(); break;   //Ambilight na co? TV, Szafka, Grzejnik, Sufit?
    case 2: ModeAnimation(); break;   //Animacje na co? to samo ^
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
  //wyzwalacz CANa zmieniaj�cy mode
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