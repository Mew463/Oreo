#include <Arduino.h>
#include <WiFiUdp.h>
#include <LEDHandler.h>
#include "USB.h"
#include <Battery_Monitor.h>
#include <BLE_Uart.h>
#include <esp_now_txrx.h>

const int packSize = 6;
char laptop_packetBuffer[packSize] = {};

char esp_now_packetBuffer[packSize] = {};

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
ESP_NOW_TXRX Alipay = ESP_NOW_TXRX(mac_addy,packSize);
struct_message myData;

void setup(){
  init_led();
  USBSerial.begin(115200);
  // delay(1000);
  ledcSetup(ledChannel, freq, resolution);
  ledcAttachPin(IRLedPin, ledChannel);
  
  setLeds(CRGB::Green); 
  Alipay.init(OnDataRecv);
  myBLEUart.init_ble();
  setLeds(CRGB::Black);
  
}

uint8_t charToByte(char *letter) {
  return byte(atoi(letter));
}
 
void loop(){
  if (myBLEUart.isConnected()) {
    strcpy(myData.a, laptop_packetBuffer);
    Alipay.send(myData);

    if (laptop_packetBuffer[0] == '0') {// Robot is disabled
      toggleLeds(CRGB::Red, CRGB::Green, 500);
      ledcWrite(ledChannel, 0);
    } else {
      toggleLeds(CRGB::Blue, CRGB::Purple, 500);
      ledcWrite(ledChannel, 100);
    }

  } else {
    toggleLeds(CRGB::Red, CRGB::Black, 500);
    ledcWrite(ledChannel, 0);
  }
  // delay(100);
  // strcpy(myData.a, "123456");
  // // myData.a = "123456";
  // // char* ptr = laptop_packetBuffer[0];
  // // USBSerial.println(byte('0'));
  // // mydata[0] = 
  // Alipay.send(myData);
  
}