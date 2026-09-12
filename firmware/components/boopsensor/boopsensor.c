#include "boopsensor.h"

static TaskHandle_t boop_task_handle = NULL;

static void IRAM_ATTR boop_isr_handler(void* arg) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    
    // Destrava a Task que está esperando pelo evento
    vTaskNotifyGiveFromISR(boop_task_handle, &xHigherPriorityTaskWoken);
    
    // Se a Task acordada tiver uma prioridade maior que a Task atualmente em execução,
    // solicita ao FreeRTOS uma troca de contexto imediata ao sair da interrupção.
    if (xHigherPriorityTaskWoken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

// 2. A Task - Roda no contexto do RTOS
static void boop_task(void* arg) {
    for(;;) {
        // Fica bloqueada (consumindo 0% de CPU) até receber a notificação da ISR
        // O pdTRUE zera o contador de notificações após a leitura
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        
        // Envia o comando de BOOP para a fila do display
        if (display_cmd_queue != NULL) {
            display_cmd_t cmd = DISPLAY_CMD_BOOP;
            xQueueSend(display_cmd_queue, &cmd, 0);
        }
    }
}

void boopsensor_init(){

    // Criação da task
    xTaskCreate(boop_task, "boop_task", 2048, NULL, 10, &boop_task_handle);

    gpio_config_t boop_gpio_conf = {
        .pin_bit_mask = (1ULL << BOOPSENSOR_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .intr_type = GPIO_INTR_POSEDGE
    };

    gpio_config(&boop_gpio_conf);

    // Instala o serviço global de interrupções
    // O flag ESP_INTR_FLAG_IRAM obriga que o serviço aloque os handlers na RAM.
    gpio_install_isr_service(ESP_INTR_FLAG_IRAM);

    // Registra a sua função ISR específica para o pino do TTP223
    gpio_isr_handler_add(BOOPSENSOR_PIN, boop_isr_handler, NULL);



}

