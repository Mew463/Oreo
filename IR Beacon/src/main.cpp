#include <Arduino.h>
#include <WiFiUdp.h>
#include <LEDHandler.h>
#include "USB.h"
#include <Battery_Monitor.h>
#include <LaptopTelemetry.h>

const char *ssid = "RESNET-BROTECTED";
const char *pswrd = "marbry2025";

// const char *ssid = "EnVision-Local";
// const char *pswrd = "thinkmakebreak";

// const char *ssid = "AlipayDevices";
// const char *pswrd = "alipay123";

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
  myLaptop.receive();

  if (myLaptop.isDisconnected()) {
    toggleLeds(CRGB::Red, CRGB::Black, 500);
  } else {
    if (packetBuffer[0] == '1') { // Enabled
      toggleLeds(CRGB::Blue, CRGB::Purple, 500);
      ledcWrite(ledChannel, 100);
    } 
    if (packetBuffer[0] == '0') {    // Disabled
      toggleLeds(CRGB::Red, CRGB::Green, 500);
      ledcWrite(ledChannel, 0);
    }
  }

}