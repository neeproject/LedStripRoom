#include <Arduino.h>
#include <ESP32CAN.h>
#include <CAN_config.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>

#include <cstdint>  //Podœwietlanie uintów

//CAN
CAN_device_t CAN_cfg;               // CAN Config
unsigned long previousMillis = 0;   // will store last time a CAN Message was send
const int interval = 1000;          // interval at which send CAN Messages (milliseconds)
const int rx_queue_size = 10;       // Receive Queue size

//WiFi
const char* ssid = "NeeNeet-AP0";
const char* password = "password";
WiFiServer server(80);

String header;

int headlightinroom = 1;  //Stan œwiat³a
int headwentinroom = 1;   //Stan wentylatora

void KolorujCalySufit(uint8_t ColorR, uint8_t ColorG, uint8_t ColorB) {  //RGB
  CAN_frame_t tx_frame;
  tx_frame.FIR.B.FF = CAN_frame_std;
  tx_frame.MsgID = 0x011;
  tx_frame.FIR.B.DLC = 3;
  tx_frame.data.u8[0] = ColorR;
  tx_frame.data.u8[1] = ColorG;
  tx_frame.data.u8[2] = ColorB;
  //delay(1);
  ESP32Can.CANWriteFrame(&tx_frame);
}

void KolorujJedenPasek(uint8_t ColorR, uint8_t ColorG, uint8_t ColorB, uint8_t NrLedStrip) { //RGB+Nr_Paska
  CAN_frame_t tx_frame;
  tx_frame.FIR.B.FF = CAN_frame_std;
  tx_frame.MsgID = 0x011;
  tx_frame.FIR.B.DLC = 4;
  tx_frame.data.u8[0] = ColorR;
  tx_frame.data.u8[1] = ColorG;
  tx_frame.data.u8[2] = ColorB;
  tx_frame.data.u8[3] = NrLedStrip;
  //delay(1);
  ESP32Can.CANWriteFrame(&tx_frame);
}

void KolorujJednegoLeda(uint8_t ColorR, uint8_t ColorG, uint8_t ColorB, uint8_t NrLedStrip, uint16_t NrLedInStrip) {  //RGB+Nr_Paska+Nr_Led
  CAN_frame_t tx_frame;
  tx_frame.FIR.B.FF = CAN_frame_std;
  tx_frame.MsgID = 0x011;
  tx_frame.FIR.B.DLC = 5;
  tx_frame.data.u8[0] = ColorR;
  tx_frame.data.u8[1] = ColorG;
  tx_frame.data.u8[2] = ColorB;
  tx_frame.data.u8[3] = NrLedStrip;
  tx_frame.data.u8[4] = NrLedInStrip;
  //delay(1);
  ESP32Can.CANWriteFrame(&tx_frame);
}

void KolorujLedOdAdoZ(uint8_t ColorR, uint8_t ColorG, uint8_t ColorB, uint8_t NrLedStrip, int NrLedAStrip, int NrLedBStrip) {  //RGB+_NrPaska+LedOd+LedDo
  CAN_frame_t tx_frame;
  tx_frame.FIR.B.FF = CAN_frame_std;
  tx_frame.MsgID = 0x011;
  tx_frame.FIR.B.DLC = 6;
  tx_frame.data.u8[0] = ColorR;
  tx_frame.data.u8[1] = ColorG;
  tx_frame.data.u8[2] = ColorB;
  tx_frame.data.u8[3] = NrLedStrip;
  tx_frame.data.u8[4] = NrLedAStrip;
  tx_frame.data.u8[5] = NrLedBStrip;
  //delay(1);
  ESP32Can.CANWriteFrame(&tx_frame);
}

void HeadAnimation(uint8_t StyleAnimation, uint8_t ValueAnimation) {
  CAN_frame_t tx_frame;
  tx_frame.FIR.B.FF = CAN_frame_std;
  tx_frame.MsgID = 0x010;
  tx_frame.FIR.B.DLC = 2;
  tx_frame.data.u8[0] = StyleAnimation;
  tx_frame.data.u8[1] = ValueAnimation;
  //delay(1);
  ESP32Can.CANWriteFrame(&tx_frame);
}

void KolorujWlacznik(uint8_t ButtonColorHSV, uint16_t ButtonBright) {  //RGB+Nr_Paska+Nr_Led
  CAN_frame_t tx_frame;
  tx_frame.FIR.B.FF = CAN_frame_std;
  tx_frame.MsgID = 0x012;
  tx_frame.FIR.B.DLC = 2;
  tx_frame.data.u8[0] = ButtonColorHSV;
  tx_frame.data.u8[1] = ButtonBright;
  //delay(1);
  ESP32Can.CANWriteFrame(&tx_frame);
}


uint32_t CurrentTime = millis();
uint32_t previousTime = 0;
uint16_t timeoutTime = 800;
uint32_t HoldTimeButton[1] = { 0 };
uint8_t HoldStateButton[1] = { 0 };

void setup() {
  //Serial.begin(115200);
  CAN_cfg.speed = CAN_SPEED_125KBPS; //CAN_SPEED_125KBPS;
  CAN_cfg.tx_pin_id = GPIO_NUM_22;
  CAN_cfg.rx_pin_id = GPIO_NUM_23;
  CAN_cfg.rx_queue = xQueueCreate(rx_queue_size, sizeof(CAN_frame_t));
  ESP32Can.CANInit();

  //Przyciski
  pinMode(39, INPUT);	//w³¹cznik
  pinMode(36, INPUT); //wolny
  pinMode(35, INPUT); //wolny
  pinMode(34, INPUT); //wolny

  pinMode(32, OUTPUT);	//swiatlo
  pinMode(33, OUTPUT);	//went

  digitalWrite(32, 1);  //Zgas swiatlo
  digitalWrite(33, 1);  //Wylacz went

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  server.begin();
}

void loop() {
  //Can_reader();
  LightButton();
  //wifi
  WiFiClient client = server.available();

  if (client) {
    CurrentTime = millis();
    previousTime = CurrentTime;
    String currentLine = "";
    while (client.connected() && CurrentTime - previousTime <= timeoutTime) {
      CurrentTime = millis();
      if (client.available()) {
        char c = client.read();
        header += c;
        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            client.println("<link rel=\"stylesheet\" href=\"https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css\">");
            client.println("<script src=\"https://cdnjs.cloudflare.com/ajax/libs/jscolor/2.0.4/jscolor.min.js\"></script>");
            client.println("</head><body><div class=\"container\"><div class=\"row\"><h1>Caly sufit</h1></div>");
            client.println("<a class=\"btn btn-primary btn-lg\" href=\"#\" id=\"change_color\" role=\"button\">Change Color</a> ");
            client.println("<input class=\"jscolor {onFineChange:'update(this)'}\" id=\"rgb\"></div>");
            client.println("<script>function update(picker) {document.getElementById('rgb').innerHTML = Math.round(picker.rgb[0]) + ', ' +  Math.round(picker.rgb[1]) + ', ' + Math.round(picker.rgb[2]);");
            client.println("document.getElementById(\"change_color\").href=\"?lall=r\" + Math.round(picker.rgb[0]) + \"g\" +  Math.round(picker.rgb[1]) + \"b\" + Math.round(picker.rgb[2]) + \"&\";}</script>");
            client.println("<br/><br/><p>Panel led: ");
            client.println(headlightinroom);
            client.println("</p>");
            client.println("<p>Wentylator: ");
            client.println(headwentinroom);
            client.println("</p>");
            client.println("</body></html>");
            client.println();

             //Œwiat³o w pokoju
            if (header.indexOf("GET /?light=") >= 0) {	//random led /?light=s1&

              int ledinfoS = header.indexOf('s');
              int ledinfoA = header.indexOf('&');

              String lightonString = header.substring(ledinfoS + 1, ledinfoA);
              headlightinroom = lightonString.toInt();  //Zapisanie prze³¹czonego stanu œwiat³a

              //Niech wysy³a jeszcze CAN info o tym

              digitalWrite(32, headlightinroom);
            }
            else
              if (header.indexOf("GET /?went=") >= 0) {//random led 

                int ledinfoS = header.indexOf('s');
                int ledinfoA = header.indexOf('&');

                String wentonString = header.substring(ledinfoS + 1, ledinfoA);
                headwentinroom = wentonString.toInt();  //Zapisanie prze³¹czonego stanu wentylatora

                //Niech wysy³a jeszcze CAN info o tym

                digitalWrite(33, headwentinroom);
              }else
                if (header.indexOf("GET /?lall=") >= 0) {   //Ca³y sufit

                  int ledinfoR = header.indexOf('r');
                  int ledinfoG = header.indexOf('g');
                  int ledinfoB = header.indexOf('b');
                  int ledinfoC = header.indexOf('&');

                  String redString   = header.substring(ledinfoR + 1, ledinfoG);
                  String greenString = header.substring(ledinfoG + 1, ledinfoB);
                  String blueString  = header.substring(ledinfoB + 1, ledinfoC);

                  KolorujCalySufit(redString.toInt(), greenString.toInt(), blueString.toInt());
                }else
                  if (header.indexOf("GET /?ltone=") >= 0) {    //Jeden ca³y pasek

                    int ledinfoS = header.indexOf('s');
                    int ledinfoR = header.indexOf('r');
                    int ledinfoG = header.indexOf('g');
                    int ledinfoB = header.indexOf('b');
                    int ledinfoC = header.indexOf('&');

                    String stringled = header.substring(ledinfoS + 1, ledinfoR);
                    String redString = header.substring(ledinfoR + 1, ledinfoG);
                    String greenString = header.substring(ledinfoG + 1, ledinfoB);
                    String blueString = header.substring(ledinfoB + 1, ledinfoC);

                    KolorujJedenPasek(redString.toInt(), greenString.toInt(), blueString.toInt(), stringled.toInt()); //RGB+Nr_Paska
                  }
                  else
                    if (header.indexOf("GET /?lone=") >= 0) {

                      int ledinfoS = header.indexOf('s');
                      int ledinfoD = header.indexOf('d');
                      int ledinfoR = header.indexOf('r');
                      int ledinfoG = header.indexOf('g');
                      int ledinfoB = header.indexOf('b');
                      int ledinfoC = header.indexOf('&');

                      String stringled = header.substring(ledinfoS + 1, ledinfoD);
                      String numberofled = header.substring(ledinfoD + 1, ledinfoR);
                      String redString = header.substring(ledinfoR + 1, ledinfoG);
                      String greenString = header.substring(ledinfoG + 1, ledinfoB);
                      String blueString = header.substring(ledinfoB + 1, ledinfoC);

                      KolorujJednegoLeda(redString.toInt(), greenString.toInt(), blueString.toInt(), stringled.toInt(), numberofled.toInt());
                    }
                    else
                      if (header.indexOf("GET /?laz=") >= 0) {

                        int ledinfoS = header.indexOf('s');
                        int ledinfoO = header.indexOf('o');
                        int ledinfoD = header.indexOf('d');
                        int ledinfoR = header.indexOf('r');
                        int ledinfoG = header.indexOf('g');
                        int ledinfoB = header.indexOf('b');
                        int ledinfoC = header.indexOf('&');

                        String stringled = header.substring(ledinfoS + 1, ledinfoO);
                        String numberodled = header.substring(ledinfoO + 1, ledinfoD);
                        String numberdoled = header.substring(ledinfoD + 1, ledinfoR);
                        String redString = header.substring(ledinfoR + 1, ledinfoG);
                        String greenString = header.substring(ledinfoG + 1, ledinfoB);
                        String blueString = header.substring(ledinfoB + 1, ledinfoC);

                        KolorujLedOdAdoZ(redString.toInt(), greenString.toInt(), blueString.toInt(), stringled.toInt(), numberodled.toInt(), numberdoled.toInt());
                      }
                      else
                        if (header.indexOf("GET /?ani=") >= 0) {

                          int ledinfoS = header.indexOf('s');
                          int ledinfoW = header.indexOf('w');
                          int ledinfoC = header.indexOf('&');

                          String StyleAnimation = header.substring(ledinfoS + 1, ledinfoW);
                          String ValueAnimation = header.substring(ledinfoW + 1, ledinfoC);

                          HeadAnimation(StyleAnimation.toInt(), ValueAnimation.toInt());
                        }
          break;
          }else {
            currentLine = "";
          }
        }
        else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    header = "";
    client.stop();
  }
}

void LightButton() {  //W³¹cznik œwiat³a w pokoju

  if (digitalRead(39) == 1) { //Jeœli przycisk wciœniêty
    if (HoldStateButton[0] == 0) {  //Czy pêlta by³a ju¿ wykonana?
      HoldStateButton[0] = 1;       //Zaznacz wykonanie pêtli
      HoldTimeButton[0] = millis(); //Zapisz czas zaczêcia wykonywania pêtli
      KolorujWlacznik(0,10);
    }

    if ((millis() - HoldTimeButton[0]) == 2400) {  //Czy by³ trzymany wiêcej ni¿ 2400ms?
      KolorujWlacznik(213, 10);
    }else if ((millis() - HoldTimeButton[0]) == 1200) {  //Czy by³ trzymany wiêcej ni¿ 1200ms?
      KolorujWlacznik(170, 10);
    }
  }
  else if (HoldStateButton[0] == 1) {  //Czy przycisk by³ ju¿ trzymany? Tak
    if (digitalRead(39) == 0) {   //Czy przycisk jest ju¿ puszczony?
      HoldStateButton[0] = 0;     //Zaznacz ¿e jest ju¿ puszczony
      KolorujWlacznik(0, 0);
      if ((millis() - HoldTimeButton[0]) > 2400) {  //Czy by³ trzymany wiêcej ni¿ 2400ms?
        KolorujCalySufit(0, 0, 0);
      }
      else
        if ((millis() - HoldTimeButton[0]) > 1200) {  //Czy by³ trzymany wiêcej ni¿ 1200ms?
          if (headwentinroom == 0) { headwentinroom = 1; }  //Zmieñ stan wentylatora
          else
            if (headwentinroom == 1) { headwentinroom = 0; }  //Zmieñ stan wentylatora
          digitalWrite(33, headwentinroom);   //Wyœlij stan wentylatora
        }
        else
          if ((millis() - HoldTimeButton[0]) > 20) {  //Czy by³ trzymany wiêcej ni¿ 20ms?
            if (headlightinroom == 0) { headlightinroom = 1; }  //Zgaœ lub zapal œwiat³o
            else
              if (headlightinroom == 1) { headlightinroom = 0; }  //Zgaœ lub zapal œwiat³o
            digitalWrite(32, headlightinroom);      //Wyœlij stan œwiat³a
          }
    }
  }
}


void Can_reader() //Odbieranie danych z Cana
{
  CAN_frame_t rx_frame;
  unsigned long cMillisInReadCan = millis();

  if (xQueueReceive(CAN_cfg.rx_queue, &rx_frame, 3 * portTICK_PERIOD_MS) == pdTRUE) {

    if (rx_frame.MsgID == 0x003) {	//Informacja temperatury
        //Pakowanie temp w zmienna
    }

  }

}

