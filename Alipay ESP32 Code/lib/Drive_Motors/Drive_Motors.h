#define lMotPin      8
#define rMotPin      7
#define motFreq      10000
#define resolution   8 // # of bits
#define lMotChannel  0
#define rMotChannel  1

#define neutralValue 120

void init_motors();

void l_motor_write(int value);

void r_motor_write(int value);

void set_both_motors(int value);