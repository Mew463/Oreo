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
  setLeds(CRGB::Red);
  delay(250);
  // ledcWrite(ledChannel, 80);
  // delay(100);
}