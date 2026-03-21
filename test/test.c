#include <stdio.h>
#include "pico/stdlib.h"


int bmp280_main(); 
int bus_scan_main();

int main()
{
    stdio_init_all();

   // bus_scan_main() ;
    bmp280_main(); 
/*

   gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);    
 
    bool led_on = true;
    while (true) {
        gpio_put(PICO_DEFAULT_LED_PIN, led_on);
        printf("Hello, world!!!\n");
        sleep_ms(1000);
        led_on = !led_on;
    }

    */
}
