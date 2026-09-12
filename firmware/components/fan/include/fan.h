#ifndef FAN_H
#define FAN_H

#include "driver/ledc.h"

#define PIN_FAN 14
#define PWM_FREQ 1000
#define PWM_DUTY_RESOLUTION LEDC_TIMER_8_BIT

#define FAN_DEFAULT_SPEED 100

void pwm_init();
void set_duty(int duty);

#endif