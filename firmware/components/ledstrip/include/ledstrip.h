#ifndef LEDSTRIP_H
#define LEDSTRIP_H

#include <stdint.h>
#include "led_strip.h"

#define PIN_LEDSTRIP_R 4
#define PIN_LEDSTRIP_L 5

#define QNT_LEDS 10

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} Rgb_led;

typedef enum rgb_side {
    RGB_STRIP_L,
    RGB_STRIP_R,
    RGB_STRIP_BOTH
} Rgb_side;

void ledstrip_init();

void ledstrip_change_color(Rgb_side side, uint8_t start_led, uint8_t stop_led, Rgb_led rgb);

#endif