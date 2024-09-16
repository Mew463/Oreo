#include <Arduino.h>
#ifndef RINGBUFFER_H
#define RINGBUFFER_H
class ringBuffer{
    public:
        /**
         * @brief Construct a ring buffer object.
         *
         * @param _criteria How close should the new value match the max value from the ring buffer to be considered legit?
         */
        ringBuffer(long *_ringBuf, int _arraysize, float _criteria) {
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
        void update(long val) {
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
        bool isLegit(long newVal) {
            long maxVal = getMaxVal();
            if (newVal > maxVal * criteria) 
                return true;
            else
                return false;
        }
        
        /**
         * @brief Returns maximum value from our ring buffer.
         */
        long getMaxVal() {
            long maxVal = 0;
            for (int i = 0; i < arraySize; i++)
                if (maxVal < ringBuf[i])
                    maxVal = ringBuf[i];
            return maxVal;
        }

        long getMinVal() {
            long minVal = ringBuf[0];
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
        long *ringBuf;
        int arraySize;

        int curIndex = 0;
        float criteria = 0;
        
};

#endif