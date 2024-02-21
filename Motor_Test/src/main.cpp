#include <Arduino.h>
#include "/Users/mingweiyeoh/Documents/GitHub/Arduino-Projects/libraries/Custom/USBSerialHandler.h"

SerialHandler myComputer = SerialHandler(); 

const int motpin = 8;
const int motchannel = 0;
const int neutralVal = 120;

void autoMotorTune() {
  delay(1000);
  USBSerial.println("Start with motor off!!");
  ledcWrite(motchannel, 220);
  delay(3000);
  USBSerial.println("Turn motor on!!!");
  delay(5000);
  USBSerial.println("Calibrating neutral");
  ledcWrite(motchannel, neutralVal); // Calibrate neutral stick
  delay(10000);
  USBSerial.println("Done!!!");
}


void setup() {
  USBSerial.begin(115200);  
  ledcSetup(motchannel, 10000, 8);
  ledcAttachPin(motpin, motchannel);
  ledcWrite(motchannel, neutralVal);


  autoMotorTune();

  while (myComputer.getInt(0) == 0) {
    delay(10);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  ledcWrite(motchannel, myComputer.getInt(0));
  delay(10);
}

