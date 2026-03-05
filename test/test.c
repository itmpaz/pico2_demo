#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "mhz19b.h"



int main()
{
    stdio_init_all();
    

    
    mhz19b_init();

    

    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);    

    gpio_put(PICO_DEFAULT_LED_PIN, true);

    uint c=0;
    while(1)
    {
        mhz19b_read();
        if (c!=mhz19b_data()->co2_counter)
        {
            c = mhz19b_data()->co2_counter;
            printf("CO2 %d ppm\n",mhz19b_data()->co2_value);

        } 
        sleep_ms(1000);
    }



    //while (1)
     //   tight_loop_contents();


}
