/*
   The design of creating the BLE server is:
   1. Create a BLE Server
   2. Create a BLE Service
   3. Create a BLE Characteristic on the Service
   4. Create a BLE Descriptor on the characteristic
   5. Start the service.
   6. Start advertising.
*/
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

BLEServer* pServer = NULL;
BLECharacteristic* CharakterystykaNr1 = NULL;
BLECharacteristic* StanNaladowniaAku = NULL;
bool deviceConnected = false; //Czy coœ jest po³¹czone?
bool oldDeviceConnected = false;
uint32_t value = 0;

// https://www.uuidgenerator.net/
#define SERVICE_UUID  BLEUUID((uint16_t)0x180F)

class MyServerCallbacks : public BLEServerCallbacks {   //To biega w kó³ko i zmienia stan infa po³¹czenia
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
    BLEDevice::startAdvertising();
  };

  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
  }
};



void setup() {
  Serial.begin(115200);
  BLEDevice::init("Control Ledroom");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks()); //informowanie czy jest po³¹czenie do classa

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);


  StanNaladowniaAku = pService->createCharacteristic(
    BLEUUID((uint16_t)0x2A19),
    BLECharacteristic::PROPERTY_READ |
    BLECharacteristic::PROPERTY_NOTIFY
  );
  // Create a BLE Descriptor

  StanNaladowniaAku->addDescriptor(new BLE2902());




  // Start the service
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  BLEDevice::startAdvertising();
  Serial.println("Waiting a client connection to notify...");
}

uint8_t BatteryValue = 65;

void loop() {
  // notify changed value
  if (deviceConnected) {
    StanNaladowniaAku->setValue(&BatteryValue, 1);
    StanNaladowniaAku->notify();
    value++;
    delay(1500); // bluetooth stack will go into congestion, if too many packets are sent, in 6 hours test i was able to go as low as 3ms
  }
  // disconnecting
  if (!deviceConnected && oldDeviceConnected) {
    delay(500); // give the bluetooth stack the chance to get things ready
    pServer->startAdvertising(); // restart advertising
    Serial.println("start advertising");
    oldDeviceConnected = deviceConnected; //0=0
    Serial.println("Rozlaczono");
  }
  // connecting
  if (deviceConnected && !oldDeviceConnected) {
    // do stuff here on connecting
    oldDeviceConnected = deviceConnected; //1=1
    Serial.println("Polaczono");
  }
}
