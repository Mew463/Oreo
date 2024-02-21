#include <melty.h>
#include <LEDHandler.h>

melty::melty() {

}

int melty::updateRPM() {
    bool curSeenIRLed = isBeaconSensed(!digitalRead(TOP_IR_PIN));
    // USBSerial.println(curSeenIRLed);
    // delay(10);
    if (curSeenIRLed != lastSeenIRLed)
        if (curSeenIRLed) {
            setLeds(CRGB::Green);
            currentPulse = millis();
            int millisPerRev = currentPulse - lastPulse;
            RPM = 60000/millisPerRev;
            lastPulse = millis();
        }
        else
            setLeds(CRGB::Red);

    lastSeenIRLed = curSeenIRLed;

    return RPM;
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