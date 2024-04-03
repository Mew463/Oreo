#include <Arduino.h>

float get3sVoltage() {
    return (float(analogRead(18)) / 4096)*3.1 * 4.1 * 1.01;
}

float get1sVoltage() {
    return (float(analogRead(18)) / 4096)*3.1 * 2.00 * 1.0625;
}