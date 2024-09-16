#include <Arduino.h>
#include <LEDHandler.h>
#include <Battery_Monitor.h>
#include <BLE_Uart.h>
#include <FastLED.h>

#define TARGETCMD '1' // Change based on which IR_Beacon working on 

const int packSize = 6;
char laptop_packetBuffer[packSize] = {'0', '0', '0', '0', '0', '0'};

const int IRLedPin = 4;
const int freq = 38000;
const int ledChannel = 1;
const int resolution = 8;

int dutycycle = 30;
const int maxdutycycle = 100;

BLE_Uart laptop = BLE_Uart(laptop_packetBuffer, packSize);

void setup(){
  init_led();
  setLeds(ORANGE); 
  USBSerial.begin(115200);
  ledcSetup(ledChannel, freq, resolution);
  ledcAttachPin(IRLedPin, ledChannel);
  laptop.init_ble("IR Beacon");
  setLeds(BLACK);
}
 
void loop(){
  if (laptop.isConnected()) {
    switch (laptop_packetBuffer[0]) {
      case '0': // Disabled 
        toggleLeds(RED, GREEN, 500);
        ledcWrite(ledChannel, 0);
        EVERY_N_SECONDS(10) {
          laptop.send("SOC: " + String(get1sSOC()) + " %");
        }
        break;
      case '1': // Enable one of the IR Beacons
        if (laptop_packetBuffer[1] == TARGETCMD) {
          toggleLeds(BLUE, PURPLE, 500);
          ledcWrite(ledChannel, dutycycle);

          EVERY_N_MILLIS(100) {
            switch (laptop_packetBuffer[2]) {
              case '1':
                dutycycle+=5;
                if (dutycycle > maxdutycycle)
                  dutycycle = maxdutycycle;
                break;
              case '2':
                dutycycle-=5;
                if (dutycycle < 0)
                  dutycycle = 0;
                break;
            }
            if (laptop_packetBuffer[2] == '1' || laptop_packetBuffer[2] == '2') {
              String msg = "dutycycle : " + String(dutycycle);
              laptop.send(msg);
            }
          }

        } else {
          setLeds(BLACK);
          ledcWrite(ledChannel, 0);
        }
        break;
      default: // Don't enable current IR Beacon if in another mode
        toggleLeds(WHITE, BLACK, 500);
        ledcWrite(ledChannel, 0);
        break;
    }

    
    
  } else {
    toggleLeds(RED, BLACK, 500);
    ledcWrite(ledChannel, 0);
  }
}