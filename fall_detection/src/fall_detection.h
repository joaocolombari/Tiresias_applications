#ifndef FALL_DETECTION_H
#define FALL_DETECTION_H

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <stdio.h>
#include <math.h>

#define FREE_FALL_THRESHOLD 5.0         // G-force threshold for free-fall detection
#define IMPACT_THRESHOLD 25.0           // G-force threshold for impact detection
#define INACTIVITY_TIME_MS 3000         // Time of inactivity after impact
#define SAMPLING_PERIOD_MS 10           // 100Hz sampling (10ms period)

/* Declaracoes das funcoes */

double compute_total_acceleration(struct sensor_value *acc);

int sensor_init(void);                 

void detect_fall_thread(void);

#endif