#include <Arduino.h>
#include <WiFiUdp.h>
#include <LEDHandler.h>
#include "USB.h"
#include <Battery_Monitor.h>
#include <BLE_Uart.h>
#include <esp_now_txrx.h>

const int packSize = 6;
uint8_t laptop_packetBuffer[packSize];

uint8_t esp_now_packetBuffer[packSize];

const int IRLedPin = 4;
const int freq = 38000;
const int ledChannel = 0;
const int resolution = 8;

// Callback when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
    memcpy(esp_now_packetBuffer, incomingData, packSize);
    USBSerial.print("Bytes received: ");
    USBSerial.println(len);
    USBSerial.print("Message: ");

    for (int i = 0; i < len; i++) 
        USBSerial.print(incomingData[i]);
    USBSerial.println();
}

BLE_Uart myBLEUart = BLE_Uart(laptop_packetBuffer, packSize);

uint8_t mac_addy[] = {0x48, 0x27, 0xE2, 0xD2, 0xC3, 0x94};
ESP_NOW_TXRX myESP32 = ESP_NOW_TXRX(mac_addy,packSize);

void setup(){
  init_led();
  USBSerial.begin(115200);
  delay(1000);
  ledcSetup(ledChannel, freq, resolution);
  ledcAttachPin(IRLedPin, ledChannel);
  
  setLeds(CRGB::Green); 
  // myESP32.init(OnDataRecv);
  myBLEUart.init_ble();
  setLeds(CRGB::Black);
  
}
 
void loop(){

  delay(1000);
  uint8_t mydata[] = {0x03, 0x02, 0x00, 0x00, 0x00, 0x00};
  // myESP32.send(mydata);


  myBLEUart.send(mydata);
  for (int i = 0; i < 6; i ++) {
    USBSerial.print(laptop_packetBuffer[i]);
  }
  USBSerial.println();
  
}