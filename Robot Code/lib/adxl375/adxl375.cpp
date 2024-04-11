#include <adxl375.h>

Adafruit_ADXL375 accel = Adafruit_ADXL375(12345);
sensors_event_t event;

long zAccelValues[zAccelValuesSize] = {0};

ringBuffer zRingBuffer = ringBuffer(zAccelValues, zAccelValuesSize, 1);

void init_adxl375() {
    Wire.begin(5,6);
    while (!accel.begin()) {
        delay(100);
        USBSerial.println("Failed to find ADXL375 chip");
    }
}

void updateAccel() {
    accel.getEvent(&event);
}

float getAccelY() {
    // sensors_event_t event;
    // accel.getEvent(&event);
    return (abs(event.acceleration.y));
}

float getAccelZ() {
    // sensors_event_t event;
    // accel.getEvent(&event);
    return (event.acceleration.z-9.8);
}

bool isFlipped() {
    static bool isFlipped = 0;
    long accelValueThreshold = 4;
    zRingBuffer.update(getAccelZ());

    if (!isFlipped) {
        for (int i = 0; i < zAccelValuesSize; i++) 
            if (zAccelValues[i] < -accelValueThreshold)
                return 0;
    } else {
        for (int i = 0; i < zAccelValuesSize; i++) 
            if (zAccelValues[i] > accelValueThreshold)
                return 1;
    }
    
    isFlipped = !isFlipped;
    return isFlipped;
}

