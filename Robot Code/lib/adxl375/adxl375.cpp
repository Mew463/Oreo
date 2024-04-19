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
    return (abs(event.acceleration.y));
}

float getAccelZ() {
    return (event.acceleration.z);
}

bool isFlipped() {
    static bool isFlipped = 0;
    long accelValueThreshold = 6;
    zRingBuffer.update(getAccelZ());

    if (!isFlipped) {
        for (int i = 0; i < zAccelValuesSize; i++)  // Negative values mean robot is NOT flipped
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

