#include "adxl375.h"

Adafruit_ADXL375 accel = Adafruit_ADXL375(12345);

void init_adxl375() {
    Wire.begin(5,6);
    if (!accel.begin()) {
        while (1) {
        delay(100);
        USBSerial.println("Failed to find ADXL375 chip");
        }
    }
}

float getAccelY() {
    sensors_event_t event;
    accel.getEvent(&event);
    return (abs(event.acceleration.y));
}

float getAccelZ() {
    sensors_event_t event;
    accel.getEvent(&event);
    return (event.acceleration.z);
}

float getGyroZ() {
    static float heading = 0;
    sensors_event_t event;
    accel.getEvent(&event);
    heading += event.orientation.z;
    return (heading);
}