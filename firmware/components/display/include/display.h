#ifndef DISPLAY_H
#define DISPLAY_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/spi_master.h"
#include "string.h"
#include <stdio.h>

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
    EYE_CMD_NO_CHANGE = 0,
    EYE_CMD_NEUTRAL,
    EYE_CMD_HAPPY,
    EYE_CMD_SAD,
    EYE_CMD_BOOP,
    EYE_CMD_FULL,
    EYE_CMD_CLEAR
} eye_cmd_t;

typedef enum {
    MOUTH_CMD_NO_CHANGE = 0,
    MOUTH_CMD_NEUTRAL,
    MOUTH_CMD_TALK_HALF,
    MOUTH_CMD_TALK_FULL,
    MOUTH_CMD_BOOP,
    MOUTH_CMD_FULL,
    MOUTH_CMD_CLEAR
} mouth_cmd_t;

typedef enum {
    NOSE_CMD_NO_CHANGE = 0,
    NOSE_CMD_NEUTRAL,
    NOSE_CMD_FULL,
    NOSE_CMD_CLEAR
} nose_cmd_t;

typedef struct {
    eye_cmd_t   left_eye;
    eye_cmd_t   right_eye;
    nose_cmd_t  left_nose;
    nose_cmd_t  right_nose;
    mouth_cmd_t left_mouth;
    mouth_cmd_t right_mouth;
} display_cmd_t;

// Função de inicialização do display
void display_init();

// Envia comando para fila
void display_send_expression(display_cmd_t *cmd);

#endif