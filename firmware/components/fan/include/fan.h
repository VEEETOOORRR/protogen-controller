#ifndef FAN_H
#define FAN_H

#include "driver/ledc.h"



#define FAN_DEFAULT 100


void pwm_init();
void set_duty(int duty);








#endif