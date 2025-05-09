#include "DShotESC.h"
#include "FastLED.h"

class Drive_Motors {
  public:
    Drive_Motors(gpio_num_t l_motor_pin, rmt_channel_t l_motor_channel, gpio_num_t r_motor_pin, rmt_channel_t r_motor_channel);
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

    gpio_num_t l_motor_pin = GPIO_NUM_NC;
    gpio_num_t r_motor_pin = GPIO_NUM_NC;

    rmt_channel_t l_motor_channel;
    rmt_channel_t r_motor_channel;

    int kickstart_value = 8;
    int l_motor_value = 0;
    int r_motor_value = 0;
};
