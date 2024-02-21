#include <Arduino.h>
#include <mpu6050.h>

#define TOP_IR_PIN     10
#define BOTTOM_IR_PIN   9
#define IRLedDataSize   30

class melty {
    public:
        melty();
        int updateRPM();
        bool isBeaconSensed(bool currentReading);
        int RPM = 0;
    private:
        bool lastSeenIRLed = 0;
        unsigned long currentPulse = millis();
        unsigned long lastPulse = millis();

        bool IRLedReadings[IRLedDataSize] = {0};
        int IRLedIndex = 0;
        bool lastIRLedReturnValue = 0;
};