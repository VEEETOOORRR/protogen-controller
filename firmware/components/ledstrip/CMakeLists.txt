idf_component_register(
    SRCS "ledstrip.c"
    INCLUDE_DIRS "include"
    REQUIRES led_strip driver esp_common
)