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

// uint8_t mac_addy[] = {0xF4, 0x12, 0xFA, 0x6A, 0xA5, 0xB4};
// ESP_NOW_TXRX IRBeacon = ESP_NOW_TXRX(mac_addy,packSize);
// struct_message myData;

// Callback when data is received
// void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
//     IRBeacon.keepAlive();
//     memcpy(&myData, incomingData, packSize);
// }


void setup()
{
  delay(750);
  init_led();
  init_mpu6050();
  init_motors();

  USBSerial.begin(115200);
  delay(1000);
  setLeds(CRGB::Green);
  laptop.init_ble("Alipay");
  // IRBeacon.init(OnDataRecv);
  setLeds(CRGB::Black); 
}

void loop()
{
  // delay(100);
  // USBSerial.println(digitalRead(10));
  // IRBeacon.keepAlive();
  // myData.a = 
  // USBSerial.println(IRBeacon.isDisconnected());

  if (laptop.isConnected()) {
    if (laptop_packetBuffer[0] == '1') { // Currently enabled 
      alipay.update();
      if (alipay.translate()) {
        l_motor_write(8-5);
        r_motor_write(8+5);
      } else {
        set_both_motors(8);
      }

      switch (laptop_packetBuffer[1]) { // Check the drive cmd
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

      if (laptop_packetBuffer[1] != '0') {// Check drive cmd
        alipay.percentageOfRotation = 0.5;
      } else {
        alipay.percentageOfRotation = 0.00;
      }

    } else { // Currently disabled
      toggleLeds(CRGB::Red, CRGB::Green, 500);
      set_both_motors(0); 

      EVERY_N_MILLIS(2000) {
        // myLaptop.send(getVoltage());
      }
    }
  } else {
    set_both_motors(0);
    toggleLeds(CRGB::Red, CRGB::Black, 500);
  }
}
