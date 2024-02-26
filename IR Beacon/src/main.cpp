#include <Arduino.h>
#include <WiFiUdp.h>
#include <LEDHandler.h>
#include "USB.h"
#include <Battery_Monitor.h>
#include <BLE_Uart.h>
#include <esp_now_txrx.h>

const int packSize = 6;
char laptop_packetBuffer[packSize] = {};

const int IRLedPin = 4;
const int freq = 38000;
const int ledChannel = 0;
const int resolution = 8;


BLE_Uart laptop = BLE_Uart(laptop_packetBuffer, packSize);

void setup(){
  init_led();
  setLeds(CRGB::Green); 
  USBSerial.begin(115200);
  ledcSetup(ledChannel, freq, resolution);
  ledcAttachPin(IRLedPin, ledChannel);

  laptop.init_ble("IR Beacon");
  setLeds(CRGB::Black);
}
 
void loop(){
  
  if (laptop.isConnected()) {
    if (laptop_packetBuffer[0] == '0') {// Robot is disabled
      toggleLeds(CRGB::Red, CRGB::Green, 500);
      ledcWrite(ledChannel, 0);
    } else {
      toggleLeds(CRGB::Blue, CRGB::Purple, 500);
      ledcWrite(ledChannel, 100);
    }

    EVERY_N_SECONDS(10) {
      laptop.send(get1sVoltage());
    }

  } else {
    toggleLeds(CRGB::Red, CRGB::Black, 500);
    ledcWrite(ledChannel, 0);
  }
}