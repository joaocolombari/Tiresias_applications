//  //  //  //  //  //  //  //  //  //  //  //
//  Tiresias Hearing Device Project         //
//  University of Sao Paulo                 //
//  Author: Joao Victor Colombari Carlet    //
//  //  //  //  //  //  //  //  //  //  //  //

#ifndef LED_H
#define LED_H

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>

/* Declaracao do semaforo */
extern struct k_sem fall_sem;

/* Declaracoes das funcoes */

int led_init(void);

void led_thread(void);

#endif