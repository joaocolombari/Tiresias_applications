//  //  //  //  //  //  //  //  //  //  //  //
//  Tiresias Hearing Device Project         //
//  University of Sao Paulo                 //
//  Author: Joao Victor Colombari Carlet    //
//  //  //  //  //  //  //  //  //  //  //  //

#include "fall_detection.h"
#include "../led/led.h"

const struct device *dev;

/* Function to calculate total acceleration in m/s2 */
double compute_total_acceleration(struct sensor_value *acc) {
    float ax = acc[0].val1 + acc[0].val2 / 1000000.0;
    float ay = acc[1].val1 + acc[1].val2 / 1000000.0;
    float az = acc[2].val1 + acc[2].val2 / 1000000.0;
    return (double)sqrt(ax * ax + ay * ay + az * az);
}

int sensor_init(void) {
    dev = DEVICE_DT_GET_ONE(bosch_bmi270);
    if (!device_is_ready(dev)) {
        printf("Device %s is not ready\n", dev->name);
        return 0;
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
     
     return 0;
}

/* Fall Detection Logic */
void detect_fall_thread(void) {

    // Aqui vou dar o primeiro fetch e esperar ele acentar
    sensor_sample_fetch(dev);
    k_sleep(K_MSEC(1000));

    struct sensor_value acc[3];
    static bool in_free_fall = false;
    static bool felt = false;
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
                felt = true;
            }

            if (felt) {
                /* Final Decision: Fall Confirmed */
                printf("[CRITICAL] Fall Detected!!\n");

                k_sem_give(&fall_sem);  // Signal LED thread to turn on LED

                felt = false;  // Reset flag for next cycle
            }
        }
    }
}