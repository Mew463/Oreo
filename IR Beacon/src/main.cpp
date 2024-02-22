#include <Arduino.h>
#include <WiFiUdp.h>
#include <LEDHandler.h>
#include "USB.h"
#include <Battery_Monitor.h>
#include <LaptopTelemetry.h>

const char *ssid = "RESNET-BROTECTED";
const char *pswrd = "marbry2025";

const int packSize = 3;
char packetBuffer[packSize];

LaptopTelemetry myLaptop = LaptopTelemetry(ssid, pswrd, packetBuffer);

const int IRLedPin = 4;
const int freq = 38000;
const int ledChannel = 0;
const int resolution = 8;

char lastEnabledCmd = '0';
 
void setup(){
  init_led();
  USBSerial.begin(115200);
  ledcSetup(ledChannel, freq, resolution);
  ledcAttachPin(IRLedPin, ledChannel);
  
  delay(500);
  setLeds(CRGB::Green);
  myLaptop.init();
  setLeds(CRGB::Black);
  
}
 
void loop(){
  myLaptop.receive();

  if (myLaptop.isDisconnected()) {
    setLeds(CRGB::Orange);
  } else {
    if (packetBuffer[0] == '1' && lastEnabledCmd == '0') { // Enabled
      toggleLeds(CRGB::Blue, CRGB::Purple, 500);
      ledcWrite(ledChannel, 100);
    } 
    if (packetBuffer[0] == '0') {    // Disabled
      toggleLeds(CRGB::Red, CRGB::Green, 500);
      ledcWrite(ledChannel, 0);
    }

    if (packetBuffer[2] == '1') // Sync led toggling
      syncToggle();

  }

  lastEnabledCmd = packetBuffer[0];
}