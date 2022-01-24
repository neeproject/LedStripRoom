#include <Adafruit_NeoPixel.h>
#include <ESP32CAN.h>
#include <CAN_config.h>

//CAN
CAN_device_t CAN_cfg;               // CAN Config
unsigned long previousMillis = 0;   // will store last time a CAN Message was send
const int interval = 1000;          // interval at which send CAN Messages (milliseconds)
const int rx_queue_size = 10;       // Receive Queue size

#define MODE_BLACK 0
#define MODE_AMBILIGHT 1
#define MODE_ANIMATION 2
#define MODE_VUMETER 3

#define NUM_LEDS 306	//iloœæ diod na tv licz¹c z dolnym paskiem któego nie ma
#define TIMEOUT 3000

Adafruit_NeoPixel ledontv(NUM_LEDS, 12, NEO_GRB + NEO_KHZ800); //ledontv/pin=9/type
Adafruit_NeoPixel ledcupboard(150, 13, NEO_GRB + NEO_KHZ800); //ledontv/pin/type

int isnexton = 1;		//jeœli w³¹czy siê tryb Black wy³¹cza diody jednorazowo, nie spami wy³¹czaniem
uint8_t mode = MODE_BLACK;	//wystartuj z trybem ciemnym

byte MESSAGE_PREAMBLE[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09 };
uint8_t PREAMBLE_LENGTH = 10;
uint8_t current_preamble_position = 0;
unsigned long last_serial_available = -1L;
byte buffersr[3];	//Bufer serial read w modeambilight


uint32_t colorledinnow[NUM_LEDS];	//Kolory led odczytane z seriala
int sendoncan = 1;		//Czy wysy³aæ dane po CAN?
int sendondownled = 1;	//Czy podœwietlaæ dó³ szafki ambilight?

void setup()
{
  Serial.begin(576000);
  ledontv.begin();
  ledcupboard.begin();
  CAN_cfg.speed = CAN_SPEED_125KBPS; //CAN_SPEED_125KBPS;
  CAN_cfg.tx_pin_id = GPIO_NUM_22;
  CAN_cfg.rx_pin_id = GPIO_NUM_23;
  CAN_cfg.rx_queue = xQueueCreate(rx_queue_size, sizeof(CAN_frame_t));
  ESP32Can.CANInit();
}

void offLedonCup() {
  for (int i = 0; i < 150; i++)
  {
    ledcupboard.setPixelColor(i, 0, 0, 0);
  }
  ledcupboard.show();
}


void offLedonTV()		//funkcja wy³¹cza ledy na TV
{
  for (int i = 0; i < NUM_LEDS; i++)
  {
    ledontv.setPixelColor(i, 0, 0, 0);
  }
  ledontv.show();
}

void modeBlack()	// Wy³¹cza ledy na tv tylko raz. Jeœli jest odczyt z seriala w³¹cza ambilight
{
  if (isnexton > 0) {	//if || min¹³ odpowiedni czas, jesli by siê zapala³y same (dodaæ to)
    offLedonTV();
    if (sendondownled) { offLedonCup(); }
    isnexton = 0;
  }
  if (Serial.available() > 0)
  {
    mode = MODE_AMBILIGHT;
  }
}

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

uint8_t AmbiCANarrayRGB[5][3];

void modeAmbilight()	// Mode Ambilight
{
  isnexton = MODE_AMBILIGHT;

  if (waitForPreamble(TIMEOUT))
  {
    for (int ledNum = 0; ledNum < NUM_LEDS + 1; ledNum++)
    {
      if (Serial.readBytes((char*)buffersr, 3) < 3) return;

      if (ledNum < NUM_LEDS)
      {
        uint8_t ReadBlue  = buffersr[0];
        uint8_t ReadGreen = buffersr[1];
        uint8_t ReadRed   = buffersr[2];
        colorledinnow[ledNum] = ledontv.Color(ReadRed, ReadGreen, ReadBlue);	//pakowanie color z seriala do array

        //Czy wysy³aæ dane po can?
        if (sendoncan) {
          if (ledNum == 64) {  //Wczeœniej 54< X <153
            AmbiCANarrayRGB[4][0] = ReadRed;
            AmbiCANarrayRGB[4][1] = ReadGreen;
            AmbiCANarrayRGB[4][2] = ReadBlue;
          }else
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

      }
      else if (ledNum == NUM_LEDS)
      {
        if (buffersr[0] == 85 && buffersr[1] == 204 && buffersr[2] == 165)	//sprawdzenie czy to na pewno koniec
        {
          //Czy wysy³aæ dane po can?
          if (sendoncan) {
            //tutaj zrobiæ wysy³anie kolorów na CANa
            for (uint8_t i = 0; i < 5; i++) {
              WyslijCanAmbilight(i, AmbiCANarrayRGB[i][0], AmbiCANarrayRGB[i][1], AmbiCANarrayRGB[i][2]);
            }
          }
          

          for (int ledNum = 0; ledNum < NUM_LEDS; ledNum++) //Pêtla zapalaj¹ca ledy 
          {
            ledontv.setPixelColor(ledNum, colorledinnow[ledNum]);	//zapalanie led z array


            //Czy wysy³aæ dane do leda na dole szafki?
            if (sendondownled && (ledNum > 207) && (ledNum <= NUM_LEDS)) {	//if is numer leda w przedziale 207-305
              int ledCupNum = ledNum - 182;
              //22 diody po prawej + 22 diody po lewej, a z przodu jest 106 diod.
              //7 - x -1
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
  else
  {
    mode = MODE_BLACK;
  }
}

bool waitForPreamble(int timeout)	//wykrywanie po³¹czenia Serial - Komputer
{
  last_serial_available = millis();
  current_preamble_position = 0;
  while (current_preamble_position < PREAMBLE_LENGTH)	//while siê nie na³aduje do 10
  {
    if (Serial.available() > 0)							//and wykryto przesy³
    {
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



void loop()
{
  switch (mode) {
  case MODE_BLACK:
    modeBlack();
    break;
  case MODE_AMBILIGHT:
    modeAmbilight();
    break;
  }

  //wyzwalacz CANa zmieniaj¹cy mode
}



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