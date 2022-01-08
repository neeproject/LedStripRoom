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
//===========================WYZWALANIE=WYSYŁANIA=DANYCH=DO=LED=====================\/
//==================================================================================\/

int ListUpdateLedStrip[14] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0}; //Czy aktualizować pasek? //Taśmy led + Włącznik + Obudowa went

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
    case 12:   lsw.show(); break;
    case 13: lwent.show(); break;
  }
}

void WhichLedStripUpdate(){ //Sprawdzanie która taśme led wysłać do aktualizacji
  for(int i=0;i<14;i++){
    if(ListUpdateLedStrip[i]){
      LedStripShowUpdate(i);
    }
  }
}

//==================================================================================\/
//========================KOLOROWANIE=SUFITU=PODSTAWA=KOLOROWANIA===================\/
//==================================================================================\/

void KolorowanieTylkoJednegoLeda(int ColorR, int ColorG, int ColorB, int NrLedStrip, int NrLedInStrip){
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

void KolorowanieDwochLedow(int ColorR, int ColorG, int ColorB, int NrLedStrip, int NrLedInStrip){ //NrLedStrip 0-5
  NrLedStrip*=2;
  KolorowanieTylkoJednegoLeda(ColorR, ColorG, ColorB, NrLedStrip, NrLedInStrip);
  NrLedStrip+=1;
  KolorowanieTylkoJednegoLeda(ColorR, ColorG, ColorB, NrLedStrip, NrLedInStrip);
}

void KolorowanieAnimacjiDwochDiod(int ColorR, int ColorG, int ColorB, int NrLedStrip, int NrLedInStrip){
  switch(NrLedStrip){
    case 20:   //Normalnie i od końca
    KolorowanieDwochLedow(ColorR, ColorG, ColorB, 0, NrLedInStrip);
    NrLedInStrip=119-NrLedInStrip;
    KolorowanieDwochLedow(ColorR, ColorG, ColorB, 0, NrLedInStrip);   //    HOO [] OOO
    break;
    case 21:   
    KolorowanieDwochLedow(ColorR, ColorG, ColorB, 1, NrLedInStrip);
    NrLedInStrip=103-NrLedInStrip;
    KolorowanieDwochLedow(ColorR, ColorG, ColorB, 1, NrLedInStrip);   //    OHO [] OOO
    break;
    case 22:   
    KolorowanieDwochLedow(ColorR, ColorG, ColorB, 2, NrLedInStrip);
    NrLedInStrip=87-NrLedInStrip;
    KolorowanieDwochLedow(ColorR, ColorG, ColorB, 2, NrLedInStrip);   //    OOH [] OOO
    break;
    case 23:
    KolorowanieDwochLedow(ColorR, ColorG, ColorB, 3, NrLedInStrip);
    NrLedInStrip=87-NrLedInStrip;
    KolorowanieDwochLedow(ColorR, ColorG, ColorB, 3, NrLedInStrip);   //    OOO [] HOO
    break;
    case 24:
    KolorowanieDwochLedow(ColorR, ColorG, ColorB, 4, NrLedInStrip);
    NrLedInStrip=103-NrLedInStrip;
    KolorowanieDwochLedow(ColorR, ColorG, ColorB, 4, NrLedInStrip);   //    OOO [] OHO
    break;
    case 25:
    KolorowanieDwochLedow(ColorR, ColorG, ColorB, 5, NrLedInStrip);
    NrLedInStrip=119-NrLedInStrip;
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
    NrLedInStrip=119-NrLedInStrip;
    KolorowanieDwochLedow(ColorR, ColorG, ColorB, 5, NrLedInStrip);   //    XOO [] OO%
    break;
    case 30:   
    KolorowanieDwochLedow(ColorR, ColorG, ColorB, 1, NrLedInStrip);
    NrLedInStrip=103-NrLedInStrip;
    KolorowanieDwochLedow(ColorR, ColorG, ColorB, 4, NrLedInStrip);   //    OXO [] O%O
    break;
    case 31:   
    KolorowanieDwochLedow(ColorR, ColorG, ColorB, 2, NrLedInStrip);
    NrLedInStrip=87-NrLedInStrip;
    KolorowanieDwochLedow(ColorR, ColorG, ColorB, 3, NrLedInStrip);   //    OOX [] %OO
    break;

  }
}

void KolorowanieAnimacjiDwochDiodH(int ColorR, int ColorG, int ColorB, int NrLedStrip, int NrLedInStrip){
  switch(NrLedStrip){
    case 32:   //Zapalanie dwóch pasków po przeciwnych stronach z dwóch stron
    KolorowanieAnimacjiDwochDiod(ColorR, ColorG, ColorB, 20, NrLedInStrip);
    KolorowanieAnimacjiDwochDiod(ColorR, ColorG, ColorB, 25, NrLedInStrip);      //    HOO [] OOH
    break;
    case 33:   
    KolorowanieAnimacjiDwochDiod(ColorR, ColorG, ColorB, 21, NrLedInStrip);
    KolorowanieAnimacjiDwochDiod(ColorR, ColorG, ColorB, 24, NrLedInStrip);      //    OHO [] OHO
    break;
    case 34:   
    KolorowanieAnimacjiDwochDiod(ColorR, ColorG, ColorB, 22, NrLedInStrip);
    KolorowanieAnimacjiDwochDiod(ColorR, ColorG, ColorB, 23, NrLedInStrip);      //    OOH [] HOO
    break;
  }
}

//Kolorowanie taśm led od środka zakres ledów  NrLedInStrip od 0-59, 0-51, 0-43
void KolorowanieTasmOdSrodkaO(int ColorR, int ColorG, int ColorB, int NrLedStrip, int NrLedInStrip) {
	switch (NrLedStrip) {
		case 35:
			if (NrLedInStrip < 60) {
				int StripLong = NrLedInStrip + 60;
				KolorowanieAnimacjiDwochDiod(ColorR, ColorG, ColorB, 20, StripLong);   //    HOO [C] OOO  //59
			}
			if (NrLedInStrip < 52) {
				int StripMedium = NrLedInStrip + 52;
				KolorowanieAnimacjiDwochDiod(ColorR, ColorG, ColorB, 21, StripMedium); //    OHO [C] OOO  //51
			}
			if (NrLedInStrip < 52) {
				int StripShort = NrLedInStrip + 44;
				KolorowanieAnimacjiDwochDiod(ColorR, ColorG, ColorB, 22, StripShort);  //    OOH [C] OOO  //43
			}
		break;	//	HHH [C] OOO
		case 36:
			if (NrLedInStrip < 60) {
				int StripLong = NrLedInStrip + 60;
				KolorowanieAnimacjiDwochDiod(ColorR, ColorG, ColorB, 25, StripLong);   //    OOO [C] OOH  //59
			}
			if (NrLedInStrip < 52) {
				int StripMedium = NrLedInStrip + 52;
				KolorowanieAnimacjiDwochDiod(ColorR, ColorG, ColorB, 24, StripMedium); //    OOO [C] OHO  //51
			}
			if (NrLedInStrip < 52) {
				int StripShort = NrLedInStrip + 44;
				KolorowanieAnimacjiDwochDiod(ColorR, ColorG, ColorB, 23, StripShort);  //    OOO [C] HOO  //43
			}
			break;	//	OOO [C] HHH
}

void KolorowanieTasmOdSrodkaH(int ColorR, int ColorG, int ColorB, int NrLedStrip, int NrLedInStrip) {
	switch (NrLedStrip) {
		case 37:
			KolorowanieTasmOdSrodkaO(ColorR, ColorG, ColorB, 35, StripLong);
		 KolorowanieTasmOdSrodkaO(ColorR, ColorG, ColorB, 36, StripLong);		//	HHH	[C]	HHH
		break;
}


//==================================================================================\/
//===============================KOLOROWANIE=SUFITU=ANIMACJE========================\/
//==================================================================================\/

void AnimacjaPodstawowa(int NrAnimacji) {

}


//==================================================================================\/
//============================KOLOROWANIE=SUFITU=GŁÓWNE=VOIDY=======================\/
//==================================================================================\/

void KolorujCalySufit(int ColorR, int ColorG, int ColorB){  //RGB
  for(int i=0;i<5;i++){
    KolorujJedenPasek(ColorR, ColorG, ColorB, i);
  }
}

void KolorujJedenPasek(int ColorR, int ColorG, int ColorB, int NrLedStrip){ //RGB+Nr_Paska
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

void KolorujJednegoLeda(int ColorR, int ColorG, int ColorB, int NrLedStrip, int NrLedInStrip){  //RGB+Nr_Paska+Nr_Led
  if(NrLedStrip<6){
    KolorowanieDwochLedow(ColorR, ColorG, ColorB, NrLedStrip, NrLedInStrip);
		}else if (NrLedStrip > 36) {
			KolorowanieTasmOdSrodkaO(ColorR, ColorG, ColorB, NrLedStrip, NrLedInStrip);
		}else if (NrLedStrip > 34) {
			KolorowanieTasmOdSrodkaO(ColorR, ColorG, ColorB, NrLedStrip, NrLedInStrip);
  }else if (NrLedStrip > 31) {
			KolorowanieAnimacjiDwochDiodH(ColorR, ColorG, ColorB, NrLedStrip, NrLedInStrip);
  }else if (NrLedStrip > 19) {
    KolorowanieAnimacjiDwochDiod(ColorR, ColorG, ColorB, NrLedStrip, NrLedInStrip);
  }else if (NrLedStrip > 5) {
    NrLedStrip-=6;
    KolorowanieDwochLedow(ColorR, ColorG, ColorB, NrLedStrip, NrLedInStrip);
  }
}

void KolorujLedOdAdoZ(int ColorR, int ColorG, int ColorB, int NrLedStrip, int NrLedAStrip, int NrLedBStrip){  //RGB+_NrPaska+LedOd+LedDo
  for(int i=NrLedAStrip;i<NrLedBStrip;i++){
    KolorujJednegoLeda(ColorR, ColorG, ColorB, NrLedStrip, i);
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

    if(rx_frame.MsgID == 0x011) {	//Informacja tylko do sufitu

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
