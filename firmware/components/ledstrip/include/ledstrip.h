#ifndef LEDSTRIP_H
#define LEDSTRIP_H

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "led_strip.h"

#define PIN_LEDSTRIP_R 4
#define PIN_LEDSTRIP_L 5

#define QNT_LEDS 13

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} ledstrip_rgb;

typedef enum {
    LEDSTRIP_L,
    LEDSTRIP_R,
    LEDSTRIP_BOTH
} ledstrip_target_t;

typedef struct {
    ledstrip_rgb rgb;
    uint8_t start_led;
    uint8_t stop_led;
    ledstrip_target_t target;
    uint32_t transition_time_ms;
} ledstrip_cmd_t;

void ledstrip_init();

void ledstrip_change_color(ledstrip_target_t side, uint8_t start_led, uint8_t stop_led, ledstrip_rgb rgb);

#endif