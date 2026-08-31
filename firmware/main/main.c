#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_timer.h"
#include "display.h"

static StaticFace_t buffer;

void teste(){
    for(int i = 0; i < 8; i++){
        display_write_all(MAX7219_REG_DIGIT0 + i, 1 << i);
    }

    vTaskDelay(100);

    for(int i = 0; i < 8; i++)
    display_write_all(MAX7219_REG_DIGIT0 + i, 0);

    vTaskDelay(100);

}


void app_main(){

    spi_conf();
    display_clean_all();

    buffer_update_left_eye(&buffer, EXPRESSAO_OLHOESQUERDO);
    buffer_update_left_mouth(&buffer, EXPRESSAO_BOCAESQUERDA);
    buffer_update_left_nose(&buffer, EXPRESSAO_NARIZESQUERDO);
    buffer_update_right_nose(&buffer, EXPRESSAO_NARIZDIREITO);
    buffer_update_right_mouth(&buffer, EXPRESSAO_BOCADIREITA);
    buffer_update_right_eye(&buffer, EXPRESSAO_OLHODIREITO);



    while(1) {
        display_update(&buffer);
        vTaskDelay(100);

    }
}

