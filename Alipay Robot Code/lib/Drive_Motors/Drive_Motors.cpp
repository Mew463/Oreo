#include "Drive_Motors.h"
#include <Arduino.h>

void init_motors() {
    ledcSetup(lMotChannel, motFreq, resolution);
    ledcSetup(rMotChannel, motFreq, resolution);
    ledcAttachPin(lMotPin, lMotChannel);
    ledcAttachPin(rMotPin, rMotChannel);

    l_motor_write(neutralValue);
    r_motor_write(neutralValue);
}

void l_motor_write(int value) { // From 0 -> 100
    ledcWrite(lMotChannel, neutralValue + value);
}

void r_motor_write(int value) { // From 0 -> 100
    ledcWrite(rMotChannel, neutralValue + value);
}

void set_both_motors(int value) {
    l_motor_write(value);
    r_motor_write(value);
}