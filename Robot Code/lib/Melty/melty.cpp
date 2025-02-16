#include <melty.h>
#include <LEDHandler.h>

melty::melty(int top_ir_pin, int bottom_ir_pin) : top_ir_pin(top_ir_pin), bottom_ir_pin(bottom_ir_pin) {
    period_micros_calc = ringBuffer(period_micros_calc_array, 5, 1);
    time_seen_beacon_calc = ringBuffer(time_seen_beacon_calc_array, TIME_SEEN_BEACON_ARRAY_SIZE, 0.7); 
}

bool melty::update() {
    bool curSeenIRLed;
    if (useTopIr)
        curSeenIRLed = isBeaconSensed(!digitalRead(top_ir_pin));
    else
        curSeenIRLed = isBeaconSensed(!digitalRead(bottom_ir_pin));

    if (curSeenIRLed != lastSeenIRLed) {
        if (curSeenIRLed) { // Activates on the rising edge of seeing the IR LED
            currentPulse = micros();
        }
        else { // Activates on the falling edge of seeing the IR LED
            time_seen_beacon = micros() - currentPulse;
            
            time_seen_beacon_calc.update(time_seen_beacon);

            if (time_seen_beacon_calc.isLegit(time_seen_beacon)) { 
                unsigned long curMicros = micros();
                period_micros = curMicros - lastPulse; // How long it takes to complete one revolution
                period_micros_calc.update(period_micros);
                lastPulse = curMicros;
                computeTimings();
            }
        }
    }
    
    lastSeenIRLed = curSeenIRLed;
    return curSeenIRLed;
}

void melty::computeTimings() {
    unsigned long rotation_period;
    ledRPM = (us_per_min)/(period_micros_calc.getMinVal());
    rotation_period = period_micros_calc.getMinVal();


        
    unsigned long center_of_beacon = currentPulse + time_seen_beacon/2; // This should ideally be centered on the beacon 
    unsigned long centerOfDrivePulse =  center_of_beacon + (float(deg)/360)*rotation_period; // Direction that we should be driving towards
    unsigned long deltaDriveTiming = (percentageOfRotation * rotation_period)/2;

    unsigned long offset = 0;
    if (centerOfDrivePulse - deltaDriveTiming < micros()) { // If we're supposed to start translating before we have calculated those values, offset by one rotation
        offset = rotation_period;
    }

    for (int i = 0; i < TRANSLATE_TIMINGS_SIZE/2; i++) {
        int index = 0;
        if (timingToggle) 
            index = i*2;
        else    
            index = i*2+1;

        startDrive[index] = centerOfDrivePulse - deltaDriveTiming + offset + rotation_period*i;
        endDrive[index] = centerOfDrivePulse + deltaDriveTiming + offset + rotation_period*i;

        startDriveInverse[index] = startDrive[index] + rotation_period/2;
        endDriveInverse[index] = endDrive[index] + rotation_period/2;
    }

    timingToggle = !timingToggle; // Toggle it so that next iteration uses different variables 

}

int melty::translate() { // Returns percentage that robot should translate
    unsigned long currentTime = micros();
        
    if (percentageOfRotation != 0) {
        for (int i = 0; i < TRANSLATE_TIMINGS_SIZE; i++) {
            if (currentTime > startDrive[i] && currentTime < endDrive[i]) {
                int delta = endDrive[i] - startDrive[i];
                int timeSinceStart = currentTime - startDrive[i];
                int maxPowerAt = delta/2;
                int distanceFromMax = abs(timeSinceStart - maxPowerAt);

                return 100 - (100 * distanceFromMax/maxPowerAt);
            }
        }

        for (int i = 0; i < TRANSLATE_TIMINGS_SIZE; i++) {
            if (currentTime > startDriveInverse[i] && currentTime < endDriveInverse[i]) {
                int delta = endDriveInverse[i] - startDriveInverse[i];
                int timeSinceStart = currentTime - startDriveInverse[i];
                int maxPowerAt = delta/2;
                int distanceFromMax = abs(timeSinceStart - maxPowerAt);

                return -(100 - (100 * distanceFromMax/maxPowerAt));
            }
        }

    }
    return 0;
}

// bool melty::translateInverse() { // Returns whether or not robot should translate now
//     unsigned long currentTime = micros();
        
//     if (percentageOfRotation != 0) {
//         for (int i = 0; i < TRANSLATE_TIMINGS_SIZE; i++) {
//             if (currentTime > startDriveInverse[i] && currentTime < endDriveInverse[i])
//                 return 1;
//         }
//         return 0;
//     }
//     else
//         return 0;
// }

bool melty::isBeaconSensed(bool currentReading) {
    // for (int i = 0; i < IRLedDataSize; i++)
    //     USBSerial.print(IRLedReadings[i]);
    // USBSerial.println();
    enum states {seen, not_seen};
    
    IRLedReadings[IRLedIndex++] = currentReading; // Add to ring buffer
    if (IRLedIndex == IRLedDataSize)
        IRLedIndex = 0;

    if (lastIRLedReturnValue == seen) {
        for (int i = 0; i < IRLedDataSize; i++)
            if (IRLedReadings[i] == lastIRLedReturnValue) // If any of our readings in the ring buffer is the last value, keep the previous state
                return seen;
        lastIRLedReturnValue = not_seen;
        return not_seen;
    } else { // This is to cover the not_seen case
        for (int i = 0; i < IRLedDataSize; i++)
            if (IRLedReadings[i] == lastIRLedReturnValue) 
                return not_seen;
        lastIRLedReturnValue = seen;
        return seen;
    }
}
