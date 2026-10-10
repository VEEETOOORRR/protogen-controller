#ifndef IMU_H
#define IMU_H

#include <stdio.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c_master.h"
#include "esp_log.h"
#include "control.h"

#define SDA_PIN 8
#define SCL_PIN 9

#define LSM6DS3_ADDR 0x6B

void imu_init();


#endif