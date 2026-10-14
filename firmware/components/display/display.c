#include <string.h>

#include "display.h"
#include "expressions.h"

// Typedef usado para determinar qual lado do buffer é preciso mudar
typedef enum {
    DISPLAY_L,
    DISPLAY_R,
    DISPLAY_BOTH
} display_side_t;

// Handle para o SPI
static spi_device_handle_t spi_handle = NULL;

// Fila global para receber os comandos
QueueHandle_t display_cmd_queue = NULL;

static void display_task(void* arg);
static void spi_conf(void);
static void display_write_all(uint8_t reg, uint8_t data);
static void display_clean_all(void);
static void display_update(StaticFace_t *buffer);
static void apply_eye_cmd(StaticFace_t *buffer, eye_cmd_t eye_cmd, display_side_t side);
static void apply_mouth_cmd(StaticFace_t *buffer, mouth_cmd_t mouth_cmd, display_side_t side);
static void apply_nose_cmd(StaticFace_t *buffer, nose_cmd_t nose_cmd, display_side_t side);

// Task do display
static void display_task(void* arg){
    display_cmd_t cmd;

    // Estado inicial
    StaticFace_t current_face;

    // Aplica expressão neutra padrão no início do programa
    apply_eye_cmd(&current_face, EYE_CMD_NEUTRAL, DISPLAY_BOTH);
    apply_mouth_cmd(&current_face, MOUTH_CMD_NEUTRAL, DISPLAY_BOTH);
    apply_nose_cmd(&current_face, NOSE_CMD_NEUTRAL, DISPLAY_BOTH);

    display_update(&current_face);

    for(;;){
        // Fica bloqueado até receber algo
        if (xQueueReceive(display_cmd_queue, &cmd, portMAX_DELAY) == pdTRUE) {

            int changed = 0;

            if(cmd.left_eye != EYE_CMD_NO_CHANGE){
                changed = 1;
                apply_eye_cmd(&current_face, cmd.left_eye, DISPLAY_L);
            }

            if(cmd.left_mouth != MOUTH_CMD_NO_CHANGE){
                changed = 1;
                apply_mouth_cmd(&current_face, cmd.left_mouth, DISPLAY_L);
            }

            if(cmd.left_nose != NOSE_CMD_NO_CHANGE){
                changed = 1;
                apply_nose_cmd(&current_face, cmd.left_nose, DISPLAY_L);
            }

            if(cmd.right_eye != EYE_CMD_NO_CHANGE){
                changed = 1;
                apply_eye_cmd(&current_face, cmd.right_eye, DISPLAY_R);
            }

            if(cmd.right_mouth != MOUTH_CMD_NO_CHANGE){
                changed = 1;
                apply_mouth_cmd(&current_face, cmd.right_mouth, DISPLAY_R);
            }

            if(cmd.right_nose != NOSE_CMD_NO_CHANGE){
                changed = 1;
                apply_nose_cmd(&current_face, cmd.right_nose, DISPLAY_R);
            }

            if(changed == 1){
                display_update(&current_face);
            }
        }
    }
}

// Inicializa periférico SPI
static void spi_conf(){

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
static void display_write_all(uint8_t reg, uint8_t data){
    
    uint8_t tx_buf[NUM_MODULES * 2];

    for (int i = 0; i < NUM_MODULES * 2; i += 2){
        tx_buf[i] = reg;
        tx_buf[i+1] = data;
    }

    spi_transaction_t t;

    memset(&t, 0, sizeof(t)); // Limpa t

    t.length = NUM_MODULES * 2 * 8;
    t.tx_buffer = tx_buf;

    spi_device_transmit(spi_handle, &t);
}

// Função auxiliar para limpar todos os displays
static void display_clean_all(){
    for(uint8_t i = 0; i < 8; i++){
        display_write_all(MAX7219_REG_DIGIT0 + i, 0);
    }
}

// Função auxiliar para atualizar todos os displays de acordo com o que está no buffer
static void display_update(StaticFace_t *buffer){

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

// Função auxiliar para traduzir um comando recebido em uma expressão para o olho
static void apply_eye_cmd(StaticFace_t *buffer, eye_cmd_t eye_cmd, display_side_t side){
    if(side == DISPLAY_L || side == DISPLAY_BOTH){
        switch(eye_cmd){
            case EYE_CMD_NEUTRAL:
                memcpy(&buffer->left_eye, EYE_L_NEUTRAL, sizeof(Eye));
                break;

            case EYE_CMD_BOOP:
                memcpy(&buffer->left_eye, EYE_L_BOOP, sizeof(Eye));
                break;

            default:
                break;
        }
    } if(side == DISPLAY_R || side == DISPLAY_BOTH) {
        switch(eye_cmd){
            case EYE_CMD_NEUTRAL:
                memcpy(&buffer->right_eye, EYE_R_NEUTRAL, sizeof(Eye));
                break;

            case EYE_CMD_BOOP:
                memcpy(&buffer->right_eye, EYE_R_BOOP, sizeof(Eye));
                break;

            default:
                break;
        }
    }
}

// Função auxiliar para traduzir um comando recebido em uma expressão para a boca
static void apply_mouth_cmd(StaticFace_t *buffer, mouth_cmd_t mouth_cmd, display_side_t side){
    if(side == DISPLAY_L || side == DISPLAY_BOTH){
        switch(mouth_cmd){
            case MOUTH_CMD_NEUTRAL:
                memcpy(&buffer->left_mouth, MOUTH_L_NEUTRAL, sizeof(Mouth));
                break;

            case MOUTH_CMD_BOOP:
                memcpy(&buffer->left_mouth, MOUTH_L_BOOP, sizeof(Mouth));
                break;

            default:
                break;
        }
    } if(side == DISPLAY_R || side == DISPLAY_BOTH) {
        switch(mouth_cmd){
            case MOUTH_CMD_NEUTRAL:
                memcpy(&buffer->right_mouth, MOUTH_R_NEUTRAL, sizeof(Mouth));
                break;

            case MOUTH_CMD_BOOP:
                memcpy(&buffer->right_mouth, MOUTH_R_BOOP, sizeof(Mouth));
                break;

            default:
                break;
        }
    }
}

// Função auxiliar para traduzir um comando recebido em uma expressão para o nariz
static void apply_nose_cmd(StaticFace_t *buffer, nose_cmd_t nose_cmd, display_side_t side){
    if(side == DISPLAY_L || side == DISPLAY_BOTH){
        switch(nose_cmd){
            case NOSE_CMD_NEUTRAL:
                memcpy(&buffer->left_nose, NOSE_L_NEUTRAL, sizeof(Nose));
                break;

            default:
                break;
        }
    } if(side == DISPLAY_R || side == DISPLAY_BOTH) {
        switch(nose_cmd){
            case NOSE_CMD_NEUTRAL:
                memcpy(&buffer->right_nose, NOSE_R_NEUTRAL, sizeof(Nose));
                break;

            default:
                break;
        }
    }
}

// Função de inicialização do display
void display_init(){
    spi_conf();

    // Cria a fila para receber comandos de outras tasks
    display_cmd_queue = xQueueCreate(5, sizeof(display_cmd_t));

    // Cria a task do display
    xTaskCreate(display_task, "display_task", 4096, NULL, 5, NULL);
}

// Envia comando para fila 
void display_send_expression(display_cmd_t *cmd) {
    xQueueSend(display_cmd_queue, cmd, 0);
}
