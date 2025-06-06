#include <Arduino.h>

int getPerc(int min, int max, int val) {
    if (val < 100) // Switch not on
        return -1;
    int perc = 100 * (val - min) / double(max - min);
    if (perc > 100) 
        perc = 100;
    if (perc < 0)
        perc = 0;
    return perc;

}
int get3sSOC(int pin) {
    int max = 4096-50;
    int min = 3000;
    return getPerc(min, max ,analogRead(pin));
}

int get1sSOC(int pin) {
    int max = 2900-50;
    int min = 2048;
    return getPerc(min, max ,analogRead(pin));
}
