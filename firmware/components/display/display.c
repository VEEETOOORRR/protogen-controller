#include <string.h>

#include "display.h"
#include "expressions.h"


// Handle para o SPI
static spi_device_handle_t spi_handle;

// Fila global para receber os comandos
QueueHandle_t display_cmd_queue = NULL;

static void set_expression_neutral(StaticFace_t *face) {
    buffer_update_left_eye(face, EYE_L_NEUTRAL);
    buffer_update_right_eye(face, EYE_R_NEUTRAL);
    buffer_update_left_mouth(face, MOUTH_L_NEUTRAL);
    buffer_update_right_mouth(face, MOUTH_R_NEUTRAL);
    buffer_update_left_nose(face, NOSE_L_NEUTRAL);
    buffer_update_right_nose(face, NOSE_R_NEUTRAL);
}

static void set_expression_boop(StaticFace_t *face) {
    buffer_update_left_eye(face, EYE_L_BOOP);
    buffer_update_right_eye(face, EYE_R_BOOP);
    buffer_update_left_mouth(face, MOUTH_L_BOOP);
    buffer_update_right_mouth(face, MOUTH_R_BOOP);
    buffer_update_left_nose(face, NOSE_L_BOOP);
    buffer_update_right_nose(face, NOSE_R_BOOP);
}


static void display_task(void* arg){
    StaticFace_t current_face;
    display_cmd_t cmd;
    
    // O tempo inicial de espera na fila é "Infinito"
    TickType_t wait_time = portMAX_DELAY; 

    // Estado inicial
    set_expression_neutral(&current_face);
    display_update(&current_face);

    for(;;){
        // Fica bloqueado até receber algo OU até o wait_time acabar
        if (xQueueReceive(display_cmd_queue, &cmd, wait_time) == pdTRUE) {
            
            // Recebeu um comando explicitamente
            if (cmd == DISPLAY_CMD_BOOP) {
                set_expression_boop(&current_face);
                display_update(&current_face);
                
                // Muda a paciência da fila para 3 segundos. 
                // Se não houver novos toques em 3s, ela vai dar timeout.
                wait_time = pdMS_TO_TICKS(3000); 
            } 
            else if (cmd == DISPLAY_CMD_NEUTRAL) {
                set_expression_neutral(&current_face);
                display_update(&current_face);
                wait_time = portMAX_DELAY; // Volta a dormir para sempre
            }

        } else {
            // Ocorreu o Timeout! Ninguém mandou comando nos últimos 3 segundos.
            set_expression_neutral(&current_face);
            display_update(&current_face);
            
            // Restaura a espera para infinito para não ficar atualizando a tela à toa
            wait_time = portMAX_DELAY; 
        }
    }
}

void display_init(){
    spi_conf();

    // Cria a fila para receber comandos de outras tasks
    display_cmd_queue = xQueueCreate(5, sizeof(display_cmd_t));

    // Cria a task do display
    xTaskCreate(display_task, "display_task", 4096, NULL, 5, NULL);
}

void spi_conf(){

    // Configuração do barramento SPI
    spi_bus_config_t busfcg = {
        .miso_io_num = PIN_NUM_MISO,
        .mosi_io_num = PIN_NUM_MOSI,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = NUM_MODULES * 2
    };

    // Configuração específica para o MAX7219
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 1000000, // 1 MHz
        .mode = 0,
        .spics_io_num = PIN_NUM_CS,
        .queue_size = 7
    };

    // Inicializa barramento
    spi_bus_initialize(SPI2_HOST, &busfcg, SPI_DMA_CH_AUTO); 

    // Adiciona dispositivo ao barramento
    spi_bus_add_device(SPI2_HOST, &devcfg, &spi_handle);

    display_write_all(MAX7219_REG_DISPLAY_TEST, 0x00); // Desativa modo de teste (se estivesse ativo)
    display_write_all(MAX7219_REG_SCAN_LIMIT, 0x07);   // Configura para escanear todas as 8 linhas/colunas
    display_write_all(MAX7219_REG_DECODE_MODE, 0x00);  // Modo Matriz de Led crua (sem decodificação BCD de 7 segmentos)
    display_write_all(MAX7219_REG_INTENSITY, DISPLAY_INTENSITY);    // Brilho inicial baixo (0x00 a 0x0F). Bom para poupar energia em testes!

    // Limpa todas as linhas/colunas (apaga tudo antes de ligar a tela para evitar flashes estáticos)
    for (int i = 1; i <= 8; i++) {
        display_write_all(i, 0x00);
    }

    display_write_all(MAX7219_REG_SHUTDOWN, 0x01);    // Acorda o chip (Modo normal de operação)

}

// Função para escrever em todos os módulos do display
void display_write_all(uint8_t reg, uint8_t data){
    
    uint8_t *tx_buf = malloc(NUM_MODULES * 2);

    for (int i = 0; i < NUM_MODULES * 2; i += 2){
        tx_buf[i] = reg;
        tx_buf[i+1] = data;
    }

    spi_transaction_t t;

    memset(&t, 0, sizeof(t)); // Limpa t

    t.length = NUM_MODULES * 2 * 8;
    t.tx_buffer = tx_buf;

    spi_device_transmit(spi_handle, &t);

    free(tx_buf);
}

void display_clean_all(){
    for(uint8_t i = 0; i < 8; i++){
        display_write_all(MAX7219_REG_DIGIT0 + i, 0);
    }
}

void display_update(StaticFace_t *buffer){

    // lineariza o buffer
    uint8_t *buffer_linearizado[NUM_MODULES] = {

        buffer->left_eye[1],
        buffer->left_eye[0],

        buffer->left_mouth[3],
        buffer->left_mouth[2],
        buffer->left_mouth[1],
        buffer->left_mouth[0],

        buffer->left_nose[0],
        buffer->right_nose[0],

        buffer->right_mouth[3],
        buffer->right_mouth[2],
        buffer->right_mouth[1],
        buffer->right_mouth[0],

        buffer->right_eye[1],
        buffer->right_eye[0]
    };

    for (int linha = 0; linha < 8; linha++) {

        uint8_t tx_data[28];
        
        for (int comando = 0; comando < 28; comando += 2) {
            int matriz_atual = comando / 2; // Vai de 0 a 13
            
            tx_data[comando] = (MAX7219_REG_DIGIT0 + 7) - linha; // Gambiarra para inverter as matrizes de baixo pra cima
            tx_data[comando + 1] = buffer_linearizado[matriz_atual][linha];
        }

        spi_transaction_t t;
        memset(&t, 0, sizeof(t)); // Limpa a struct a ser enviada

        t.length = 14 * 2 * 8;    // 224 bits (28 bytes no total)
        t.tx_buffer = tx_data;

        spi_device_polling_transmit(spi_handle, &t);
    }
}

// Olho Esquerdo / Direito
void buffer_update_left_eye(StaticFace_t *buffer, const Eye e) {
    memcpy(buffer->left_eye, e, sizeof(Eye));
}

void buffer_update_right_eye(StaticFace_t *buffer, const Eye e) {
    memcpy(buffer->right_eye, e, sizeof(Eye));
}

// Boca Esquerda / Direita
void buffer_update_left_mouth(StaticFace_t *buffer, const Mouth m) {
    memcpy(buffer->left_mouth, m, sizeof(Mouth));
}

void buffer_update_right_mouth(StaticFace_t *buffer, const Mouth m) {
    memcpy(buffer->right_mouth, m, sizeof(Mouth));
}

// Nariz Esquerdo / Direito
void buffer_update_left_nose(StaticFace_t *buffer, const Nose n) {
    memcpy(buffer->left_nose, n, sizeof(Nose));
}

void buffer_update_right_nose(StaticFace_t *buffer, const Nose n) {
    memcpy(buffer->right_nose, n, sizeof(Nose));
}


