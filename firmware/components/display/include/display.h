#ifndef DISPLAY_H
#define DISPLAY_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/spi_master.h"
#include "string.h"
#include <stdio.h>

#include "expressions.h"

// Configurações SPI
#define PIN_NUM_MISO  13      // O MAX7219 não envia dados de volta, pino não usado
#define PIN_NUM_MOSI  10      // Conecta no DIN da primeira matriz
#define PIN_NUM_CS    11      // Conecta no CS/LOAD das matrizes
#define PIN_NUM_CLK   12      // Conecta no CLK das matrizes


#define SPI_CLK_SPEED 1000000 // Velocidade do CLK SPI; Default: 1 MHz

#define NUM_MODULES   14      // Quantidade de matrizes 8x8 encadeadas. Útil para funções que mexem com todas as matrizes.

// Registradores do MAX7219
#define MAX7219_REG_NOOP         0x00
#define MAX7219_REG_DIGIT0       0x01 // Registrador da primeira linha da matriz. Acessa-se as outras incrementando de 0x01 a 0x08
#define MAX7219_REG_DECODE_MODE  0x09
#define MAX7219_REG_INTENSITY    0x0A
#define MAX7219_REG_SCAN_LIMIT   0x0B
#define MAX7219_REG_SHUTDOWN     0x0C
#define MAX7219_REG_DISPLAY_TEST 0x0F

#define DISPLAY_INTENSITY        0x01 // Intensidade default (Vai de 0x01 a 0x0F)

typedef enum {
    DISPLAY_CMD_NEUTRAL,
    DISPLAY_CMD_BOOP
} display_cmd_t;

extern QueueHandle_t display_cmd_queue;

void display_init();

void display_init();

void spi_conf(); // Inicializa e configura os displays via SPI

void display_write_all(uint8_t reg, uint8_t data); // Manda o mesmo comando para cada uma das matrizes 8x8

void display_clean_all(); // Limpa todas as matrizes 8x8

void display_update(StaticFace_t *buffer); // Atualiza o display de acordo com o buffer global

void buffer_update_left_eye(StaticFace_t *buffer, const Eye e); // Atualiza no buffer as matrizes referentes ao olho esquerdo
void buffer_update_right_eye(StaticFace_t *buffer, const Eye e); // Atualiza no buffer as matrizes referentes ao olho direito
void buffer_update_left_mouth(StaticFace_t *buffer, const Mouth m); // Atualiza no buffer as matrizes referentes à boca esquerda
void buffer_update_right_mouth(StaticFace_t *buffer, const Mouth m); // Atualiza no buffer as matrizes referentes à boca direita
void buffer_update_left_nose(StaticFace_t *buffer, const Nose n); // Atualiza no buffer as matrizes referentes ao nariz esquerdo
void buffer_update_right_nose(StaticFace_t *buffer, const Nose n); // Atualiza no buffer as matrizes referentes ao nariz direito



#endif