/*
        Tiresias Fall Detection Application 
        Joao Victor Colombari Carlet
*/

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

#define LED_NODE DT_ALIAS(led0)
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED_NODE, gpios);

const struct device *dev;

/* Function to calculate total acceleration in m/s2 */
double compute_total_acceleration(struct sensor_value *acc) {
    float ax = acc[0].val1 + acc[0].val2 / 1000000.0;
    float ay = acc[1].val1 + acc[1].val2 / 1000000.0;
    float az = acc[2].val1 + acc[2].val2 / 1000000.0;
    return (double)sqrt(ax * ax + ay * ay + az * az);
}

/* Fall Detection Logic */
void detect_fall(void) {

    // Aqui vou dar o primeiro fetch e esperar ele acentar
    sensor_sample_fetch(dev);
    k_sleep(K_MSEC(1000));

    struct sensor_value acc[3];
    static bool in_free_fall = false;
    static bool caiu_morto = false;
    static int64_t fall_start_time = 0;

    while (1) {
        k_sleep(K_MSEC(SAMPLING_PERIOD_MS)); // 10MS for 100Hz
        sensor_sample_fetch(dev);
        sensor_channel_get(dev, SENSOR_CHAN_ACCEL_XYZ, acc);
        
        double total_acc = compute_total_acceleration(acc);
        printf("Acceleration: %.2f m/s2\n", total_acc);

        /* Step 1: Detect Free-Fall */
        if (total_acc < FREE_FALL_THRESHOLD) {
            in_free_fall = true;
            fall_start_time = k_uptime_get();
            printf("[WARNING] Free fall detected!\n");
        }

        /* Step 2: Detect Impact */
        if (in_free_fall && total_acc > IMPACT_THRESHOLD) {
            printf("[ALERT] Impact detected!\n");
            in_free_fall = false;

            /* Step 3: Detect Post-Fall Inactivity */
            int64_t start_time = k_uptime_get();
            while (k_uptime_get() - start_time < INACTIVITY_TIME_MS) {
                k_sleep(K_MSEC(1000)); // espera bastante pra debounce
                sensor_sample_fetch(dev);
                sensor_channel_get(dev, SENSOR_CHAN_ACCEL_XYZ, acc);
                total_acc = compute_total_acceleration(acc);
                if (total_acc > 12) {  // Small movement cancels fall alert
                    printf("[INFO] Movement detected after fall. False alarm.\n");
                    break;
                }
                caiu_morto = true;
            }

            // printf("caiu morto is %d \n", caiu_morto);

            if (caiu_morto) {
                /* Final Decision: Fall Confirmed */
                gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
                printf("[CRITICAL] FUDEU O VELHO CAIU LIGA PRO SAMU...\n");
                // TODO: Trigger BLE alert, buzzer, or emergency response
                return;
            }
        }
    }
}

/* Main Function */
int main(void) {
    dev = DEVICE_DT_GET_ONE(bosch_bmi270);
    if (!device_is_ready(dev)) {
        printf("Device %s is not ready\n", dev->name);
        return 0;
    }

    if (!device_is_ready(led.port)) {
        return -1;
    }

    printf("Device %p name is %s\n", dev, dev->name);

    /* Sensor Configuration */
    struct sensor_value full_scale, sampling_freq, oversampling;
        
    /*
        Esse sensor_value e a definicao da estrutura sensor_value e do tipo de valor 
        que as variaveis das instancias dessa estrutura podem assumnir, que no caso
        sao int de 32 bits..
    */

    /*
        O full scale sao os valores de fullscale referidos no proprio datasheet, que
        podem ser 2, 4, 8 e 16 G. A API usa uma funcao chamada channel_accel_convert
        para converter para SI.
    */

    full_scale.val1 = 2;            /* G */
    full_scale.val2 = 0;
    sampling_freq.val1 = 100;       /* Hz. Performance mode */
    sampling_freq.val2 = 0;
    oversampling.val1 = 1;          /* Normal mode */
    oversampling.val2 = 0;

    sensor_attr_set(dev, SENSOR_CHAN_ACCEL_XYZ, SENSOR_ATTR_FULL_SCALE, &full_scale);
    sensor_attr_set(dev, SENSOR_CHAN_ACCEL_XYZ, SENSOR_ATTR_OVERSAMPLING, &oversampling);
    sensor_attr_set(dev, SENSOR_CHAN_ACCEL_XYZ, SENSOR_ATTR_SAMPLING_FREQUENCY, &sampling_freq);

    printf("Fall Detection System Initialized!\n");
    
    /* Start Fall Detection */
    detect_fall();

    return 0;
}
