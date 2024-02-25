#include <Arduino.h>
#include <WiFiUdp.h>
#include <LEDHandler.h>
#include "USB.h"
#include <Battery_Monitor.h>
#include <BLE_Uart.h>
#include <esp_now_txrx.h>

const int packSize = 6;
uint8_t mypacketBuffer[packSize];

const int IRLedPin = 4;
const int freq = 38000;
const int ledChannel = 0;
const int resolution = 8;

// Callback when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
    // struct_message myData;
    // memcpy(&myData, incomingData, sizeof(myData));
    USBSerial.print("Bytes received: ");
    USBSerial.println(len);
    USBSerial.print("Message: ");
    // USBSerial.println(myData.a);

    for (int i = 0; i < len; i++) 
        USBSerial.print(incomingData[i]);
    USBSerial.println();
}

// BLE_Uart myBLEUart = BLE_Uart(mypacketBuffer, packSize);

uint8_t mac_addy[] = {0x48, 0x27, 0xE2, 0xD2, 0xC3, 0x94};
ESP_NOW_TXRX myESP32 = ESP_NOW_TXRX(mac_addy,packSize);

void setup(){
  init_led();
  USBSerial.begin(115200);
  delay(1000);
  ledcSetup(ledChannel, freq, resolution);
  ledcAttachPin(IRLedPin, ledChannel);
  
  setLeds(CRGB::Green); 
  myESP32.init(OnDataRecv);
  // myBLEUart.init_ble();
  setLeds(CRGB::Black);
  
}
 
void loop(){

  delay(1000);
  uint8_t mydata[] = {0x03, 0x02};
  myESP32.send(mydata);


  // myBLEUart.send("hello");
  // for (int i = 0; i < 6; i ++) {
  //   USBSerial.print(mypacketBuffer[i]);
  // }
  // USBSerial.println();
  
}