#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "display.h"
#include "boopsensor.h"
#include "fan.h"

void app_main(void) {
    // Inicializa o subsistema de display 
    display_init();

    // Inicializa o sensor de boop
    boopsensor_init();

    pwm_init();
    set_duty(100);
    
}