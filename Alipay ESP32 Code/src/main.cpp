#include <Arduino.h>
#include <LEDHandler.h>
#include <LaptopTelemetry.h>
#include "USB.h"
#include <mpu6050.h>
#include <Drive_Motors.h>
#include <Battery_Monitor.h>
#include <melty.h> 

const char *ssid = "RESNET-BROTECTED";
const char *pswrd = "marbry2025";

const int packSize = 3;
char packetBuffer[packSize];

LaptopTelemetry myLaptop = LaptopTelemetry(ssid, pswrd, packetBuffer);
melty alipay = melty();



void setup()
{
  delay(750); // Some reason my aliexpress esp32s3 needs this delay to give it enough time to initialize
  init_led();
  init_mpu6050();
  init_motors();

  USBSerial.begin(115200);
  setLeds(CRGB::Green);
  myLaptop.init();
  setLeds(CRGB::Black);
}

void loop()
{
  myLaptop.receive();
  
  if (myLaptop.isDisconnected()) {
    set_both_motors(0);
    setLeds(CRGB::Orange);
  } else {
    if (packetBuffer[1] == '1') {
      alipay.updateRPM();

      EVERY_N_MILLIS(100) {
        myLaptop.send(alipay.RPM);
      }
      
      if (packetBuffer[0] == '1') {
        set_both_motors(5);
      }
      else if (packetBuffer[0] == '2')
        set_both_motors(7);
      else if (packetBuffer[0] == '3')
        set_both_motors(9);
      else if (packetBuffer[0] == '4')
        set_both_motors(12);
      // else
      //   setLeds(CRGB::Black);
    } else {
      toggleLeds(CRGB::Red, CRGB::Green, 500);
      set_both_motors(0); 

      EVERY_N_MILLIS(2000) {
        myLaptop.send(getVoltage());
      }
    } 
  }
  
  
}
