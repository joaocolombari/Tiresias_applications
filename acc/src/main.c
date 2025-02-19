/*
        Tiresias acc application 
        Joao Victor Colombari Carlet
*/

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <stdio.h>

#define sleep_time_1000us 1000

int main(void)
{
        const struct device *const dev = DEVICE_DT_GET_ONE(bosch_bmi270);

        device_init(dev); // Isso aqui se usa com o zephyr,deferred-init; do overlay. Serve para deixar o device esperando eu ligar ele na trhead
        
        //struct sensor_value acc[3];

        if (!device_is_ready(dev)) {
                printf("Device %s is not ready\n", dev->name);
                return 0;
        }

        printf("Device %p name is %s\n", dev, dev->name);

        return 0;
}
