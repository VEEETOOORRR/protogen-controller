#include "ledstrip.h"

#define FPS 60
#define TICK_RATE_MS (1000 / FPS)


// Handles mantidos no escopo do arquivo para acesso pelas funções do módulo
static led_strip_handle_t led_strip_R = NULL;
static led_strip_handle_t led_strip_L = NULL;

QueueHandle_t ledstrip_cmd_queue; 

void ledstrip_init(void) {

    ledstrip_cmd_queue = xQueueCreate(5, sizeof(ledstrip_cmd_t));

    /// LED strip common configuration
    led_strip_config_t strip_config_R = {
        .strip_gpio_num = PIN_LEDSTRIP_R,
        .max_leds = QNT_LEDS,
        .led_model = LED_MODEL_WS2812,
        .color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRB,
        .flags = {
            .invert_out = false,
        }
    };

    led_strip_config_t strip_config_L = {
        .strip_gpio_num = PIN_LEDSTRIP_L,
        .max_leds = QNT_LEDS,
        .led_model = LED_MODEL_WS2812,
        .color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRB,
        .flags = {
            .invert_out = false,
        }
    };

    /// RMT backend configuration
    led_strip_rmt_config_t rmt_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = 10 * 1000 * 1000, // 10MHz
        .mem_block_symbols = 64,
        .flags = {
            .with_dma = false,
        }
    };

    /// Create the LED strip objects
    led_strip_new_rmt_device(&strip_config_R, &rmt_config, &led_strip_R);
    led_strip_new_rmt_device(&strip_config_L, &rmt_config, &led_strip_L);

    // Limpa os LEDs na inicialização
    led_strip_clear(led_strip_R);
    led_strip_clear(led_strip_L);
}

void ledstrip_change_color(ledstrip_target_t side, uint8_t start_led, uint8_t stop_led, ledstrip_rgb rgb) {
    // Garante que o índice final não ultrapasse a quantidade total de LEDs
    if (stop_led > QNT_LEDS) {
        stop_led = QNT_LEDS;
    }

    if (side == LEDSTRIP_R || side == LEDSTRIP_BOTH) {
        if (led_strip_R != NULL) {
            for (uint8_t i = start_led; i < stop_led; i++) {
                led_strip_set_pixel(led_strip_R, i, rgb.r, rgb.g, rgb.b);
            }
            led_strip_refresh(led_strip_R);
        }
    }

    if (side == LEDSTRIP_L || side == LEDSTRIP_BOTH) {
        if (led_strip_L != NULL) {
            for (uint8_t i = start_led; i < stop_led; i++) {
                led_strip_set_pixel(led_strip_L, i, rgb.r, rgb.g, rgb.b);
            }
            led_strip_refresh(led_strip_L);
        }
    }
}


void ledstrip_task(void *arg){
    ledstrip_rgb start_rgb_buffer_r[QNT_LEDS], start_rgb_buffer_l[QNT_LEDS];
    ledstrip_rgb current_rgb_buffer_r[QNT_LEDS], current_rgb_buffer_l[QNT_LEDS];
    ledstrip_rgb target_rgb_buffer_r[QNT_LEDS], target_rgb_buffer_l[QNT_LEDS];
    
    for(;;){

    }

}