#ifndef TOUCHSENSOR_H
#define TOUCHSENSOR_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#include "control.h"
#include "system_events.h"


#define BOOPSENSOR_PIN 21

// Inicializa sensor, interrupção e task do boopsensor
void boopsensor_init();

#endif