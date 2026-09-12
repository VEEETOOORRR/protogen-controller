#include "fan.h"

void pwm_init(){

    ledc_timer_config_t ledc_timer_fan = {
        .duty_resolution = PWM_DUTY_RESOLUTION,  
        .freq_hz = PWM_FREQ,                   
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .clk_cfg = LEDC_AUTO_CLK
    };

    ledc_channel_config_t ledc_channel_fan = {
        .channel = LEDC_CHANNEL_0,
        .duty = (FAN_DEFAULT_SPEED * 255/100),
        .gpio_num = PIN_FAN,               
        .intr_type = LEDC_INTR_DISABLE,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_sel = LEDC_TIMER_0
    };

    ledc_timer_config(&ledc_timer_fan);
    ledc_channel_config(&ledc_channel_fan);
}

void set_duty(int duty){

    if (duty > 100) duty = 100;
    if (duty < 0) duty = 0;

    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, (duty * 255/100));
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
}

