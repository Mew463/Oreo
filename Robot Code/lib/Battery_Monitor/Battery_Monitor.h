#include <Arduino.h>

int getPerc(int min, int max, int val) {
    if (val == 0) // Switch not on
        return -1;
    int perc = 100 * (val - min) / double(max - min);
    if (perc > 100) 
        perc = 100;
    if (perc < 0)
        perc = 0;
    return perc;

}
int get3sSOC() {
    int max = 4096-20;
    int min = 3300;
    return getPerc(min, max ,analogRead(18));
}

int get1sSOC() {
    int max = 2585-50;
    int min = 2048;
    return getPerc(min, max ,analogRead(18));
}
