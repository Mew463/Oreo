#include <Arduino.h>
#include <LEDHandler.h>
#include <WiFiUdp.h>
#include <WiFi.h>
#include "USB.h"

const char *ssid = "RESNET-BROTECTED";
const char *pswrd = "marbry2025";
const IPAddress baseStationIp = IPAddress(192, 168, 86, 25);
const int baseStationPort = 12346;
const int localUdpPort = 12345;

WiFiUDP udp;
const int packSize = 3;
char packetBuffer[packSize];
bool disconnectedFromConroller = 0;
unsigned long lastTransmission = millis();

void setup()
{
  delay(750);
  init_led();
  USBSerial.begin(115200);
  setLeds(CRGB::Green);

  WiFi.setMinSecurity(WIFI_AUTH_WEP);
  WiFi.begin(ssid, pswrd);
  WiFi.setTxPower(WIFI_POWER_8_5dBm);
  wl_status_t wifistat = WiFi.status();
  while (wifistat != WL_CONNECTED)
  {
    wifistat = WiFi.status();
    switch (wifistat)
    {
    case WL_NO_SSID_AVAIL:
      USBSerial.println("[WiFi] SSID not found");
      break;
    case WL_CONNECT_FAILED:
      USBSerial.print("[WiFi] Failed - WiFi not connected! Reason: ");
      break;
    case WL_CONNECTION_LOST:
      USBSerial.println("[WiFi] Connection was lost");
      break;
    case WL_SCAN_COMPLETED:
      USBSerial.println("[WiFi] Scan is completed");
      break;
    case WL_DISCONNECTED:
      USBSerial.println("[WiFi] WiFi is disconnected");
      break;
    case WL_CONNECTED:
      USBSerial.println("[WiFi] WiFi is connected!");
      USBSerial.print("[WiFi] IP address: ");
      USBSerial.println(WiFi.localIP());
      udp.begin(localUdpPort);
      break;
    default:
      USBSerial.print(".");
    }
    delay(100);
  }
  setLeds(CRGB::Black);
}

void loop()
{
  int packetSize = udp.parsePacket();
  if (packetSize) { // receive incoming UDP packets
    // USBSerial.printf("Received %d bytes from %s, port %d\n", packetSize, udp.remoteIP().toString().c_str(), udp.remotePort());
    int len = udp.read(packetBuffer, packetSize);
    if (len > 0)
      packetBuffer[len] = 0;
    lastTransmission = millis();
    // USBSerial.printf("[CONTROLLER]: %s\n", packetBuffer);
  }

  disconnectedFromConroller = (millis() - lastTransmission > 500);
  if (disconnectedFromConroller) {
    toggleLeds(CRGB::Red, CRGB::Black, 250);
  } else {
    if (packetBuffer[1] == '1') {
      if (packetBuffer[0] == '1')
        setLeds(CRGB::Red);
      else if (packetBuffer[0] == '2')
        setLeds(CRGB::Orange);
      else if (packetBuffer[0] == '3')
        setLeds(CRGB::Blue);
      else if (packetBuffer[0] == '4')
        setLeds(CRGB::Green);
      else
        setLeds(CRGB::Black);
    } else {
    toggleLeds(CRGB::Red, CRGB::Green, 500);
    }
  }
    
  // udp.beginPacket(baseStationIp, baseStationPort); 
  // udp.print("hello from esp32!");
  // udp.endPacket();
  // delay(500);
  // USBSerial.println("sent!");
}
