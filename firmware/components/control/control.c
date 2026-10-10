#include "control.h"

static QueueHandle_t control_event_queue = NULL;


static void control_task(void *){
    system_event_t event;

    for(;;){
        if (xQueueReceive(control_event_queue, &event, portMAX_DELAY) == pdTRUE) {
            switch (event.type){
            
            case EVENT_BOOP_TOUCHED:
                break;

            case EVENT_BOOP_RELEASED:
                break;

            case EVENT_VOICE_TALK_HALF:
                break;

            case EVENT_VOICE_TALK_FULL:
                break;

            case EVENT_VOICE_SILENCE:
                break;

            case EVENT_IMU_TILT_RIGHT:
                break;

            case EVENT_IMU_TILT_LEFT:
                break;

            case EVENT_IMU_CENTERED:
                break;

            case EVENT_APP_CHANGE_COLOR:
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