#include <Arduino.h>

float getVoltage() {
    return (float(analogRead(18)) / 4096)*3.1 * 3.95;
}