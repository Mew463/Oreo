#include <Arduino.h>

float getVoltage() {
    return (float(analogRead(18)) / 4096); // <-- Not working properly??
}