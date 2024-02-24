#include <WiFi.h>

void setup() {
  // put your setup code here, to run once:
  USBSerial.begin(115200);
  USBSerial.println();
  USBSerial.print("ESP Board MAC Address:  ");
  USBSerial.println(WiFi.macAddress());
}

void loop() {
  delay(1000);
  USBSerial.println(WiFi.macAddress());
  // put your main code here, to run repeatedly:
}