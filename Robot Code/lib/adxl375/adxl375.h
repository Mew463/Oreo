#include <Adafruit_ADXL375.h>
#include <ringBuffer.h>

#define zAccelValuesSize 20

void init_adxl375();

void updateAccel();

void calibrateAccel(bool topside);

float getAccelY();

float getAccelZ();

bool isFlipped();