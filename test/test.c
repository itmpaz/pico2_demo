#include <stdio.h>
#include "pico/stdlib.h"


int main()
{
    stdio_init_all();

   gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);    
 
    bool led_on = true;
    while (true) {
        gpio_put(PICO_DEFAULT_LED_PIN, led_on);
        printf("Hello, world!!!\n");
        sleep_ms(1000);
        led_on = !led_on;
    }
}
