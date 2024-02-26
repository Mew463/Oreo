#include <Arduino.h>
#include <LEDHandler.h>
#include "USB.h"
#include <mpu6050.h>
#include <Drive_Motors.h>
#include <Battery_Monitor.h>
#include <melty.h> 
#include <BLE_Uart.h>

const int packSize = 6;
char laptop_packetBuffer[packSize] = {};

BLE_Uart laptop = BLE_Uart(laptop_packetBuffer, packSize);

melty alipay = melty();
struct melty_parameters {
  int rot = 15;
  int tra = 5;
  float per = 0.5;
} melty_parameters;

void setup()
{
  init_led();
  setLeds(CRGB::Green);
  USBSerial.begin(115200);
  init_mpu6050();
  init_motors();
  laptop.init_ble("Alipay");
  setLeds(CRGB::Black); 
}

void loop()
{
  if (laptop.isConnected()) {
    if (laptop_packetBuffer[0] == '1') { // Currently enabled and translating!!
      alipay.update();
      if (alipay.translate()) {
        l_motor_write(melty_parameters.rot-melty_parameters.tra);
        r_motor_write(melty_parameters.rot+melty_parameters.tra);
      } else {
      set_both_motors(melty_parameters.rot);
      }

      switch (laptop_packetBuffer[1]) { // Check the drive cmd
      case '8':
        alipay.deg = 0; 
        break;
      case '7':
        alipay.deg = 45; 
        break;
      case '6':
        alipay.deg = 90; 
        break;
      case '5':
        alipay.deg = 135; 
        break;
      case '4':
        alipay.deg = 180; 
        break;
      case '3':
        alipay.deg = 225; 
        break;
      case '2':
        alipay.deg = 270; 
        break;
      case '1':
        alipay.deg = 315; 
        break;
      }

      if (laptop_packetBuffer[1] != '0') {// Check drive cmd
        alipay.percentageOfRotation = melty_parameters.per;
      } else {
        alipay.percentageOfRotation = 0;
      }

      EVERY_N_MILLIS(50) { // Print out the settings
        String msg = "rotpwr : " + String(melty_parameters.rot) + " tranpwr : " + String(melty_parameters.tra) + " perc : " + String(melty_parameters.per);
        laptop.send(msg);
      }

      EVERY_N_MILLIS(100) {
        switch (laptop_packetBuffer[2]) {
          case '1':
            melty_parameters.rot++;
            break;
          case '2':
            melty_parameters.rot--;
            break;
          case '3':
            melty_parameters.tra++;
            break;
          case '4':
            melty_parameters.tra--;
            break;
          case '5':
            melty_parameters.per = melty_parameters.per + .03;
            break;
          case '6':
            melty_parameters.per = melty_parameters.per - .03;
            break;
        }
      }

    } else { // Currently disabled
      toggleLeds(CRGB::Red, CRGB::Green, 500);
      set_both_motors(0); 
        EVERY_N_SECONDS(10) {
        laptop.send(get3sVoltage());
        }

    }
  } else {
    set_both_motors(0);
    toggleLeds(CRGB::Red, CRGB::Black, 500);
  }
}
