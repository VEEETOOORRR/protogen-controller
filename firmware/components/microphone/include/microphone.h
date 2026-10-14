#ifndef MICROPHONE_H
#define MICROPHONE_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc/adc_oneshot.h"

#include "control.h"

#define MIC_PIN 6 // ADC1_5

void mic_init();

#endif