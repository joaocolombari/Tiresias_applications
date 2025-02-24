#include "led.h"

/* Declaracao do semaforo */
extern struct k_sem fall_sem;

#define LED_NODE DT_ALIAS(led0)
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED_NODE, gpios);

int led_init(void) {

    if (!device_is_ready(led.port)) {
        return 0;
    }

    gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE);

    return 0;
}

void led_thread(void) {

    while (1) {
        k_sem_take(&fall_sem, K_FOREVER);  // Wait until a fall is detected
        gpio_pin_set_dt(&led, 1);
        k_sleep(K_SECONDS(5));  // Keep LED on for 5 seconds
        gpio_pin_set_dt(&led, 0);
    }
}