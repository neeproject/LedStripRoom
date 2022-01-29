#include <Arduino.h>
#include <ESP32CAN.h>
#include <CAN_config.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_NeoPixel.h>

#include <cstdint>  //Potem usunąć, potrzebne tylko do VS, aby podświetlać inty.

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

Adafruit_NeoPixel lsw(2,    27, NEO_GRB + NEO_KHZ800);	//27 - włącznik
Adafruit_NeoPixel lwent(15, 33, NEO_GRB + NEO_KHZ800);	//27 - włącznik

//CAN
CAN_device_t CAN_cfg;

//Termometry
OneWire oneWire(32);  //Pin OneWire DS temperature
DallasTemperature sentemp(&oneWire);  //Pass our oneWire reference to Dallas Temperature.

DeviceAddress sentempid1 = { 0x28, 0x50, 0x3B, 0x07, 0xD6, 0x01, 0x3C, 0x53 };
DeviceAddress sentempid2 = { 0x28, 0x9E, 0xD8, 0x75, 0xD0, 0x01, 0x3C, 0x35 };

//Zmienne globalne

uint8_t ModeAnimation = 0;
uint32_t PreviousTimeAnimation = 0;
uint16_t DelayTimeAnimation = 20;

//==================================================================================\/
//=====================================GOTOWE=KOLORY================================\/
//==================================================================================\/

uint32_t ColorLibrayArray[12] = {  //Tablica kolorów.
lsu1.Color(255, 0, 0),    //red
lsu1.Color(255, 128, 0),  //orange
lsu1.Color(255, 255, 0),  //yellow
lsu1.Color(128, 255, 0),  //green-yellow
lsu1.Color(0, 255, 0),    //green
lsu1.Color(0, 255, 128),  //green-blu
lsu1.Color(0, 255, 255),  //Cyan
lsu1.Color(0, 128, 255),  //Brandeis Blue
lsu1.Color(0, 0, 255),   //Blue
lsu1.Color(128, 0, 255),  //indygo (perpul)
lsu1.Color(255, 0, 255),  //Magenta
lsu1.Color(255, 0, 128)  //Rose
};
//==================================================================================\/
//===========================WYZWALANIE=WYSYŁANIA=DANYCH=DO=LED=====================\/
//==================================================================================\/

uint8_t ListUpdateLedStrip[14] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0}; //Czy aktualizować pasek? //Taśmy led + Włącznik + Obudowa went

//Aktualizowanie wybranej wcześniej taśmy led
void LedStripShowUpdate(uint8_t NrLedStrip){
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
    case 12:   lsw.show(); break;
    case 13: lwent.show(); break;
  }
}

//Sprawdzanie która taśme led wysłać do aktualizacji
void WhichLedStripUpdate(){
  for(uint8_t i=0;i<14;i++){
    if(ListUpdateLedStrip[i]){
      LedStripShowUpdate(i);
      ListUpdateLedStrip[i] = 0;
    }
  }
}

//==================================================================================\/
//========================KOLOROWANIE=SUFITU=PODSTAWA=KOLOROWANIA===================\/
//==================================================================================\/

void KolorowanieTylkoJednegoLeda(uint8_t ColorR, uint8_t ColorG, uint8_t ColorB, uint8_t NrLedStrip, int NrLedInStrip){
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
    case 12:   lsw.setPixelColor(NrLedInStrip,ColorR,ColorG,ColorB);  break;
    case 13: lwent.setPixelColor(NrLedInStrip,ColorR,ColorG,ColorB);  break;
  }
  ListUpdateLedStrip[NrLedStrip]=1;
}


void KolorowanieDwochLedow(uint8_t ColorR, uint8_t ColorG, uint8_t ColorB, uint8_t NrLedStrip, int NrLedInStrip){ //NrLedStrip 0-5
  NrLedStrip*=2;
  KolorowanieTylkoJednegoLeda(ColorR, ColorG, ColorB, NrLedStrip, NrLedInStrip);
  NrLedStrip+=1;
  KolorowanieTylkoJednegoLeda(ColorR, ColorG, ColorB, NrLedStrip, NrLedInStrip);
}

void KolorowanieWiecejNizDwochDiod(uint8_t ColorR, uint8_t ColorG, uint8_t ColorB, uint8_t NrLedStrip, int NrLedInStrip) {
  switch (NrLedStrip) {
  case 20:   //Normalnie i od końca
    KolorowanieDwochLedow(ColorR, ColorG, ColorB, 0, NrLedInStrip);
    NrLedInStrip = 119 - NrLedInStrip;
    KolorowanieDwochLedow(ColorR, ColorG, ColorB, 0, NrLedInStrip);   //    HOO [] OOO
    break;
  case 21:
    KolorowanieDwochLedow(ColorR, ColorG, ColorB, 1, NrLedInStrip);
    NrLedInStrip = 103 - NrLedInStrip;
    KolorowanieDwochLedow(ColorR, ColorG, ColorB, 1, NrLedInStrip);   //    OHO [] OOO
    break;
  case 22:
    KolorowanieDwochLedow(ColorR, ColorG, ColorB, 2, NrLedInStrip);
    NrLedInStrip = 87 - NrLedInStrip;
    KolorowanieDwochLedow(ColorR, ColorG, ColorB, 2, NrLedInStrip);   //    OOH [] OOO
    break;
  case 23:
    KolorowanieDwochLedow(ColorR, ColorG, ColorB, 3, NrLedInStrip);
    NrLedInStrip = 87 - NrLedInStrip;
    KolorowanieDwochLedow(ColorR, ColorG, ColorB, 3, NrLedInStrip);   //    OOO [] HOO
    break;
  case 24:
    KolorowanieDwochLedow(ColorR, ColorG, ColorB, 4, NrLedInStrip);
    NrLedInStrip = 103 - NrLedInStrip;
    KolorowanieDwochLedow(ColorR, ColorG, ColorB, 4, NrLedInStrip);   //    OOO [] OHO
    break;
  case 25:
    KolorowanieDwochLedow(ColorR, ColorG, ColorB, 5, NrLedInStrip);
    NrLedInStrip = 119 - NrLedInStrip;
    KolorowanieDwochLedow(ColorR, ColorG, ColorB, 5, NrLedInStrip);   //    OOO [] OOH
    break;
  case 26:   //Zapalanie dwóch przeciwnych pasków na raz
    KolorowanieDwochLedow(ColorR, ColorG, ColorB, 0, NrLedInStrip);
    KolorowanieDwochLedow(ColorR, ColorG, ColorB, 5, NrLedInStrip);   //    XOO [] OOX
    break;
  case 27:
    KolorowanieDwochLedow(ColorR, ColorG, ColorB, 1, NrLedInStrip);
    KolorowanieDwochLedow(ColorR, ColorG, ColorB, 4, NrLedInStrip);   //    OXO [] OXO
    break;
  case 28:
    KolorowanieDwochLedow(ColorR, ColorG, ColorB, 2, NrLedInStrip);
    KolorowanieDwochLedow(ColorR, ColorG, ColorB, 3, NrLedInStrip);   //    OOX [] XOO
    break;
  case 29:   //Jeden normalnie, drugi odwrócony
    KolorowanieDwochLedow(ColorR, ColorG, ColorB, 0, NrLedInStrip);
    NrLedInStrip = 119 - NrLedInStrip;
    KolorowanieDwochLedow(ColorR, ColorG, ColorB, 5, NrLedInStrip);   //    XOO [] OO%
    break;
  case 30:
    KolorowanieDwochLedow(ColorR, ColorG, ColorB, 1, NrLedInStrip);
    NrLedInStrip = 103 - NrLedInStrip;
    KolorowanieDwochLedow(ColorR, ColorG, ColorB, 4, NrLedInStrip);   //    OXO [] O%O
    break;
  case 31:
    KolorowanieDwochLedow(ColorR, ColorG, ColorB, 2, NrLedInStrip);
    NrLedInStrip = 87 - NrLedInStrip;
    KolorowanieDwochLedow(ColorR, ColorG, ColorB, 3, NrLedInStrip);   //    OOX [] %OO
    break;

  }
}

void KolorowanieWiecejNizDwochDiodH(uint8_t ColorR, uint8_t ColorG, uint8_t ColorB, uint8_t NrLedStrip, int NrLedInStrip) {
  switch (NrLedStrip) {
    case 32:   //Zapalanie dwóch pasków po przeciwnych stronach z dwóch stron
      KolorowanieWiecejNizDwochDiod(ColorR, ColorG, ColorB, 20, NrLedInStrip);
      KolorowanieWiecejNizDwochDiod(ColorR, ColorG, ColorB, 25, NrLedInStrip);      //    HOO [] OOH
    break;
    case 33:
      KolorowanieWiecejNizDwochDiod(ColorR, ColorG, ColorB, 21, NrLedInStrip);
      KolorowanieWiecejNizDwochDiod(ColorR, ColorG, ColorB, 24, NrLedInStrip);      //    OHO [] OHO
    break;
    case 34:
      KolorowanieWiecejNizDwochDiod(ColorR, ColorG, ColorB, 22, NrLedInStrip);
      KolorowanieWiecejNizDwochDiod(ColorR, ColorG, ColorB, 23, NrLedInStrip);      //    OOH [] HOO
    break;
  }
}

//Kolorowanie taśm led od środka zakres ledów  NrLedInStrip od 0-59, 0-51, 0-43
void KolorowanieTasmOdSrodkaO(uint8_t ColorR, uint8_t ColorG, uint8_t ColorB, uint8_t NrLedStrip, int NrLedInStrip) {
  switch (NrLedStrip) {
  case 35:
    if (NrLedInStrip < 60) {
      int StripLong = NrLedInStrip + 60;
      KolorowanieWiecejNizDwochDiod(ColorR, ColorG, ColorB, 20, StripLong);   //    HOO [C] OOO  //59
    }
    if (NrLedInStrip < 52) {
      int StripMedium = NrLedInStrip + 52;
      KolorowanieWiecejNizDwochDiod(ColorR, ColorG, ColorB, 21, StripMedium); //    OHO [C] OOO  //51
    }
    if (NrLedInStrip < 44) {
      int StripShort = NrLedInStrip + 44;
      KolorowanieWiecejNizDwochDiod(ColorR, ColorG, ColorB, 22, StripShort);  //    OOH [C] OOO  //43
    }
    break;	//	HHH [C] OOO
  case 36:
    if (NrLedInStrip < 60) {
      int StripLong = NrLedInStrip + 60;
      KolorowanieWiecejNizDwochDiod(ColorR, ColorG, ColorB, 25, StripLong);   //    OOO [C] OOH  //59
    }
    if (NrLedInStrip < 52) {
      int StripMedium = NrLedInStrip + 52;
      KolorowanieWiecejNizDwochDiod(ColorR, ColorG, ColorB, 24, StripMedium); //    OOO [C] OHO  //51
    }
    if (NrLedInStrip < 44) {
      int StripShort = NrLedInStrip + 44;
      KolorowanieWiecejNizDwochDiod(ColorR, ColorG, ColorB, 23, StripShort);  //    OOO [C] HOO  //43
    }
    break;	//	OOO [C] HHH

  case 37:  //od birzegów
    if (NrLedInStrip < 60) {
      KolorowanieWiecejNizDwochDiod(ColorR, ColorG, ColorB, 20, NrLedInStrip);
      if (NrLedInStrip < 52) {
        KolorowanieWiecejNizDwochDiod(ColorR, ColorG, ColorB, 21, NrLedInStrip);
        if (NrLedInStrip < 44) {
          KolorowanieWiecejNizDwochDiod(ColorR, ColorG, ColorB, 22, NrLedInStrip);
        }
      }
    }
    break;//	HHH [C] OOO
  case 38:  //od brzegów
    if (NrLedInStrip < 60) {
      KolorowanieWiecejNizDwochDiod(ColorR, ColorG, ColorB, 25, NrLedInStrip);
      if (NrLedInStrip < 52) {
        KolorowanieWiecejNizDwochDiod(ColorR, ColorG, ColorB, 24, NrLedInStrip);
        if (NrLedInStrip < 44) {
          KolorowanieWiecejNizDwochDiod(ColorR, ColorG, ColorB, 23, NrLedInStrip);
        }
      }
    }
    break;//	OOO [C] HHH
  }

}

void KolorowanieTasmOdSrodkaH(uint8_t ColorR, uint8_t ColorG, uint8_t ColorB, uint8_t NrLedStrip, int NrLedInStrip) {
  switch (NrLedStrip) {
  case 40:  //Kolorowanie od środka pasków
    KolorowanieTasmOdSrodkaO(ColorR, ColorG, ColorB, 35, NrLedInStrip);
    KolorowanieTasmOdSrodkaO(ColorR, ColorG, ColorB, 36, NrLedInStrip);		//	HHH	[C]	HHH
    break;
  case 41:  //Kolorowanie od brzegów pasków
    KolorowanieTasmOdSrodkaO(ColorR, ColorG, ColorB, 37, NrLedInStrip);
    KolorowanieTasmOdSrodkaO(ColorR, ColorG, ColorB, 38, NrLedInStrip);		//	HHH	[C]	HHH
    break;
  }
}


//==================================================================================\/
//============================KOLOROWANIE=SUFITU=ANIMACJE=HSV=======================\/
//==================================================================================\/

//Jeden pasek w jednej listwie
void KolorowanieTasmHSV(uint32_t ColorFillHSV, uint16_t FirstLedHSV, uint16_t CountLedHSV, uint8_t NrLedStrip) {
  switch (NrLedStrip) {
  case 0:   lsu1.fill(ColorFillHSV, FirstLedHSV, CountLedHSV);  break;
  case 1:   lsu2.fill(ColorFillHSV, FirstLedHSV, CountLedHSV);  break;
  case 2:   lsu3.fill(ColorFillHSV, FirstLedHSV, CountLedHSV);  break;
  case 3:   lsu4.fill(ColorFillHSV, FirstLedHSV, CountLedHSV);  break;
  case 4:   lsu5.fill(ColorFillHSV, FirstLedHSV, CountLedHSV);  break;
  case 5:   lsu6.fill(ColorFillHSV, FirstLedHSV, CountLedHSV);  break;
  case 6:   lsz6.fill(ColorFillHSV, FirstLedHSV, CountLedHSV);  break;
  case 7:   lsz5.fill(ColorFillHSV, FirstLedHSV, CountLedHSV);  break;
  case 8:   lsz4.fill(ColorFillHSV, FirstLedHSV, CountLedHSV);  break;
  case 9:   lsz3.fill(ColorFillHSV, FirstLedHSV, CountLedHSV);  break;
  case 10:  lsz2.fill(ColorFillHSV, FirstLedHSV, CountLedHSV);  break;
  case 11:  lsz1.fill(ColorFillHSV, FirstLedHSV, CountLedHSV);  break;
  case 12:   lsw.fill(ColorFillHSV, FirstLedHSV, CountLedHSV);  break;
  case 13: lwent.fill(ColorFillHSV, FirstLedHSV, CountLedHSV);  break;
  }
  ListUpdateLedStrip[NrLedStrip] = 1;
}

//Dwa paski w jednej listwie
void KolorowanieTasmHSVDualLed(uint32_t ColorFillHSV, uint16_t FirstLedHSV, uint16_t CountLedHSV, uint8_t NrLedStrip) {
  switch (NrLedStrip) {
  case 0:   
    KolorowanieTasmHSV(ColorFillHSV, FirstLedHSV, CountLedHSV, 0);
    KolorowanieTasmHSV(ColorFillHSV, FirstLedHSV, CountLedHSV, 1);
    break;
  case 1:
    KolorowanieTasmHSV(ColorFillHSV, FirstLedHSV, CountLedHSV, 2);
    KolorowanieTasmHSV(ColorFillHSV, FirstLedHSV, CountLedHSV, 3);
    break;
  case 2:
    KolorowanieTasmHSV(ColorFillHSV, FirstLedHSV, CountLedHSV, 4);
    KolorowanieTasmHSV(ColorFillHSV, FirstLedHSV, CountLedHSV, 5);
    break;
  case 3:
    KolorowanieTasmHSV(ColorFillHSV, FirstLedHSV, CountLedHSV, 6);
    KolorowanieTasmHSV(ColorFillHSV, FirstLedHSV, CountLedHSV, 7);
    break;
  case 4:
    KolorowanieTasmHSV(ColorFillHSV, FirstLedHSV, CountLedHSV, 8);
    KolorowanieTasmHSV(ColorFillHSV, FirstLedHSV, CountLedHSV, 9);
    break;
  case 5:
    KolorowanieTasmHSV(ColorFillHSV, FirstLedHSV, CountLedHSV, 10);
    KolorowanieTasmHSV(ColorFillHSV, FirstLedHSV, CountLedHSV, 11);
    break;
  }
}

//Wzorki początkowe
void KolorowanieTasmHSVWzorkiWood(uint32_t ColorFillHSV, uint16_t FirstLedHSV, uint16_t CountLedHSV, uint8_t NrLedStrip) {
  switch (NrLedStrip) {
  case 20:    //    %OO [] OOO   //Na jednym pasku od dwóch stron na raz
    KolorowanieTasmHSVDualLed(ColorFillHSV, FirstLedHSV, CountLedHSV, 0);
    KolorowanieTasmHSVDualLed(ColorFillHSV, (119 - CountLedHSV - FirstLedHSV), CountLedHSV, 0);
    break;
  case 21:    //    O%O [] OOO
    KolorowanieTasmHSVDualLed(ColorFillHSV, FirstLedHSV, CountLedHSV, 1);
    KolorowanieTasmHSVDualLed(ColorFillHSV, (103 - CountLedHSV - FirstLedHSV), CountLedHSV, 1);
    break;
  case 22:    //    OO% [] OOO
    KolorowanieTasmHSVDualLed(ColorFillHSV, FirstLedHSV, CountLedHSV, 2);
    KolorowanieTasmHSVDualLed(ColorFillHSV, (87 - CountLedHSV - FirstLedHSV), CountLedHSV, 2);
    break;
  case 23:    //    OOO [] %OO
    KolorowanieTasmHSVDualLed(ColorFillHSV, FirstLedHSV, CountLedHSV, 3);
    KolorowanieTasmHSVDualLed(ColorFillHSV, (87 - CountLedHSV - FirstLedHSV), CountLedHSV, 3);
    break;
  case 24:    //    OOO [] O%O
    KolorowanieTasmHSVDualLed(ColorFillHSV, FirstLedHSV, CountLedHSV, 4);
    KolorowanieTasmHSVDualLed(ColorFillHSV, (103 - CountLedHSV - FirstLedHSV), CountLedHSV, 4);
    break;
  case 25:    //    OOO [] OO%
    KolorowanieTasmHSVDualLed(ColorFillHSV, FirstLedHSV, CountLedHSV, 5);
    KolorowanieTasmHSVDualLed(ColorFillHSV, (119 - CountLedHSV - FirstLedHSV), CountLedHSV, 5);
    break;

  case 26:    //    %OO [] OOO   //Na jednym pasku od dwóch stron na raz
    KolorowanieTasmHSVDualLed(ColorFillHSV, FirstLedHSV, CountLedHSV, 5);
    KolorowanieTasmHSVDualLed(ColorFillHSV, (119 - CountLedHSV - FirstLedHSV), CountLedHSV, 0);
    break;
  case 27:    //    O%O [] OOO
    KolorowanieTasmHSVDualLed(ColorFillHSV, FirstLedHSV, CountLedHSV, 4);
    KolorowanieTasmHSVDualLed(ColorFillHSV, (103 - CountLedHSV - FirstLedHSV), CountLedHSV, 1);
    break;
  case 28:    //    OO% [] OOO
    KolorowanieTasmHSVDualLed(ColorFillHSV, FirstLedHSV, CountLedHSV, 3);
    KolorowanieTasmHSVDualLed(ColorFillHSV, (87 - CountLedHSV - FirstLedHSV), CountLedHSV, 2);
    break;
  case 29:    //    OOO [] %OO
    KolorowanieTasmHSVDualLed(ColorFillHSV, FirstLedHSV, CountLedHSV, 2);
    KolorowanieTasmHSVDualLed(ColorFillHSV, (87 - CountLedHSV - FirstLedHSV), CountLedHSV, 3);
    break;
  case 30:    //    OOO [] O%O
    KolorowanieTasmHSVDualLed(ColorFillHSV, FirstLedHSV, CountLedHSV, 1);
    KolorowanieTasmHSVDualLed(ColorFillHSV, (103 - CountLedHSV - FirstLedHSV), CountLedHSV, 4);
    break;
  case 31:    //    OOO [] OO%
    KolorowanieTasmHSVDualLed(ColorFillHSV, FirstLedHSV, CountLedHSV, 0);
    KolorowanieTasmHSVDualLed(ColorFillHSV, (119 - CountLedHSV - FirstLedHSV), CountLedHSV, 5);
    break;

  case 32:    //    HOO [] OOH  //Na dwóch paskach po przeciwnej stronie na raz.
    KolorowanieTasmHSVDualLed(ColorFillHSV, FirstLedHSV, CountLedHSV, 0);
    KolorowanieTasmHSVDualLed(ColorFillHSV, FirstLedHSV, CountLedHSV, 5);
    break;
  case 33:    //    OHO [] OHO
    KolorowanieTasmHSVDualLed(ColorFillHSV, FirstLedHSV, CountLedHSV, 1);
    KolorowanieTasmHSVDualLed(ColorFillHSV, FirstLedHSV, CountLedHSV, 4);
    break;
  case 34:    //    OOH [] HOO
    KolorowanieTasmHSVDualLed(ColorFillHSV, FirstLedHSV, CountLedHSV, 2);
    KolorowanieTasmHSVDualLed(ColorFillHSV, FirstLedHSV, CountLedHSV, 3);
    break;
  }
}

void KolorowanieTasmHSVWzorkiStone(uint32_t ColorFillHSV, uint16_t FirstLedHSV, uint16_t CountLedHSV, uint8_t NrLedStrip) {
  switch (NrLedStrip) {
  case 40:    //    %OO [] OO%  //Na dwóch paskach po przeciwnej stronie na raz z odbiciem.
    KolorowanieTasmHSVWzorkiWood(ColorFillHSV, FirstLedHSV, CountLedHSV, 20);
    KolorowanieTasmHSVWzorkiWood(ColorFillHSV, FirstLedHSV, CountLedHSV, 25);
    break;
  case 41:    //    O%O [] O%O
    KolorowanieTasmHSVWzorkiWood(ColorFillHSV, FirstLedHSV, CountLedHSV, 21);
    KolorowanieTasmHSVWzorkiWood(ColorFillHSV, FirstLedHSV, CountLedHSV, 24);
    break;
  case 42:    //    OO% [] %OO
    KolorowanieTasmHSVWzorkiWood(ColorFillHSV, FirstLedHSV, CountLedHSV, 22);
    KolorowanieTasmHSVWzorkiWood(ColorFillHSV, FirstLedHSV, CountLedHSV, 23);
    break;

  case 43:  //Od środka jedna strona
    if (CountLedHSV < 60) {
      KolorowanieTasmHSVWzorkiWood(ColorFillHSV, FirstLedHSV + 60, CountLedHSV, 20);   //    HOO [C] OOO
    }
    if (CountLedHSV < 52) {
      KolorowanieTasmHSVWzorkiWood(ColorFillHSV, FirstLedHSV + 52, CountLedHSV, 21);  //    OHO [C] OOO 
    }
    if (CountLedHSV < 44) {
      KolorowanieTasmHSVWzorkiWood(ColorFillHSV, FirstLedHSV + 44, CountLedHSV, 22);  //    OOH [C] OOO
    }
    break;	//	HHH [C] OOO
  case 44:  //Od środka druga strona
    if (CountLedHSV < 60) {
      KolorowanieTasmHSVWzorkiWood(ColorFillHSV, FirstLedHSV + 60, CountLedHSV, 25);   //    HOO [C] OOO
    }
    if (CountLedHSV < 52) {
      KolorowanieTasmHSVWzorkiWood(ColorFillHSV, FirstLedHSV + 52, CountLedHSV, 24);  //    OHO [C] OOO 
    }
    if (CountLedHSV < 44) {
      KolorowanieTasmHSVWzorkiWood(ColorFillHSV, FirstLedHSV + 44, CountLedHSV, 23);  //    OOH [C] OOO
    }
    break;	//	OOO [C] HHH

  case 45:  //Od boku jedna strona
    if (CountLedHSV < 60) {
      KolorowanieTasmHSVWzorkiWood(ColorFillHSV, FirstLedHSV, CountLedHSV, 20);   //    HOO [C] OOO
    }
    if (CountLedHSV < 52) {
      KolorowanieTasmHSVWzorkiWood(ColorFillHSV, FirstLedHSV, CountLedHSV, 21);  //    OHO [C] OOO 
    }
    if (CountLedHSV < 44) {
      KolorowanieTasmHSVWzorkiWood(ColorFillHSV, FirstLedHSV, CountLedHSV, 22);  //    OOH [C] OOO
    }
    break;	//	HHH [C] OOO
  case 46:  //Od boku druga strona
    if (CountLedHSV < 60) {
      KolorowanieTasmHSVWzorkiWood(ColorFillHSV, FirstLedHSV, CountLedHSV, 25);   //    HOO [C] OOO
    }
    if (CountLedHSV < 52) {
      KolorowanieTasmHSVWzorkiWood(ColorFillHSV, FirstLedHSV, CountLedHSV, 24);  //    OHO [C] OOO 
    }
    if (CountLedHSV < 44) {
      KolorowanieTasmHSVWzorkiWood(ColorFillHSV, FirstLedHSV, CountLedHSV, 23);  //    OOH [C] OOO
    }
    break;	//	OOO [C] HHH
  }
}

void KolorowanieTasmHSVWzorkiSilver(uint32_t ColorFillHSV, uint16_t FirstLedHSV, uint16_t CountLedHSV, uint8_t NrLedStrip) {
  switch (NrLedStrip) {
  case 50:    //    HOO [] OOH  //Na dwóch paskach po przeciwnej stronie na raz od środka
    KolorowanieTasmHSVWzorkiWood(ColorFillHSV, FirstLedHSV, CountLedHSV, 43);
    KolorowanieTasmHSVWzorkiWood(ColorFillHSV, FirstLedHSV, CountLedHSV, 44);
    break;
  case 51:    //    HOO [] OOH  //Na dwóch paskach po przeciwnej stronie na raz.
    KolorowanieTasmHSVWzorkiWood(ColorFillHSV, FirstLedHSV, CountLedHSV, 45);
    KolorowanieTasmHSVWzorkiWood(ColorFillHSV, FirstLedHSV, CountLedHSV, 46);
    break;
  }
}

void KolorowanieTasmHSVWhichWzorek(uint32_t ColorFillHSV, uint16_t FirstLedHSV, uint16_t CountLedHSV, uint8_t NrLedStrip) {
  if (NrLedStrip > 49) {
    KolorowanieTasmHSVWzorkiSilver(ColorFillHSV, FirstLedHSV, CountLedHSV, NrLedStrip);
  }
  else if (NrLedStrip > 39) {
    KolorowanieTasmHSVWzorkiStone(ColorFillHSV, FirstLedHSV, CountLedHSV, NrLedStrip);
  }
  else if (NrLedStrip > 19) {
    KolorowanieTasmHSVWzorkiWood(ColorFillHSV, FirstLedHSV, CountLedHSV, NrLedStrip);
  }
  else if (NrLedStrip > 5) {
    KolorowanieTasmHSV(ColorFillHSV, FirstLedHSV, CountLedHSV, NrLedStrip-5);
  }
  else if (NrLedStrip < 6) {
    KolorowanieTasmHSVDualLed(ColorFillHSV, FirstLedHSV, CountLedHSV, NrLedStrip);
  }
}

//Początek animacji Smooth
uint16_t SmoothColorHSV = 0;    //0-65535 = kolor HSV
uint16_t SmoothJumpHSV = 10;  //Przeskok koloru HSV
uint8_t SmoothBrightHSV = 255;        //jasność HSV

void AnimateSmoothAll() {
  SmoothColorHSV += (SmoothJumpHSV * 10);
  uint32_t ChangeColorSmoothHSV = lsu1.ColorHSV(SmoothColorHSV, 255, SmoothBrightHSV);  //przeskok koloru,nasycenie,jasność
  for (uint8_t i = 0; i < 6; i++) {
    KolorowanieTasmHSVDualLed(ChangeColorSmoothHSV, 0, 0, i);
  }
}


//Początek animacji Rainbow
uint16_t RainbowColorStartAllHSV = 0;    //0-65535 = kolor HSV
uint16_t RainbowJumpAllHSV = 10;  //To wpłynie na prędkość przejścia kolorów
uint16_t RainbowJumpOneHSV = 100;  //Większa wartość to więcej kolorów na taśmach
uint8_t RainbowBrightAllHSV = 255;        //jasność HSV

void AnimateRainbow(uint16_t StartColorRainbowHSV, uint8_t RainbowBrightHSV, uint8_t NrLedStrip) {
  switch (NrLedStrip) {
  case 0:
    for (uint8_t i = 0; i < 120; i++) {
      uint32_t Color32Rainbow = lsu1.ColorHSV(StartColorRainbowHSV + (RainbowJumpOneHSV * i), 255, RainbowBrightHSV);
      KolorowanieTasmHSVDualLed(Color32Rainbow, i, 1, 0);
    }
    break;
  case 1:
    for (uint8_t i = 0; i < 104; i++) {
      uint32_t Color32Rainbow = lsu1.ColorHSV(StartColorRainbowHSV + (RainbowJumpOneHSV * i), 255, RainbowBrightHSV);
      KolorowanieTasmHSVDualLed(Color32Rainbow, i, 1, 1);
    }
    break;
  case 2:
    for (uint8_t i = 0; i < 88; i++) {
      uint32_t Color32Rainbow = lsu1.ColorHSV(StartColorRainbowHSV + (RainbowJumpOneHSV * i), 255, RainbowBrightHSV);
      KolorowanieTasmHSVDualLed(Color32Rainbow, i, 1, 2);
    }
    break;
  case 3:
    for (uint8_t i = 0; i < 88; i++) {
      uint32_t Color32Rainbow = lsu1.ColorHSV(StartColorRainbowHSV + (RainbowJumpOneHSV * i), 255, RainbowBrightHSV);
      KolorowanieTasmHSVDualLed(Color32Rainbow, i, 1, 3);
    }
    break;
  case 4:
    for (uint8_t i = 0; i < 104; i++) {
      uint32_t Color32Rainbow = lsu1.ColorHSV(StartColorRainbowHSV + (RainbowJumpOneHSV * i), 255, RainbowBrightHSV);
      KolorowanieTasmHSVDualLed(Color32Rainbow, i, 1, 4);
    }
    break;
  case 5:
    for (uint8_t i = 0; i < 120; i++) {
      uint32_t Color32Rainbow = lsu1.ColorHSV(StartColorRainbowHSV + (RainbowJumpOneHSV * i), 255, RainbowBrightHSV);
      KolorowanieTasmHSVDualLed(Color32Rainbow, i, 1, 5);
    }
    break;
  }
}

void AnimateRainbowSyncAll() {

  AnimateRainbow(RainbowColorStartAllHSV, RainbowBrightAllHSV, 0);
  AnimateRainbow(RainbowColorStartAllHSV + (8 * RainbowJumpOneHSV), RainbowBrightAllHSV, 1);
  AnimateRainbow(RainbowColorStartAllHSV + (16 * RainbowJumpOneHSV), RainbowBrightAllHSV, 2);
  AnimateRainbow(RainbowColorStartAllHSV + (16 * RainbowJumpOneHSV), RainbowBrightAllHSV, 3);
  AnimateRainbow(RainbowColorStartAllHSV + (8 * RainbowJumpOneHSV), RainbowBrightAllHSV, 4);
  AnimateRainbow(RainbowColorStartAllHSV, RainbowBrightAllHSV, 5);

  RainbowColorStartAllHSV += RainbowJumpAllHSV;
}


//Początek animacji AnimateDisappeLed
uint32_t DisappeNewPixelPreviousTime = 0;  //mills wcześniejszy
uint16_t DisappeNewPixelDelayTime = 500; //Co ile dodawać nowego leda i usuwać starego
uint32_t DisappePreviousTime = 0;  //mills wcześniejszy
uint16_t DisappeDelayTime = 90; //Co ile ściemniać i rozjaśniać
uint16_t DisappeLedColorStatic = 0;

uint16_t DisappeColorArray[36];   //Kolor danego leda
uint8_t DisappeColorBrightArray[36];    //Jasność leda
uint8_t DisappeColorBrightDirection[36];  //Kierunek jasności, ściemniać czy rozjaśniać
uint8_t DisappeColorBrightMaxValue = 239; //Maksymalna jasność leda.
uint8_t DisappeLedStrip[36];      //Który led
uint8_t DisappeLedInStrip[36];    //Który pasek

uint8_t DisappeLedActive = 35;      //Liczba diod w animacji, można zmieniać kodem html
uint8_t DisappeLedActiveCount = 0;  //Licznik o którego leda chodzi w danej chwili
uint8_t DisappeColorRandom = 1;   //Czy kolor ledów losowany, czy ustalony z góry

void AnimateDisappeLedSet(uint8_t DisappeActualLed) {
  uint32_t DisappeColorHSV = lsu1.ColorHSV(DisappeColorArray[DisappeActualLed], 255, DisappeColorBrightArray[DisappeActualLed]);

  KolorowanieTasmHSV(DisappeColorHSV, DisappeLedInStrip[DisappeActualLed], 1, DisappeLedStrip[DisappeActualLed] * 2);
  KolorowanieTasmHSV(DisappeColorHSV, DisappeLedInStrip[DisappeActualLed], 1, DisappeLedStrip[DisappeActualLed]*2+1);
}

void AnimateDisappeLed() {
  if ((millis() - DisappeNewPixelPreviousTime) > DisappeNewPixelDelayTime) {  //wykonuj co 1200ms.
    DisappeNewPixelPreviousTime = millis();
    
    if (DisappeLedActiveCount < DisappeLedActive) {
      DisappeLedActiveCount++;
    }
    else {
      DisappeLedActiveCount = 0;
    }

    DisappeLedStrip[DisappeLedActiveCount] = random(0, 6);  //Losowanie paska led dla diody

    switch (DisappeLedStrip[DisappeLedActiveCount]) {   //Losowanie diody dla paska led
      case 0: case 5:
          DisappeLedInStrip[DisappeLedActiveCount] = random(0, 120);  //Losowanie diody
        break;
      case 1: case 4:
          DisappeLedInStrip[DisappeLedActiveCount] = random(0, 104);  //Losowanie diody
        break;
      case 2: case 3:
          DisappeLedInStrip[DisappeLedActiveCount] = random(0, 88);  //Losowanie diody
        break;
    }
    if (DisappeColorRandom) {
      DisappeColorArray[DisappeLedActiveCount] = random(0, 65534);  //Losowanie koloru diody
    }
    else {
      DisappeColorArray[DisappeLedActiveCount] = DisappeLedColorStatic;
    }
    DisappeColorBrightDirection[DisappeLedActiveCount] = 0;   //Ustawienie kierunku rozjaśniania leda
    DisappeColorBrightArray[DisappeLedActiveCount] = 0; //jasność na 0

  }else

  if ((millis() - DisappePreviousTime) > DisappeDelayTime) {  //wykonuj co 90ms.
    DisappePreviousTime = millis();
    
    for (uint8_t i = 0; i <= DisappeLedActive; i++){
      if (DisappeColorBrightArray[i] > DisappeColorBrightMaxValue) {  //jeśli osiągnięto odpowiednią jasność
        DisappeColorBrightDirection[i] = 1;
      }

      if (DisappeColorBrightDirection[i]==0) { //Dodawanie lub odejmowanie jasności leda
        if (DisappeColorBrightArray[i] < 40) {  //Jeden led będzie świecił 5400ms = (90ms*40+90ms*20); Max świecenie to 36000.
          DisappeColorBrightArray[i] += 1;
        }else {
          DisappeColorBrightArray[i] += 10;
        }
      }
      else if (DisappeColorBrightDirection[i]==1) {
        if (DisappeColorBrightArray[i] < 40) {
          if (DisappeColorBrightArray[i] > 0) {
            DisappeColorBrightArray[i] -= 1;
          }
          
        }else {
          DisappeColorBrightArray[i] -= 10;
        }
      }
      
      AnimateDisappeLedSet(i);//kolorowanie leda

    } //zamknięcie for
  } //zamknięcie ifa co 250ms
}


//Początek animacji Ambilight \/ \/ \/
uint32_t AmbilightTVArrayRGB[5];
uint8_t AmbilightUpdateCountLed = 0;

void AmbilightTV(uint8_t NrLedAmbi, uint8_t ColorR, uint8_t ColorG, uint8_t ColorB) {

  AmbilightTVArrayRGB[NrLedAmbi] = lsu1.Color(ColorR, ColorG, ColorB);

  if (NrLedAmbi == 4 && AmbilightUpdateCountLed > 3) {
    AmbilightUpdateCountLed = 0;

    switch (ModeAnimation) {
      case 20:
        //Długie taśmy
        for (uint8_t i = 0; i < 5; i++) {
          KolorowanieTasmHSVWzorkiWood(AmbilightTVArrayRGB[i], i * 24, 24, 32);
        }   //0-23//24-47//48-71//72-96//96-119
        //Średnie
        KolorowanieTasmHSVWzorkiWood(AmbilightTVArrayRGB[0], 0, 21, 33);   //0-20  =21
        KolorowanieTasmHSVWzorkiWood(AmbilightTVArrayRGB[1], 21, 21, 33);  //21-41
        KolorowanieTasmHSVWzorkiWood(AmbilightTVArrayRGB[2], 42, 20, 33);  //42-61
        KolorowanieTasmHSVWzorkiWood(AmbilightTVArrayRGB[3], 62, 21, 33);  //62-82
        KolorowanieTasmHSVWzorkiWood(AmbilightTVArrayRGB[4], 83, 21, 33);  //83-103
        //Krótkie taśmy
        KolorowanieTasmHSVWzorkiWood(AmbilightTVArrayRGB[0], 0, 17, 34); //0-16 = 17
        KolorowanieTasmHSVWzorkiWood(AmbilightTVArrayRGB[1], 17, 18, 34); //17-34
        KolorowanieTasmHSVWzorkiWood(AmbilightTVArrayRGB[2], 35, 18, 34);  //35-52
        KolorowanieTasmHSVWzorkiWood(AmbilightTVArrayRGB[3], 53, 18, 34);  //53-70
        KolorowanieTasmHSVWzorkiWood(AmbilightTVArrayRGB[4], 71, 17, 34);  //71-87
        break;
      case 21:
        //Długie taśmy
        for (uint8_t i = 0; i < 5; i++) {
          KolorowanieTasmHSVDualLed(AmbilightTVArrayRGB[i], i * 24, 24, 0);
        }   //0-23//24-47//48-71//72-96//96-119
        //Średnie
        KolorowanieTasmHSVDualLed(AmbilightTVArrayRGB[0], 0, 21, 1);   //0-20  =21
        KolorowanieTasmHSVDualLed(AmbilightTVArrayRGB[1], 21, 21, 1);  //21-41
        KolorowanieTasmHSVDualLed(AmbilightTVArrayRGB[2], 42, 20, 1);  //42-61
        KolorowanieTasmHSVDualLed(AmbilightTVArrayRGB[3], 62, 21, 1);  //62-82
        KolorowanieTasmHSVDualLed(AmbilightTVArrayRGB[4], 83, 21, 1);  //83-103
        //Krótkie taśmy
        KolorowanieTasmHSVDualLed(AmbilightTVArrayRGB[0], 0, 17, 2); //0-16 = 17
        KolorowanieTasmHSVDualLed(AmbilightTVArrayRGB[1], 17, 18, 2); //17-34
        KolorowanieTasmHSVDualLed(AmbilightTVArrayRGB[2], 35, 18, 2);  //35-52
        KolorowanieTasmHSVDualLed(AmbilightTVArrayRGB[3], 53, 18, 2);  //53-70
        KolorowanieTasmHSVDualLed(AmbilightTVArrayRGB[4], 71, 17, 2);  //71-87
        break;
    }
  }
    //if ERROR , brak odbieranych danych
  if (AmbilightUpdateCountLed > 15) {
    KolorowanieDwochLedow(200, 0, 0, 0, 20);
  }
  AmbilightUpdateCountLed++;
}


//Music - Animacja, Volume, Color
void Music(uint8_t MusicAnimation, uint8_t MusicVolume, uint8_t MusicColor) {
  
  uint32_t MusicAnimationHSV = lsu1.ColorHSV(MusicColor*255, 255, 255); // Kolor, nasycenie, jasność

  KolorujCalySufit(0,0,0);  //Wyczyszczenie sufitu

  KolorowanieTasmHSVWhichWzorek(MusicAnimationHSV, 0, MusicVolume, MusicAnimation);
}

//==================================================================================\/
//============================KOLOROWANIE=SUFITU=GŁÓWNE=VOIDY=======================\/
//==================================================================================\/

void KolorujCalySufit(uint8_t ColorR, uint8_t ColorG, uint8_t ColorB){  //RGB
  for(uint8_t i=0;i<6;i++){
    KolorujJedenPasek(ColorR, ColorG, ColorB, i);
  }
}

void KolorujJedenPasek(uint8_t ColorR, uint8_t ColorG, uint8_t ColorB, uint8_t NrLedStrip){ //RGB+Nr_Paska
  switch(NrLedStrip){
    case 0: case 5: case 6: case 7: case 16: case 17: case 20:    //if(NrLedStrip==(0||5||6||7||16||17||20)){ ????
      KolorujLedOdAdoZ(ColorR, ColorG, ColorB, NrLedStrip, 0, 119);
    break;
    case 1: case 4: case 8: case 9: case 14: case 15: case 21:
      KolorujLedOdAdoZ(ColorR, ColorG, ColorB, NrLedStrip, 0, 103);
    break;
    case 2: case 3: case 10: case 11: case 12: case 13: case 22:
      KolorujLedOdAdoZ(ColorR, ColorG, ColorB, NrLedStrip, 0, 87);
    break;
  }
}

void KolorujJednegoLeda(uint8_t ColorR, uint8_t ColorG, uint8_t ColorB, uint8_t NrLedStrip, int NrLedInStrip){  //RGB+Nr_Paska+Nr_Led
  if(NrLedStrip<6){
    KolorowanieDwochLedow(ColorR, ColorG, ColorB, NrLedStrip, NrLedInStrip);
	}else if (NrLedStrip > 40) {
    KolorowanieTasmOdSrodkaH(ColorR, ColorG, ColorB, NrLedStrip, NrLedInStrip);
	}else if (NrLedStrip > 34) {
		KolorowanieTasmOdSrodkaO(ColorR, ColorG, ColorB, NrLedStrip, NrLedInStrip);
  }else if (NrLedStrip > 31) {
		KolorowanieWiecejNizDwochDiodH(ColorR, ColorG, ColorB, NrLedStrip, NrLedInStrip);
  }else if (NrLedStrip > 19) {
    KolorowanieWiecejNizDwochDiod(ColorR, ColorG, ColorB, NrLedStrip, NrLedInStrip);
  }else if (NrLedStrip > 5) {
    NrLedStrip-=6;
    KolorowanieTylkoJednegoLeda(ColorR, ColorG, ColorB, NrLedStrip, NrLedInStrip);
  }
}

void KolorujLedOdAdoZ(uint8_t ColorR, uint8_t ColorG, uint8_t ColorB, uint8_t NrLedStrip, int NrLedAStrip, int NrLedBStrip){  //RGB+_NrPaska+LedOd+LedDo
  for(int i=NrLedAStrip;i<=NrLedBStrip;i++){
    KolorujJednegoLeda(ColorR, ColorG, ColorB, NrLedStrip, i);
  }
}

//==================================================================================\/


void HeadVoidAnimation() {
  if ((millis() - PreviousTimeAnimation) > DelayTimeAnimation) {
    PreviousTimeAnimation = millis();

    switch (ModeAnimation) {
    case 1: AnimateSmoothAll();
      break;
    case 2: AnimateRainbowSyncAll();
      break;
    }
  }

  //Animacje z delay w void 
  switch (ModeAnimation) {
  case 3: AnimateDisappeLed();
    break;
  }

}

//==================================================================================\/
//=======================================CAN=READ===================================\/
//==================================================================================\/

void Can_reader() //Odbieranie danych z Cana
{
  CAN_frame_t rx_frame;
  unsigned long cMillisInReadCan = millis();

  if(xQueueReceive(CAN_cfg.rx_queue, &rx_frame, 3 * portTICK_PERIOD_MS) == pdTRUE) {

    if (rx_frame.MsgID == 0x010) {	//Informacja tylko do sufitu RGB www - Animacje
      switch (rx_frame.FIR.B.DLC) {   //Sprawdzanie długości ciągu
      case 2:
        switch (rx_frame.data.u8[0]) {
          case 0: ModeAnimation = rx_frame.data.u8[1];  //Typ animacji  //0 0 - wylaczenie  //0 1 =Animacja 1 //0 2=Animacja 2
            break;
          case 1: DelayTimeAnimation = rx_frame.data.u8[1]; //default 20 // Czas powtórzenia animacji 1, 2, gdzie główny delay jest
            break;
          //animacja 1
          case 10: SmoothJumpHSV = rx_frame.data.u8[1]*5;   //default 10  //skok kolorów
            break;
          case 11: SmoothBrightHSV = rx_frame.data.u8[1]; //default 255 //jasność animacji 1
            break;
          //animacja 2
          case 20: RainbowJumpAllHSV = rx_frame.data.u8[1]*5;   //default 10  //predkosc przesuwania kolorów animacji 2
            break;
          case 21: RainbowJumpOneHSV = rx_frame.data.u8[1]*5;   //default 100 //o ile jednostek więcej ma świecić kolejny led animacji 2
            break;
          case 22: RainbowBrightAllHSV = rx_frame.data.u8[1]; //default 255 //jasność animacji 2
            break;
          //animacja 3
          case 30: DisappeLedActive = rx_frame.data.u8[1];   //default 35 //ilość led w animacji 3
            break;
          case 31: DisappeNewPixelDelayTime = rx_frame.data.u8[1]*100;   //default 5*100=500ms //Czas co ile dodawać nową diodę
            break;
          case 32: DisappeDelayTime = rx_frame.data.u8[1]*10;   //default 9*10=90ms //Czas co ile aktualizować jasność diody
            break;
          case 33: DisappeColorBrightMaxValue = rx_frame.data.u8[1];   //default 249 //Jasność max animacji 3
            break;
          case 34: DisappeColorRandom = rx_frame.data.u8[1];   //def 1 //Czy kolor animacji 3 ma być losowany? Jeśli nie to 0
            break;
          case 35: DisappeLedColorStatic = rx_frame.data.u8[1] * 255; //Ustawianie koloru statycznego hsv
            break;
           //animacja 4
        }
      }
    }
    
    if(rx_frame.MsgID == 0x011) {	//Informacja tylko do sufitu RGB www

      switch(rx_frame.FIR.B.DLC) {   //Sprawdzanie długości ciągu
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
      uint32_t ButtonColor = lsu1.ColorHSV(rx_frame.data.u8[0]*255, 255, rx_frame.data.u8[1]);
      KolorowanieTasmHSV(ButtonColor, 0, 1, 12);
      KolorowanieTasmHSV(ButtonColor, 0, 2, 12);
    }

    //AmbilightTV
    if (rx_frame.MsgID == 0x020) {	
      if (ModeAnimation==20 || ModeAnimation == 21) {
        AmbilightTV(rx_frame.data.u8[0], rx_frame.data.u8[1], rx_frame.data.u8[2], rx_frame.data.u8[3]); //Nr_leda + RGB
      }
    }

    //AmbilightTV om off
    if (rx_frame.MsgID == 0x021 || rx_frame.MsgID == 0x022) { //21 dla samego sufitu, 22 dla sufitu i szafki

      switch(rx_frame.data.u8[0]){
      case 0:
        ModeAnimation = 0;  //wylaczyc ambilight
        break;
      case 1:
        ModeAnimation = 20; //wlaczyc funkcje ambilight
        break;
      case 2:
        ModeAnimation = 21; //zrobic inny styl
        break;
      case 3:
        ModeAnimation = 1; //zrobic inny styl - Smooth
        break;
      case 4:
        ModeAnimation = 2; //zrobic inny styl - Rainbow
        break;
      case 5:
        ModeAnimation = 3; //zrobic inny styl - Disapled
        break;
      }
    }
    
    //Music (25=Sufit / 26=Fan / 27=Karnisz / 28=Bed / 29=Grzejnik / 30=Szafa / 31=Szafka
    if ((rx_frame.MsgID > 0x059) && (rx_frame.MsgID < 0x070)) {
      if (ModeAnimation == 25) {
        Music(rx_frame.data.u8[0], rx_frame.data.u8[1], rx_frame.data.u8[2]); //Animate, Volume, Color 
      }
    }
  }

}

//==================================================================================\/
//========================================VOIDY=====================================\/
//==================================================================================\/


void setup() {
  //CAN
  CAN_cfg.speed = CAN_SPEED_125KBPS; //CAN_SPEED_125KBPS;
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
  lwent.begin();
  // Neopixel Initialize all pixels to 'off'
  KolorujCalySufit(0,0,0);
  lsw.show();
  lwent.show();
}

void loop() {
  Can_reader(); //Odbieranie danych z Cana
  HeadVoidAnimation();  //Animacje z delay, ale jest na końcu if do animacji bez delay
  WhichLedStripUpdate();  //Które paski led aktualizować?
}
