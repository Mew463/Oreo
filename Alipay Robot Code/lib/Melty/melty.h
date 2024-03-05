#include <Arduino.h>
#include <mpu6050.h>

#define RINGBUFSIZE 5

class ringBuffer{
    public:
        /**
         * @brief Construct a ring buffer object.
         *
         * @param _criteria How close should the new value match the max value from the ring buffer to be considered legit?
         */
        ringBuffer(float _criteria) {
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
            if (curIndex > RINGBUFSIZE-1)
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
            for (int i = 0; i < RINGBUFSIZE; i++)
                if (maxVal < ringBuf[i])
                    maxVal = ringBuf[i];
            return maxVal;
        }

    private:
        unsigned long ringBuf[RINGBUFSIZE];
        int curIndex = 0;
        float criteria = 0;
        String returnArray() { // For debug purposes
            String msg = "";
            for (int i = 0; i < RINGBUFSIZE; i++) {
                msg = msg + String(ringBuf[i]) + " ";
            }
            return msg;
        }
};



#define TOP_IR_PIN      10
#define BOTTOM_IR_PIN   9
#define IRLedDataSize   25 // Size of our Ring Buffer that will hold the IR Led data 

class melty {
    public:
        melty();
        void update();
        bool isBeaconSensed(bool currentReading);
        void computeTimings();
        bool translate();
        int RPM = 0;
        int deg = 0;
        float percentageOfRotation = 0;
        bool useTopIr = 1;
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

        ringBuffer period_micros_calc;
        ringBuffer time_seen_beacon_calc;

        const int us_per_min = 60000000;
};

