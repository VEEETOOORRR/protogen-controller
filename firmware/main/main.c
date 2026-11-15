#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "display.h"
#include "boopsensor.h"
#include "fan.h"
#include "ledstrip.h"
#include "control.h"
#include "imu.h"
#include "microphone.h"

void app_main(void) {
    // Inicializa o subsistema de display 
    display_init();

    // Inicializa fitas de led
    ledstrip_init();

    // Inicializa o subsistema de controle
    control_init();

    // Inicializa o sensor de boop
    boopsensor_init();

    // Inicializa ventoinha
    pwm_init();

    // Inicializa microfone
    mic_init();

    // Inicializa IMU
    imu_init();

    set_duty(100);
}