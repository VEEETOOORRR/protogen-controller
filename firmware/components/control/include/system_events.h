#ifndef SYSTEM_EVENTS_H
#define SYSTEM_EVENTS_H

typedef enum {
    EVENT_BOOP_TOUCHED,
    EVENT_BOOP_RELEASED,
    EVENT_VOICE_TALK_HALF,
    EVENT_VOICE_TALK_FULL,
    EVENT_VOICE_SILENCE,
    EVENT_IMU_TILT_RIGHT,
    EVENT_IMU_TILT_LEFT,
    EVENT_IMU_CENTERED,
    EVENT_APP_CHANGE_COLOR
} event_type_t;

typedef struct {
    event_type_t type;     // Qual evento aconteceu?
    int32_t payload;       // Opcional: Valor extra
} system_event_t;

#endif