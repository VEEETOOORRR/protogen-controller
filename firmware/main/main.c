#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "display.h"
#include "boopsensor.h"
#include "fan.h"
#include "ledstrip.h"
#include "imu.h"

void app_main(void) {
    // Inicializa o subsistema de display 
    display_init();

    // Inicializa o sensor de boop
    boopsensor_init();

    pwm_init();

    ledstrip_init();

    imu_init();

    Rgb_led rgb;



    set_duty(100);


    while(1){
        for(int i = 0; i <= 255; i++){
            rgb.r = i;
            rgb.g = i + 85;
            rgb.b = i + 170;
            ledstrip_change_color(RGB_STRIP_BOTH, 0, 15, rgb);
            vTaskDelay(pdMS_TO_TICKS(20));
        }


    }
    
}