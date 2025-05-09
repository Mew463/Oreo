#include "Drive_Motors.h"
#include <Arduino.h>

Drive_Motors::Drive_Motors(gpio_num_t l_motor_pin, rmt_channel_t l_motor_channel, gpio_num_t r_motor_pin, rmt_channel_t r_motor_channel) : l_motor_pin(l_motor_pin), l_motor_channel(l_motor_channel), r_motor_pin(r_motor_pin), r_motor_channel(r_motor_channel) {}

void Drive_Motors::init_motors() {
    rmot.install(r_motor_pin, r_motor_channel); 
    rmot.init();
    rmot.setReversed(false);
    rmot.set3DMode(true);
    rmot.throttleArm(); // <--- Super important!!!;

    lmot.install(l_motor_pin, l_motor_channel); 
    lmot.init();
    lmot.setReversed(false);
    lmot.set3DMode(true);
    lmot.throttleArm(); // <--- Super important!!!;
}

void Drive_Motors::arm_motors() {
	for (int i = 0; i < 10; i++)
	{
		rmot.sendThrottle3D(0);
		lmot.sendThrottle3D(0);
		delay(10);
	}
}

void Drive_Motors::l_motor_write(int value) { 
    l_motor_value = value;
    if (value != 0)
        if (value > 0)
            value += kickstart_value;
        else
            value -= kickstart_value;
    EVERY_N_MILLIS(1) // Important or else DSHOT data is sent too fast.
        if (!flip_motors)
            lmot.sendThrottle3D(value);
        else
            rmot.sendThrottle3D(value);
}

void Drive_Motors::r_motor_write(int value) { 
    r_motor_value = value;
    if (value != 0)
        if (value > 0)
            value += kickstart_value;
        else
            value -= kickstart_value;
    EVERY_N_MILLIS(1) // Important or else DSHOT data is sent too fast.
        if (!flip_motors)
            rmot.sendThrottle3D(value);
        else
            lmot.sendThrottle3D(value);
}

void Drive_Motors::set_both_motors(int value) {
    l_motor_write(value);
    r_motor_write(value);
}

bool Drive_Motors::isNeutral() {
    if (l_motor_value == 0 && r_motor_value == 0)
        return true;
    else
        return false;
}