#include <Adafruit_ADXL375.h>
#include <ringBuffer.h>

#define zAccelValuesSize 10

void init_adxl375();

void updateAccel();

float getAccelY();

float getAccelZ();

bool isFlipped();