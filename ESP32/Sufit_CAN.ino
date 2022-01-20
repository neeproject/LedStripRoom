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

void KolorowanieTasmHSVWzorkiWood(uint32_t ColorFillHSV, uint16_t FirstLedHSV, uint16_t CountLedHSV, uint8_t NrLedStrip) {
  switch (NrLedStrip) {
  case 6:    //    %OO [] OOO
    KolorowanieTasmHSVDualLed(ColorFillHSV, FirstLedHSV, CountLedHSV, 0);
    KolorowanieTasmHSVDualLed(ColorFillHSV, (119 - CountLedHSV - FirstLedHSV), CountLedHSV, 0);
    break;
  case 7:    //    O%O [] OOO
    KolorowanieTasmHSVDualLed(ColorFillHSV, FirstLedHSV, CountLedHSV, 1);
    KolorowanieTasmHSVDualLed(ColorFillHSV, (103 - CountLedHSV - FirstLedHSV), CountLedHSV, 1);
    break;
  case 8:    //    OO% [] OOO
    KolorowanieTasmHSVDualLed(ColorFillHSV, FirstLedHSV, CountLedHSV, 2);
    KolorowanieTasmHSVDualLed(ColorFillHSV, (87 - CountLedHSV - FirstLedHSV), CountLedHSV, 2);
    break;
  case 9:    //    OOO [] %OO
    KolorowanieTasmHSVDualLed(ColorFillHSV, FirstLedHSV, CountLedHSV, 3);
    KolorowanieTasmHSVDualLed(ColorFillHSV, (87 - CountLedHSV - FirstLedHSV), CountLedHSV, 3);
    break;
  case 10:    //    OOO [] O%O
    KolorowanieTasmHSVDualLed(ColorFillHSV, FirstLedHSV, CountLedHSV, 4);
    KolorowanieTasmHSVDualLed(ColorFillHSV, (103 - CountLedHSV - FirstLedHSV), CountLedHSV, 4);
    break;
  case 11:    //    OOO [] OO%
    KolorowanieTasmHSVDualLed(ColorFillHSV, FirstLedHSV, CountLedHSV, 5);
    KolorowanieTasmHSVDualLed(ColorFillHSV, (119 - CountLedHSV - FirstLedHSV), CountLedHSV, 5);
    break;


  case 12:    //    HOO [] OOH
    KolorowanieTasmHSVDualLed(ColorFillHSV, FirstLedHSV, CountLedHSV, 0);
    KolorowanieTasmHSVDualLed(ColorFillHSV, FirstLedHSV, CountLedHSV, 5);
    break;
  case 13:    //    OHO [] OHO
    KolorowanieTasmHSVDualLed(ColorFillHSV, FirstLedHSV, CountLedHSV, 1);
    KolorowanieTasmHSVDualLed(ColorFillHSV, FirstLedHSV, CountLedHSV, 4);
    break;
  case 14:    //    OOH [] HOO
    KolorowanieTasmHSVDualLed(ColorFillHSV, FirstLedHSV, CountLedHSV, 2);
    KolorowanieTasmHSVDualLed(ColorFillHSV, FirstLedHSV, CountLedHSV, 3);
    break;
  }
}


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


//AnimateDisappeLed
uint32_t DisappeNewPixelPreviousTime = 0;  //mills wcześniejszy
uint16_t DisappeNewPixelDelayTime = 1000; //Co ile dodawać nowego leda i usuwać starego
uint32_t DisappePreviousTime = 0;  //mills wcześniejszy
uint16_t DisappeDelayTime = 250; //Co ile ściemniać i rozjaśniać

uint16_t DisappeColorArray[36];   //Kolor danego leda
uint8_t DisappeColorBrightArray[36];    //Jasność leda
uint8_t DisappeColorBrightDirection[36];  //Kierunek jasności, ściemniać czy rozjaśniać
uint8_t DisappeColorBrightMaxValue = 249; //Maksymalna jasność leda.
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
  if ((millis() - DisappeNewPixelPreviousTime) > DisappeNewPixelDelayTime) {  //wykonuj co 1000ms.
    DisappeNewPixelPreviousTime = millis();
    
    DisappeLedStrip[DisappeLedActiveCount] = random(0, 5);  //Losowanie paska led dla diody

    switch (DisappeLedStrip[DisappeLedActiveCount]) {   //Losowanie diody dla paska led
      case 0: case 5:
          DisappeLedInStrip[DisappeLedActiveCount] = random(0, 119);  //Losowanie diody
        break;
      case 1: case 4:
          DisappeLedInStrip[DisappeLedActiveCount] = random(0, 103);  //Losowanie diody
        break;
      case 2: case 3:
          DisappeLedInStrip[DisappeLedActiveCount] = random(0, 87);  //Losowanie diody
        break;
    }
    if (DisappeColorRandom) {
      DisappeColorArray[DisappeLedActiveCount] = random(0, 65534);  //Losowanie koloru diody
    }
    DisappeColorBrightDirection[DisappeLedActiveCount] = 1;   //Ustawienie kierunku rozjaśniania leda

    if (DisappeLedActiveCount < DisappeLedActive) {
      DisappeLedActiveCount++;
    }else {
      DisappeLedActiveCount = 0;
    }
  }

  if ((millis() - DisappePreviousTime) > DisappeDelayTime) {  //wykonuj co 250ms.
    DisappePreviousTime = millis();
    
    for (uint8_t i = 0; i < DisappeLedActive; i++){

      if (DisappeColorBrightDirection[i]) { //Dodawanie lub odejmowanie jasności leda
        if (DisappeColorBrightArray[i] < 50) {  //Jeden led będzie świecił 17500ms = (250ms*50+250ms*20); Max świecenie to 36000.
          DisappeColorBrightArray[i] += 1;
        }else {
          DisappeColorBrightArray[i] += 10;
        }
      }
      else {
        if (DisappeColorBrightArray[i] < 50) {
          DisappeColorBrightArray[i] -= 1;
        }else {
          DisappeColorBrightArray[i] -= 10;
        }
      }
      
      AnimateDisappeLedSet(i);//kolorowanie leda

      if (DisappeColorBrightArray[i] >= DisappeColorBrightMaxValue) {  //jeśli osiągnięto odpowiednią jasność
        DisappeColorBrightDirection[DisappeLedActiveCount] = 0;
      }
    } //zamknięcie for
  } //zamknięcie ifa co 250ms

}


//Ambilight \/ \/ \/

uint32_t AmbilightTVArrayRGB[98];
uint8_t AmbilightUpdateCountLed = 0;

void AmbilightTV(uint8_t NrLedAmbi, uint8_t ColorR, uint8_t ColorG, uint8_t ColorB) {

  AmbilightTVArrayRGB[NrLedAmbi] = lsu1.Color(ColorR, ColorG, ColorB);
  AmbilightUpdateCountLed++;

  if (NrLedAmbi == 97 && AmbilightUpdateCountLed > 96){
      AmbilightUpdateCountLed = 0;

      for (uint8_t i = 0; i < 98; i++) {
        uint8_t ii = 97 - i;
        lsu1.setPixelColor(10 + ii, AmbilightTVArrayRGB[i]);
        lsu2.setPixelColor(10 + ii, AmbilightTVArrayRGB[i]);
      }
      
      lsu1.show();
      lsu2.show();
  }else
    //if ERROR , brak odbieranych danych
  if (AmbilightUpdateCountLed > 250) {
    AmbilightUpdateCountLed = 0;
    lsu1.setPixelColor(0,255,0,0);
    lsu1.show();
  }
}

//Ambilight /\ /\ /\

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

uint8_t ModeAnimation = 0;
uint32_t PreviousTimeAnimation = 0;
uint16_t DelayTimeAnimation = 20;

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

//Music
void Music(uint8_t MusicAnimation, uint8_t MusicVolume, uint8_t MusicColor) {
  switch (MusicAnimation) {
    //Kolorowanie od środków  \/
  case 0:
    KolorujLedOdAdoZ(MusicColorR, MusicColorG, MusicColorB, 40, 0, MusicVolume); // HHH [] HHH  //od środka do brzegów z dwóch stron
    break;
  case 1:
    KolorujLedOdAdoZ(MusicColorR, MusicColorG, MusicColorB, 35, 0, MusicVolume); // HHH [] 000  //od środka do brzegów
    break;  
  case 2:
    KolorujLedOdAdoZ(MusicColorR, MusicColorG, MusicColorB, 36, 0, MusicVolume); // 000 [] HHH  //od środka do brzegów
    break;
    //Kolorowanie od brzegów do środka \/
  case 3:
    KolorujLedOdAdoZ(MusicColorR, MusicColorG, MusicColorB, 41, 0, MusicVolume); // HHH [] HHH  //od brzegów do środka z dwóch stron
    break;
  case 4:
    KolorujLedOdAdoZ(MusicColorR, MusicColorG, MusicColorB, 37, 0, MusicVolume); // HHH [] 000  //od brzegów do środka
    break;
  case 5:
    KolorujLedOdAdoZ(MusicColorR, MusicColorG, MusicColorB, 38, 0, MusicVolume); // 000 [] HHH  //od brzegów do środka
    break;
  case 6:

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
          case 31: DisappeNewPixelDelayTime = rx_frame.data.u8[1]*100;   //default 10*100=1000ms //Czas co ile dodawać nową diodę
            break;
          case 32: DisappeDelayTime = rx_frame.data.u8[1]*10;   //default 25*10=250ms //Czas co ile aktualizować jasność diody
            break;
          case 33: DisappeColorBrightMaxValue = rx_frame.data.u8[1];   //default 249 //Jasność max animacji 3
            break;
          case 34: DisappeColorRandom = rx_frame.data.u8[1];   //def 1 //Czy kolor animacji 3 ma być losowany? Jeśli nie to kurwa co?
            break;
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

    }
    if (rx_frame.MsgID == 0x020) {	//AmbilightTV
      if (ModeAnimation==20) {
        AmbilightTV(rx_frame.data.u8[0], rx_frame.data.u8[1], rx_frame.data.u8[2], rx_frame.data.u8[3]); //Nr_leda + RGB
      }
    }

    //Music (25=Sufit / 26=Fan / 27=Karnisz / 28=Bed / 29=Grzejnik / 30=Szafa / 31=Szafka
    if (rx_frame.MsgID == 0x025) {	
      if (ModeAnimation == 25) {
        //Music(rx_frame.data.u8[0], rx_frame.data.u8[1], rx_frame.data.u8[2]); //Animate, Volume, Color 
      }
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
