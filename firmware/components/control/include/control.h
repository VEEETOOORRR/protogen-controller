#ifndef CONTROL_H
#define CONTROL_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "system_events.h"
#include "display.h"
#include "expressions.h"
#include "ledstrip.h"

void control_init();
void control_send_event(system_event_t *event);

#endif