#include <Arduino.h>
#include <ESP32CAN.h>
#include <CAN_config.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_NeoPixel.h>

//NeoPixel
Adafruit_NeoPixel lsu1(120, 15, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel lsu2(120, 2,  NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel lsu3(104, 4,  NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel lsu4(104, 16, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel lsu5(88,  17, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel lsu6(88,  5,  NEO_GRB + NEO_KHZ800);

Adafruit_NeoPixel lsz1(120, 18, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel lsz2(120, 19, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel lsz3(104, 21, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel lsz4(104, 13, NEO_GRB + NEO_KHZ800); //13	//kiedys 12
Adafruit_NeoPixel lsz5(88,  12, NEO_GRB + NEO_KHZ800); //12 //kiedys 13
Adafruit_NeoPixel lsz6(88,  14, NEO_GRB + NEO_KHZ800); //14 //kiedys 13

Adafruit_NeoPixel lsw(2, 27, NEO_GRB + NEO_KHZ800);	//27 - włącznik

//CAN
CAN_device_t CAN_cfg;

//Termometry
OneWire oneWire(32);  //Pin OneWire DS temperature
DallasTemperature sentemp(&oneWire);  //Pass our oneWire reference to Dallas Temperature.

DeviceAddress sentempid1 = { 0x28, 0x50, 0x3B, 0x07, 0xD6, 0x01, 0x3C, 0x53 };
DeviceAddress sentempid2 = { 0x28, 0x9E, 0xD8, 0x75, 0xD0, 0x01, 0x3C, 0x35 };

//Zmienne globalne



//==================================================================================\/
//===========================WYZWALANIE=WYSYŁANIA=DANYCH=DO=LED=====================\/
//==================================================================================\/

int mySensVals[13] = {0,0,0,0,0,0,0,0,0,0,0,0,0}; //Czy aktualizować pasek? //Taśmy led + Włącznik

void LedStripShowUpdate(int NrLedStrip){  //Aktualizowanie wybranej wcześniej taśmy led
  switch(NrLedStrip){
    case 0:   lsu1.show(); break;
    case 1:   lsu2.show(); break;
    case 2:   lsu3.show(); break;
    case 3:   lsu4.show(); break;
    case 4:   lsu5.show(); break;
    case 5:   lsu6.show(); break;
    case 6:   lsz6.show(); break;
    case 7:   lsz5.show(); break;
    case 8:   lsz4.show(); break;
    case 9:   lsz3.show(); break;
    case 10:  lsz2.show(); break;
    case 11:  lsz1.show(); break;
    case 12:  lsw.show();  break;
  }
}

void WhichLedStripUpdate(){ //Sprawdzanie która taśme led wysłać do aktualizacji
  for(int i=0;i<13;i++){
    if(mySensVals[i]){
      LedStripShowUpdate(i);
    }
  }
}

//==================================================================================\/
//==========================KOLOROWANIE=SUFITU=VOIDY=KOLOROWANIA====================\/
//==================================================================================\/

void KolorujPasekWPasku(int ColorR, int ColorG, int ColorB, int NrLedStrip){
  switch(NrLedStrip){
    case 0:   for(int i=0; i<120; i++){lsu1.setPixelColor(i,ColorR,ColorG,ColorB);} break;
    case 1:   for(int i=0; i<120; i++){lsu2.setPixelColor(i,ColorR,ColorG,ColorB);} break;
    case 2:   for(int i=0; i<104; i++){lsu3.setPixelColor(i,ColorR,ColorG,ColorB);} break;
    case 3:   for(int i=0; i<104; i++){lsu4.setPixelColor(i,ColorR,ColorG,ColorB);} break;
    case 4:   for(int i=0; i<88;  i++){lsu5.setPixelColor(i,ColorR,ColorG,ColorB);} break;
    case 5:   for(int i=0; i<88;  i++){lsu6.setPixelColor(i,ColorR,ColorG,ColorB);}	break;
    case 6:   for(int i=0; i<88;  i++){lsz6.setPixelColor(i,ColorR,ColorG,ColorB);} break;
    case 7:   for(int i=0; i<88;  i++){lsz5.setPixelColor(i,ColorR,ColorG,ColorB);} break;
    case 8:   for(int i=0; i<104; i++){lsz4.setPixelColor(i,ColorR,ColorG,ColorB);} break;
    case 9:   for(int i=0; i<104; i++){lsz3.setPixelColor(i,ColorR,ColorG,ColorB);} break;
    case 10:  for(int i=0; i<120; i++){lsz2.setPixelColor(i,ColorR,ColorG,ColorB);} break;
    case 11:  for(int i=0; i<120; i++){lsz1.setPixelColor(i,ColorR,ColorG,ColorB);} break;
  }
  mySensVals[NrLedStrip]=1; //Zaznaczenie numeru taśmy led do aktualizacji
}

void KolorujDwaWJednejListwie(int ColorR, int ColorG, int ColorB, int NrLedStrip){   //Koloruj po dwa paski na raz w jednej listwie led
  NrLedStrip*=2;
  KolorujPasekWPasku(ColorR, ColorG, ColorB, NrLedStrip);
  NrLedStrip++;
  KolorujPasekWPasku(ColorR, ColorG, ColorB, NrLedStrip);
}

void KolorujWzorekLedowyListwy(int ColorR, int ColorG, int ColorB, int NrLedStrip){    //Kolorowanie wzorka z tasm led
  switch(NrLedStrip){   //Legenda: X-(On) Pasek Led, O-(Off) pasek led, [] - Duża lampa na suficie
    case 20:
      KolorujDwaWJednejListwie(ColorR, ColorG, ColorB, 2);
      KolorujDwaWJednejListwie(ColorR, ColorG, ColorB, 3);   //    XOX [] XOX
    case 21:
      KolorujDwaWJednejListwie(ColorR, ColorG, ColorB, 0);
      KolorujDwaWJednejListwie(ColorR, ColorG, ColorB, 5);   //    XOO [] OOX
    break;
    case 22:
      KolorujDwaWJednejListwie(ColorR, ColorG, ColorB, 0);
      KolorujDwaWJednejListwie(ColorR, ColorG, ColorB, 5);   //    XXO [] OXX
    case 23:
      KolorujDwaWJednejListwie(ColorR, ColorG, ColorB, 1);
      KolorujDwaWJednejListwie(ColorR, ColorG, ColorB, 4);   //    OXO [] OXO
    break;
    case 24:
      KolorujDwaWJednejListwie(ColorR, ColorG, ColorB, 1);
      KolorujDwaWJednejListwie(ColorR, ColorG, ColorB, 4);   //    OXX [] XXO
    case 25:
      KolorujDwaWJednejListwie(ColorR, ColorG, ColorB, 2);
      KolorujDwaWJednejListwie(ColorR, ColorG, ColorB, 3);   //    OOX [] XOO
    break;

    case 26:
      KolorujDwaWJednejListwie(ColorR, ColorG, ColorB, 1);   //    OXX [] XXX
    case 27:
      KolorujDwaWJednejListwie(ColorR, ColorG, ColorB, 2);   //    OOX [] XXX
    case 28:
      KolorujDwaWJednejListwie(ColorR, ColorG, ColorB, 3);   //    OOO [] XXX
    case 29:
      KolorujDwaWJednejListwie(ColorR, ColorG, ColorB, 4);
      KolorujDwaWJednejListwie(ColorR, ColorG, ColorB, 5);   //    OOO [] OXX
    break;
    case 30:
      KolorujDwaWJednejListwie(ColorR, ColorG, ColorB, 4);   //    XXX [] XXO
    case 31:
      KolorujDwaWJednejListwie(ColorR, ColorG, ColorB, 3);   //    XXX [] XOO
    case 32:
      KolorujDwaWJednejListwie(ColorR, ColorG, ColorB, 2);   //    XXX [] OOO
    case 33:
      KolorujDwaWJednejListwie(ColorR, ColorG, ColorB, 1);
      KolorujDwaWJednejListwie(ColorR, ColorG, ColorB, 0);   //    XXO [] OOO
    break;
    
    case 99:  for(int i=0; i<2; i++){ lsw.setPixelColor(i,ColorR,ColorG,ColorB);} break;  //Kolorowanie całego włącznika
  }
}

void KolorujJednaDiode(int ColorR, int ColorG, int ColorB, int NrLedStrip, int NrLedInStrip){
  switch(NrLedStrip){
    case 0:   lsu1.setPixelColor(NrLedInStrip,ColorR,ColorG,ColorB);  break;
    case 1:   lsu2.setPixelColor(NrLedInStrip,ColorR,ColorG,ColorB);  break;
    case 2:   lsu3.setPixelColor(NrLedInStrip,ColorR,ColorG,ColorB);  break;
    case 3:   lsu4.setPixelColor(NrLedInStrip,ColorR,ColorG,ColorB);  break;
    case 4:   lsu5.setPixelColor(NrLedInStrip,ColorR,ColorG,ColorB);  break;
    case 5:   lsu6.setPixelColor(NrLedInStrip,ColorR,ColorG,ColorB);  break;
    case 6:   lsz6.setPixelColor(NrLedInStrip,ColorR,ColorG,ColorB);  break;
    case 7:   lsz5.setPixelColor(NrLedInStrip,ColorR,ColorG,ColorB);  break;
    case 8:   lsz4.setPixelColor(NrLedInStrip,ColorR,ColorG,ColorB);  break;
    case 9:   lsz3.setPixelColor(NrLedInStrip,ColorR,ColorG,ColorB);  break;
    case 10:  lsz2.setPixelColor(NrLedInStrip,ColorR,ColorG,ColorB);  break;
    case 11:  lsz1.setPixelColor(NrLedInStrip,ColorR,ColorG,ColorB);  break;
  }
}

void KolorujDwieDiodyWListwie(int ColorR, int ColorG, int ColorB, int NrLedStrip, int NrLedInStrip){
  NrLedStrip*=2;
  KolorujJednaDiode(ColorR, ColorG, ColorB, NrLedStrip, NrLedInStrip);
  NrLedStrip+=1;
  KolorujJednaDiode(ColorR, ColorG, ColorB, NrLedStrip, NrLedInStrip);
}

void KolorujWzorekLedowyDiody(int ColorR, int ColorG, int ColorB, int NrLedStrip, int NrLedInStrip){    //Kolorowanie wzorka pojedynczych led 
  switch(NrLedStrip){ 
    case 20:
      KolorujDwaWJednejListwie(ColorR, ColorG, ColorB, 2);
      KolorujDwaWJednejListwie(ColorR, ColorG, ColorB, 3);   //    XOX [] XOX
    case 21:
      KolorujDwaWJednejListwie(ColorR, ColorG, ColorB, 0);
      KolorujDwaWJednejListwie(ColorR, ColorG, ColorB, 5);   //    XOO [] OOX
    break;

    case 99: lsw.setPixelColor(NrLedInStrip,ColorR,ColorG,ColorB);  break;
  }
}



//==================================================================================\/
//============================KOLOROWANIE=SUFITU=GŁÓWNE=VOIDY=======================\/
//==================================================================================\/

void AnimacjaSufitu(int NrAnimation, int BrightAnimation){  //Nr_Animacji+Jasność

}

void KolorujCalySufit(int ColorR, int ColorG, int ColorB){  //RGB
  for(int i=0;i<6;i++){
    KolorujDwaWJednejListwie(ColorR, ColorG, ColorB, i);
  }
}

void KolorujJedenPasek(int ColorR, int ColorG, int ColorB, int NrLedStrip){ //RGB+Nr_Paska
  if(NrLedStrip<6){             //Kolorowanie dwóch pasków na raz w jednej listwie led
    KolorujDwaWJednejListwie(ColorR, ColorG, ColorB, NrLedStrip);
  }else if(NrLedStrip>19){      //Zapalanie po kilka pasków i łatwe tworzenie wzorów
    KolorujWzorekLedowyListwy(ColorR, ColorG, ColorB, NrLedStrip);
  }else if(NrLedStrip>5){       //Kolorowanie tylko jednego paska z dwóch w jednej listwie led.
    NrLedStrip-=6;
    KolorujPasekWPasku(ColorR, ColorG, ColorB, NrLedStrip);
  }
}

void KolorujJednegoLeda(int ColorR, int ColorG, int ColorB, int NrLedStrip, int NrLedInStrip){  //RGB+Nr_Paska+Nr_Led
  if(NrLedStrip<6){         //Zapalanie dwie diody w listwie
    KolorujDwieDiodyWListwie(ColorR, ColorG, ColorB, NrLedStrip, NrLedInStrip);
  }else if(NrLedStrip>19){  //Zapalanie kilka ledów - jakieś animacje czy coś

    KolorujJednaDiode(ColorR, ColorG, ColorB, NrLedStrip, NrLedInStrip);

  }else if(NrLedStrip>5){   //Zapalanie tylko jednej diody w listwie
    NrLedStrip-=6;
    KolorujJednaDiode(ColorR, ColorG, ColorB, NrLedStrip, NrLedInStrip);
  }
}

void KolorujLedOdAdoZ(int ColorR, int ColorG, int ColorB, int NrLedStrip, int NrLedAStrip, int NrLedBStrip){  //RGB+_NrPaska+LedOd+LedDo

}

//==================================================================================\/
//=======================================CAN=READ===================================\/
//==================================================================================\/

void Can_reader() //Odbieranie danych z Cana
{
  CAN_frame_t rx_frame;
  unsigned long cMillisInReadCan = millis();

  if (xQueueReceive(CAN_cfg.rx_queue, &rx_frame, 3 * portTICK_PERIOD_MS) == pdTRUE) {

    if (rx_frame.MsgID == 0x011) {	//Informacja tylko do sufitu

      switch (rx_frame.FIR.B.DLC) {   //Sprawdzanie długości ciągu
        case 2:
          AnimacjaSufitu(rx_frame.data.u8[0],rx_frame.data.u8[1]);  //Nr_Animacji+Jasność
        break;
        case 3:
          KolorujCalySufit(rx_frame.data.u8[0],rx_frame.data.u8[1],rx_frame.data.u8[2]);  //RGB
        break;
        case 4:
          KolorujJedenPasek(rx_frame.data.u8[0],rx_frame.data.u8[1],rx_frame.data.u8[2],rx_frame.data.u8[3]); //RGB+Nr_Paska
        break;
        case 5:
          KolorujJednegoLeda(rx_frame.data.u8[0],rx_frame.data.u8[1],rx_frame.data.u8[2],rx_frame.data.u8[3],rx_frame.data.u8[4]);  //RGB+Nr_Paska+Nr_Led
        break;
        case 6:
          KolorujLedOdAdoZ(rx_frame.data.u8[0],rx_frame.data.u8[1],rx_frame.data.u8[2],rx_frame.data.u8[3],rx_frame.data.u8[4],rx_frame.data.u8[5]);  //RGB+_NrPaska+LedOd+LedDo
        break;
      }
    }

    if (rx_frame.MsgID == 0x012) {	//Informacja tylko do włącznika światła

    }

    if (rx_frame.MsgID == 0x020) {	//Informacja Animacji

    }
  }

}

//==================================================================================\/
//========================================VOIDY=====================================\/
//==================================================================================\/


void setup() {
  //CAN
  CAN_cfg.speed = CAN_SPEED_250KBPS; //CAN_SPEED_125KBPS;
  CAN_cfg.tx_pin_id = GPIO_NUM_23;
  CAN_cfg.rx_pin_id = GPIO_NUM_22;
  CAN_cfg.rx_queue = xQueueCreate(10, sizeof(CAN_frame_t));
  ESP32Can.CANInit();

  //Temperature
  sentemp.begin();

  //Neopixel activate led strip
  lsu1.begin();
  lsu2.begin();
  lsu3.begin();
  lsu4.begin();
  lsu5.begin();
  lsu6.begin();
  lsz1.begin();
  lsz2.begin();
  lsz3.begin();
  lsz4.begin();
  lsz5.begin();
  lsz6.begin();
  lsw.begin();

  // Neopixel Initialize all pixels to 'off'
  KolorujCalySufit(0,0,0);
  lsw.show();
}

void loop() {
  Can_reader(); //Odbieranie danych z Cana



  WhichLedStripUpdate();  //Które paski led aktualizować?
}
