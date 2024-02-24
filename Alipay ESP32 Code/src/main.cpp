#include <Arduino.h>
#include <LEDHandler.h>
#include <LaptopTelemetry.h>
#include "USB.h"
#include <mpu6050.h>
#include <Drive_Motors.h>
#include <Battery_Monitor.h>
#include <melty.h> 

// const char *ssid = "RESNET-BROTECTED";
// const char *pswrd = "marbry2025";

// const char *ssid = "EnVision-Local";
// const char *pswrd = "thinkmakebreak";

const char *ssid = "AlipayDevices";
const char *pswrd = "alipay123";

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
    toggleLeds(CRGB::Red, CRGB::Black, 500);
  } else {
    if (packetBuffer[1] == '1') { // Currently enabled
      alipay.update();
      if (alipay.translate()) {
        l_motor_write(8-5);
        r_motor_write(8+5);
      } else {
        set_both_motors(8);
      }

      switch (packetBuffer[0]) { // Check the drive cmd
      case '1':
        alipay.deg = 0; 
        break;
      case '2':
        alipay.deg = 45; 
        break;
      case '3':
        alipay.deg = 90; 
        break;
      case '4':
        alipay.deg = 135; 
        break;
      case '5':
        alipay.deg = 180; 
        break;
      case '6':
        alipay.deg = 225; 
        break;
      case '7':
        alipay.deg = 270; 
        break;
      case '8':
        alipay.deg = 315; 
        break;
      }

      if (packetBuffer[0] != '0') {// Check drive cmd
        alipay.percentageOfRotation = 0.5;
      } else {
        alipay.percentageOfRotation = 0.00;
      }

    } else {
      toggleLeds(CRGB::Red, CRGB::Green, 500);
      set_both_motors(0); 

      EVERY_N_MILLIS(2000) {
        myLaptop.send(getVoltage());
      }
    } 
  }
  
  
}
