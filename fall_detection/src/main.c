//  //  //  //  //  //  //  //  //  //  //  //
//  Tiresias Hearing Device Project         //
//  University of Sao Paulo                 //
//  Author: Joao Victor Colombari Carlet    //
//  //  //  //  //  //  //  //  //  //  //  //

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <stdio.h>
#include <math.h>

#include "led/led.h"
#include "fall_detection/fall_detection.h"
#include "BLE/ble_service.h"

/* 
    Semaphore for signaling fall detection:

    Statically define and initialize a semaphore;

    The semaphore can be accessed outside the module where it is defined using:

    extern struct k_sem <name>; 

*/

K_SEM_DEFINE(fall_sem, 0, 1);

/* Main Function */
int main(void) {

    ble_service_init();
    led_init();
    sensor_init();

    printf("Fall Detection System Initialized!\n");

    return 0;
}

/* Define threads */
K_THREAD_DEFINE(ble_tid, 10240, ble_service_thread, NULL, NULL, NULL, 3, 0, 0);
K_THREAD_DEFINE(led_tid, 1024, led_thread, NULL, NULL, NULL, 2, 0, 0);
K_THREAD_DEFINE(fall_tid, 1024, detect_fall_thread, NULL, NULL, NULL, 1, 0, 0);

/* In Zephyr, main thread is priority zero by default, so no need to define it! */