#include "control.h"

static QueueHandle_t control_event_queue = NULL;


static void control_task(void *){
    system_event_t event;


    // INSTANCIADOS APENAS POR RAZÕES DE TESTE!!
    display_cmd_t neutral = {
        .left_eye = EYE_CMD_NEUTRAL,
        .left_nose = NOSE_CMD_NEUTRAL,
        .right_eye = EYE_CMD_NEUTRAL,
        .right_nose = NOSE_CMD_NEUTRAL   
    };
    display_cmd_t boop = {
        .left_eye = EYE_CMD_BOOP,
        .left_mouth = MOUTH_CMD_BOOP,
        .left_nose = NOSE_CMD_NEUTRAL,
        .right_eye = EYE_CMD_BOOP,
        .right_mouth = MOUTH_CMD_BOOP,
        .right_nose = NOSE_CMD_NEUTRAL
    };

    display_cmd_t silent_mouth = {
        .left_mouth = MOUTH_CMD_NEUTRAL,
        .right_mouth = MOUTH_CMD_NEUTRAL
    };

    display_cmd_t half_mouth = {
        .left_mouth = MOUTH_CMD_TALK_HALF,
        .right_mouth = MOUTH_CMD_TALK_HALF
    };

    display_cmd_t full_mouth = {
        .left_mouth = MOUTH_CMD_TALK_FULL,
        .right_mouth = MOUTH_CMD_TALK_FULL
    };

    ledstrip_change_color(RGB_STRIP_BOTH, 0, 16, (Rgb_led){255, 200, 0});

    for(;;){
        if (xQueueReceive(control_event_queue, &event, portMAX_DELAY) == pdTRUE) {
            switch (event.type){
                case EVENT_BOOP_TOUCHED: // Implementando desse jeito enquanto não penso numa forma melhor de controlar o tempo em que fica boopado
                    display_send_expression(&boop);
                    ledstrip_change_color(RGB_STRIP_BOTH, 0, 16, (Rgb_led){100, 200, 0});
                    vTaskDelay(pdMS_TO_TICKS(2000));
                    display_send_expression(&neutral);
                    ledstrip_change_color(RGB_STRIP_BOTH, 0, 16, (Rgb_led){255, 200, 0});
                    break;

                case EVENT_BOOP_RELEASED:
                    display_send_expression(&neutral);
                    break;

                case EVENT_VOICE_SILENCE:
                    display_send_expression(&silent_mouth);
                    break;

                case EVENT_VOICE_TALK_HALF:
                    display_send_expression(&half_mouth);
                    break;

                case EVENT_VOICE_TALK_FULL:
                    display_send_expression(&full_mouth);
                    break;



                case EVENT_IMU_TILT_RIGHT:
                    display_send_expression(&boop);
                    ledstrip_change_color(RGB_STRIP_R, 0, 16, (Rgb_led){255, 200, 0});
                    ledstrip_change_color(RGB_STRIP_L, 0, 16, (Rgb_led){0, 0, 0});
                    break;

                case EVENT_IMU_TILT_LEFT:
                    display_send_expression(&boop);
                    ledstrip_change_color(RGB_STRIP_L, 0, 16, (Rgb_led){255, 200, 0});
                    ledstrip_change_color(RGB_STRIP_R, 0, 16, (Rgb_led){0, 0, 0});
                    break;

                case EVENT_IMU_CENTERED:
                    display_send_expression(&neutral);
                    ledstrip_change_color(RGB_STRIP_L, 0, 16, (Rgb_led){255, 200, 0});
                    ledstrip_change_color(RGB_STRIP_R, 0, 16, (Rgb_led){255, 200, 0});
                    break;
                    
                default:
                    break;
            }
        }       
    }

}

void control_init(){
    control_event_queue = xQueueCreate(10, sizeof(system_event_t));
    xTaskCreate(control_task, "control_task", 4096, NULL, 5, NULL);
}

void control_send_event(system_event_t *event){
    xQueueSend(control_event_queue, event, 0);
}