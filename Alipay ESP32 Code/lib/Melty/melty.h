#include <Arduino.h>
#include <mpu6050.h>

#define TOP_IR_PIN     10
#define BOTTOM_IR_PIN   9
#define IRLedDataSize   10

class melty {
    public:
        melty();
        void update();
        bool isBeaconSensed(bool currentReading);
        bool translate();
        int RPM = 0;
        int deg = 0;
        float percentageOfRotation = 0;
    private:
        bool lastSeenIRLed = 0;
        unsigned long currentPulse = millis();
        unsigned long lastPulse = millis();
        unsigned long time_seen_beacon = millis();

        bool IRLedReadings[IRLedDataSize] = {0};
        int IRLedIndex = 0;
        bool lastIRLedReturnValue = 0;
        unsigned long startDrive = millis();
        unsigned long endDrive = millis();

        unsigned long startDrive2 = millis();
        unsigned long endDrive2 = millis();

        bool timingToggle = 0;
};