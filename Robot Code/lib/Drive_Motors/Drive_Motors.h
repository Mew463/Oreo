#include "DShotESC.h"
#include "FastLED.h"

class Drive_Motors {
  public:
    Drive_Motors() {}
    void init_motors();
    void arm_motors();
    void l_motor_write(int value);
    void r_motor_write(int value);
    void set_both_motors(int value);
    bool isNeutral();
    bool flip_motors = 0;
  private:
    DShotESC rmot;
    DShotESC lmot;

    int kickstart_value = 8;
    int l_motor_value = 0;
    int r_motor_value = 0;
};
