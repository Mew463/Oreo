#include <BLE_Uart.h>

BLEServer *pServer = NULL;
BLECharacteristic * pTxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;

char* packetBuffer;
int packSize = 6;

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    }

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValue = pCharacteristic->getValue();

      if (rxValue.length() > 0) {
        for (int i = 0; i < packSize; i++) {
          packetBuffer[i] = rxValue[i];
          // USBSerial.print(rxValue[i]);
        }
        // USBSerial.println();
      }
    }
};

BLE_Uart::BLE_Uart(char* _packetBuffer, int _packSize) {
    packetBuffer = _packetBuffer;
    packSize = _packSize;
}

void BLE_Uart::init_ble() {
  BLEDevice::setDeviceName("ESP32_BLE"); // Name wont change for one of the S3s for some reason. 
  BLEDevice::init("ESP32_BLE");
  
  pServer = BLEDevice::createServer(); // Create the BLE Server
  pServer->setCallbacks(new MyServerCallbacks());
  
  BLEService *pService = pServer->createService(SERVICE_UUID); // Create the BLE Service=
  
  pTxCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_TX,NIMBLE_PROPERTY::NOTIFY); // Create a BLE Characteristic
                                            
  BLECharacteristic * pRxCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_RX,NIMBLE_PROPERTY::WRITE);

  pRxCharacteristic->setCallbacks(new MyCallbacks());
  
  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();
}

void BLE_Uart::send(String message) {
    pTxCharacteristic->setValue(message);
    pTxCharacteristic->notify();
}
