idf_component_register(
    SRCS "microphone.c"
    INCLUDE_DIRS "include"
    REQUIRES esp_adc driver esp_common control freertos
)