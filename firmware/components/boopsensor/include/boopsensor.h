#ifndef TOUCHSENSOR_H
#define TOUCHSENSOR_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#include "control.h"
#include "system_events.h"


#define BOOPSENSOR_DATA_PIN 1
#define BOOPSENSOR_ENABLE_PIN 2

// Inicializa sensor, interrupção e task do boopsensor
void boopsensor_init();

#endif