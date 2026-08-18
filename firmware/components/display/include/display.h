#ifndef DISPLAY_H
#define DISPLAY_H

#include "driver/spi_master.h"
#include "string.h"
#include <stdio.h>

#include "expressions.h"

#define PIN_NUM_MISO   13   // O MAX7219 não envia dados de volta, pino não usado
#define PIN_NUM_MOSI   11   // Conecta no DIN da primeira matriz
#define PIN_NUM_CLK    12   // Conecta no CLK das matrizes
#define PIN_NUM_CS     10   // Conecta no CS/LOAD das matrizes

#define NUM_MODULES    14    // Quantidade de matrizes 8x8 encadeadas

// Registradores do MAX7219
#define MAX7219_REG_NOOP         0x00
#define MAX7219_REG_DIGIT0       0x01 // Linhas/Colunas de 0 a 7
#define MAX7219_REG_DECODE_MODE  0x09
#define MAX7219_REG_INTENSITY    0x0A
#define MAX7219_REG_SCAN_LIMIT   0x0B
#define MAX7219_REG_SHUTDOWN     0x0C
#define MAX7219_REG_DISPLAY_TEST 0x0F

#define DISPLAY_INTENSITY        0x01 // Intensidade default (Vai de 0x01 a 0x0F)

void spi_conf();

void display_write_all(uint8_t reg, uint8_t data);

void display_set_pixel(int x, int y, uint8_t state);

void display_render();

#endif