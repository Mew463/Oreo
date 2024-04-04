#include <Arduino.h>
#include <adxl375.h>
#include <ringBuffer.h>

#define TOP_IR_PIN      9
#define BOTTOM_IR_PIN   10
#define IRLedDataSize   40  // Size of our Ring Buffer that will hold the IR Led data 

class melty {
    public:
        melty();
        bool update();
        bool isBeaconSensed(bool currentReading);
        void computeTimings();
        bool translate();
        int RPM = 0;
        int deg = 0;
        float percentageOfRotation = 0;
        bool useTopIr = 1;

        long period_micros_calc_array[20] = {0};
        ringBuffer period_micros_calc;

        long time_seen_beacon_calc_array[10] = {0};
        ringBuffer time_seen_beacon_calc;

    private:
        bool lastSeenIRLed = 0;
        long period_micros = micros();
        long time_seen_beacon = micros();
        long currentPulse = micros(), lastPulse = micros();
        
        bool IRLedReadings[IRLedDataSize] = {0};
        int IRLedIndex = 0;
        
        bool lastIRLedReturnValue = 0;

        long startDrive = micros(), endDrive = micros(), startDrive2 = micros(), endDrive2 = micros();

        bool timingToggle = 0;

        

        const int us_per_min = 60000000;
};

 