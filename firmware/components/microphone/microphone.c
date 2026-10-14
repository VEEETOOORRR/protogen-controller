#include "microphone.h"

static adc_oneshot_unit_handle_t adc1_handle;
static TaskHandle_t mic_task_handle = NULL;

adc_unit_t unit_id;
adc_channel_t channel;

static void adc_init() {

    adc_oneshot_io_to_channel(MIC_PIN, &unit_id, &channel);

    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = unit_id, 
    };
    // O uso do ESP_ERROR_CHECK é fundamental para travar o boot se faltar memória
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &adc1_handle));

    // Configura o Canal específico daquele pino
    adc_oneshot_chan_cfg_t config = {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_12,
    };
    
    adc_oneshot_config_channel(adc1_handle, channel, &config);
}

static void mic_task(void *arg){
    system_event_t event = {.type = EVENT_VOICE_SILENCE, .payload = 0};

    event_type_t last_event = EVENT_IMU_TILT_RIGHT; // Evento aleatório inicial

    int adc_raw;

    for(;;){
        int adc_sum = 0;

        for(int i = 0; i < 10; i++){
            if(adc_oneshot_read(adc1_handle, channel, &adc_raw) == ESP_OK){
                adc_sum += adc_raw;
            }
        }

        int adc_average = (4095 - adc_sum / 10); // Saída do módulo microfone tem pull-up!
        
        event_type_t current_event;

        if(adc_average > 1000){
            current_event = EVENT_VOICE_TALK_FULL;
        } else if(adc_average > 200){
            current_event = EVENT_VOICE_TALK_HALF;
        } else {
            current_event = EVENT_VOICE_SILENCE;
        }

        if(current_event != last_event){
            event.type = current_event;
            control_send_event(&event);
            last_event = current_event;
        }

        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

void mic_init(){
    adc_init();

    xTaskCreate(mic_task, "mic_task", 2048, NULL, 10, &mic_task_handle);

}



