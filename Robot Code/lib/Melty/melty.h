#include <Arduino.h>
#include <adxl375.h>

class ringBuffer{
    public:
        /**
         * @brief Construct a ring buffer object.
         *
         * @param _criteria How close should the new value match the max value from the ring buffer to be considered legit?
         */
        ringBuffer(unsigned long *_ringBuf, int _arraysize, float _criteria) {
            ringBuf = _ringBuf;
            arraySize = _arraysize;
            criteria = _criteria;
        }

        ringBuffer() {}

        /**
         * @brief Adds a value to the ring buffer.
         *
         * @param val Value to be added to the ring buffer. 
         */
        void update(unsigned long val) {
            ringBuf[curIndex++] = val;
            if (curIndex > arraySize-1)
                curIndex = 0;
        }

        /**
         * @brief Compares the input value to the max value.
         *
         * @param newVal New value to check against current max value.
         * 
         * @return Whether new value is some percentage of max value. Based on the criteria variable. 
         */
        bool isLegit(unsigned long newVal) {
            unsigned long maxVal = getMaxVal();
            if (newVal > maxVal * criteria) 
                return true;
            else
                return false;
        }
        
        /**
         * @brief Returns maximum value from our ring buffer.
         */
        unsigned long getMaxVal() {
            unsigned long maxVal = 0;
            for (int i = 0; i < arraySize; i++)
                if (maxVal < ringBuf[i])
                    maxVal = ringBuf[i];
            return maxVal;
        }

        unsigned long getMinVal() {
            unsigned long minVal = ringBuf[0];
            for (int i = 1; i < arraySize; i++)
                if (minVal > ringBuf[i])
                    minVal = ringBuf[i];
            if (minVal == 0)
                minVal = 1;
            return minVal;
        }

        String returnArray() { // For debug purposes
            String msg = "";
            for (int i = 0; i < arraySize; i++) {
                msg = msg + String(ringBuf[i]) + " ";
            }
            return msg;
        }

        

    private:
        unsigned long *ringBuf;
        int arraySize;

        int curIndex = 0;
        float criteria = 0;
        
};



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

        unsigned long period_micros_calc_array[20] = {0};
        ringBuffer period_micros_calc;

        unsigned long time_seen_beacon_calc_array[10] = {0};
        ringBuffer time_seen_beacon_calc;

    private:
        bool lastSeenIRLed = 0;
        unsigned long period_micros = micros();
        unsigned long time_seen_beacon = micros();
        unsigned long currentPulse = micros(), lastPulse = micros();
        
        bool IRLedReadings[IRLedDataSize] = {0};
        int IRLedIndex = 0;
        
        bool lastIRLedReturnValue = 0;

        unsigned long startDrive = micros(), endDrive = micros(), startDrive2 = micros(), endDrive2 = micros();

        bool timingToggle = 0;

        

        const int us_per_min = 60000000;
};

 