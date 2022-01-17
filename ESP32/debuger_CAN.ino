#include <Arduino.h>
#include <ESP32CAN.h>
#include <CAN_config.h>
#include <cstdint>  //Potem usun¹æ, potrzebne tylko do VS, aby podœwietlaæ inty.

//CAN
CAN_device_t CAN_cfg;

void Can_reader() //Odbieranie danych z Cana
{
  CAN_frame_t rx_frame;

  unsigned long currentMillis = millis();

  // Receive next CAN frame from queue
  if (xQueueReceive(CAN_cfg.rx_queue, &rx_frame, 3 * portTICK_PERIOD_MS) == pdTRUE) {

    if (rx_frame.FIR.B.FF == CAN_frame_std) {
      printf("");//A New standard frame
    }
    else {
      printf("");//A New extended frame
    }

    if (rx_frame.FIR.B.RTR == CAN_RTR) {
      printf(" RTR Zjebana ramka z 0x%08X, DLC %d\r\n", rx_frame.MsgID, rx_frame.FIR.B.DLC);
      for (int i = 0; i < rx_frame.FIR.B.DLC; i++) {
        printf(" %d ", rx_frame.data.u8[i]);
      }
    }
    else {
      printf(" from 0x%08X, DLC %d, Data ", rx_frame.MsgID, rx_frame.FIR.B.DLC);
      for (int i = 0; i < rx_frame.FIR.B.DLC; i++) {
        printf(" %d ", rx_frame.data.u8[i]);
      }
      printf("\n");
    }
  }

}

void setup() {
  //CAN
  CAN_cfg.speed = CAN_SPEED_125KBPS; //CAN_SPEED_125KBPS;
  CAN_cfg.tx_pin_id = GPIO_NUM_22;
  CAN_cfg.rx_pin_id = GPIO_NUM_23;
  CAN_cfg.rx_queue = xQueueCreate(10, sizeof(CAN_frame_t));
  ESP32Can.CANInit();
  Serial.begin(115200);
  Serial.println("elo");
}

void loop() {
  Can_reader(); //Odbieranie danych z Cana
}