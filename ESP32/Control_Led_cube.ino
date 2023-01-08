#include <WiFi.h>
#include <HTTPClient.h>
#include "Adafruit_APDS9960.h"
Adafruit_APDS9960 apds;

const char* ssid = "NeeNeet-AP0";
const char* password = "";

String serverName = "http://10.10.10.101/?";

void setup() {
  Serial.begin(115200); //usun¹æ

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }

  apds.begin();
  apds.enableProximity(true);
  apds.enableGesture(true);
}

uint8_t ArrowGestureInt = 0;
uint8_t LedSendR = 0;
uint8_t LedSendG = 0;
uint8_t LedSendB = 0;
void loop() {

  uint8_t gesture = apds.readGesture();
  if (gesture == APDS9960_DOWN) ArrowGestureInt=1;
  if (gesture == APDS9960_UP) ArrowGestureInt=2;
  if (gesture == APDS9960_LEFT) ArrowGestureInt=3;
  if (gesture == APDS9960_RIGHT) ArrowGestureInt=4;


  if ((WiFi.status() == WL_CONNECTED) && ArrowGestureInt>0) { //Jeœli po³¹czono z WiFi

    switch (ArrowGestureInt){
    case 1:   
        LedSendR = 0; LedSendG = 0; LedSendB = 0;   break;
    case 2:
      LedSendR = 0; LedSendG = 255; LedSendB = 0;   break;
    case 3:
      LedSendR = 0; LedSendG = 0; LedSendB = 255;   break;
    case 4:
      LedSendR = 255; LedSendG = 0; LedSendB = 0;   break;
    }

    HTTPClient http;

    String serverPath = serverName + "lall=r" + String(LedSendR) + "g" + String(LedSendG) + "b" + String(LedSendB) + "&";   //Wybieranie koloru

    http.begin(serverPath.c_str()); //Wejœcie na link z danymi sparsowanymi
    int httpCode = http.GET();                                        //Make the request

    if (httpCode > 0) { //Check for the returning code
      //Wys³ano i odebrano
    } else {
      //Nie wys³ano, jakiœ error
    }

    http.end(); //Zakoñczenie geta

    ArrowGestureInt = 0;
  }
  delay(10);
}