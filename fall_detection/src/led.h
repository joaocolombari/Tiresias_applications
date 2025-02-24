#ifndef LED_H
#define LED_H

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>

/* Declaracoes das funcoes */

int led_init(void);

void led_thread(void);

#endif