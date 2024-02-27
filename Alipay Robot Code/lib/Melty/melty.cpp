#include <melty.h>
#include <LEDHandler.h>

melty::melty() {

}

void melty::update() {
    bool curSeenIRLed = isBeaconSensed(!digitalRead(TOP_IR_PIN));
    // USBSerial.println(digitalRead(TOP_IR_PIN));
    if (curSeenIRLed != lastSeenIRLed)
        if (curSeenIRLed) { // Activates on the rising edge of seeing the IR LED
            setLeds(CRGB::Green);
            currentPulse = millis();
            unsigned long period_MS = currentPulse - lastPulse; // How long it takes to complete one revolution
            RPM = 60000/(period_MS);
            lastPulse = currentPulse;

            unsigned long center_of_beacon = currentPulse + time_seen_beacon/2; // This should ideally be centered on the beacon 
            unsigned long centerOfDrivePulse =  center_of_beacon + (float(deg)/360)*period_MS; // Direction that we should be driving towards
            unsigned long deltaDriveTiming = (percentageOfRotation * period_MS)/2;
            if (timingToggle) { // If it is going to translate whilst computing the new timings we should use a different set of variables
                endDrive = centerOfDrivePulse + deltaDriveTiming;
                startDrive = centerOfDrivePulse - deltaDriveTiming;
            } else { // Do the same but calculate with the other values
                endDrive2 = centerOfDrivePulse + deltaDriveTiming;
                startDrive2  = centerOfDrivePulse - deltaDriveTiming;
            }

            timingToggle = !timingToggle; // Toggle it so that next iteration uses different variables 
        }
        else { // Activates on the falling edge of seeing the IR LED
            setLeds(CRGB::Red);
            time_seen_beacon = millis() - lastPulse;
        }

    lastSeenIRLed = curSeenIRLed;
}

bool melty::translate() { // Returns whether or not robot should translate now
    unsigned long currentTime = millis();
    if (percentageOfRotation != 0)
        return (currentTime > startDrive && currentTime < endDrive || currentTime > startDrive2 && currentTime < endDrive2);
    else
        return 0;
}

bool melty::isBeaconSensed(bool currentReading) {
    
    for (int i = 0; i < IRLedDataSize; i++)
        USBSerial.print(IRLedReadings[i]);
    USBSerial.println();

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