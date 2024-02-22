#include <melty.h>
#include <LEDHandler.h>

melty::melty() {

}

void melty::update() {
    bool curSeenIRLed = isBeaconSensed(!digitalRead(TOP_IR_PIN));
    if (curSeenIRLed != lastSeenIRLed)
        if (curSeenIRLed) { // Activates on the rising edge of seeing the IR LED
            setLeds(CRGB::Green);
            currentPulse = millis();
            unsigned long deltaPulse = currentPulse - lastPulse; // How long it takes to complete one revolution
            RPM = 60000/(deltaPulse);
            lastPulse = currentPulse;

            unsigned long midPulse = currentPulse + pulseWidth/2; // This should ideally be centered on the beacon 
            unsigned long centerOfDrivePulse =  midPulse + (float(deg)/360)*deltaPulse; // Direction that we should be driving towards
            unsigned long deltaDriveTiming = (percentageOfRotation * deltaPulse)/2;
            if (timingToggle) { // If it is going to translate whilst computing the new timings we should use a different set of variables
                endDrive = centerOfDrivePulse + deltaDriveTiming;
                startDrive = centerOfDrivePulse - deltaDriveTiming;
            } else { // Do the same but calculate with the other values
                endDrive2 = centerOfDrivePulse + deltaDriveTiming;
                startDrive2  = centerOfDrivePulse - deltaDriveTiming;
            }

            timingToggle = !timingToggle; // Toggle it so that next iteration uses different variables 
        }
        else {
            setLeds(CRGB::Red);
            pulseWidth = millis() - lastPulse;
        }

    lastSeenIRLed = curSeenIRLed;
}

bool melty::translate() {
    unsigned long currentTime = millis();
    if (percentageOfRotation != 0)
        return (currentTime > startDrive && currentTime < endDrive || currentTime > startDrive2 && currentTime < endDrive2);
    else
        return 0;
}

bool melty::isBeaconSensed(bool currentReading) {
    IRLedReadings[IRLedIndex++] = currentReading; // This is just code for a ring buffer
    if (IRLedIndex == IRLedDataSize)
        IRLedIndex = 0;

    if (lastIRLedReturnValue) {
        for (int i = 0; i < IRLedDataSize; i++)
            if (IRLedReadings[i] == lastIRLedReturnValue) 
                return true;
        lastIRLedReturnValue = !lastIRLedReturnValue;
    } else {
        for (int i = 0; i < IRLedDataSize; i++)
            if (IRLedReadings[i] == lastIRLedReturnValue) 
                return false;
        lastIRLedReturnValue = !lastIRLedReturnValue;
    }
    return lastIRLedReturnValue;
}