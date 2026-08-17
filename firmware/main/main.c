#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_timer.h"
#include "display.h"

void app_main(){

    spi_conf();

    // Pequeno loop de teste: Pisca o primeiro LED de cada matriz para testar
    while(1) {
        display_write_all(MAX7219_REG_DIGIT0, 0x01); // Liga o primeiro bit da linha 0
        vTaskDelay(pdMS_TO_TICKS(500));
        display_write_all(MAX7219_REG_DIGIT0, 0x00); // Desliga
        vTaskDelay(pdMS_TO_TICKS(500));
    }

}

