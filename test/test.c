#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "mhz19b.h"
#include "eink.h"


int main()
{
    stdio_init_all();
    mhz19b_init();
    eink_init();
    
    sleep_ms(1000);
    eink_sleep();
    sleep_ms(1000);
    
    


    uint c=0;
    char text[50];
    while(1)
    {
        mhz19b_read();
        if (c==mhz19b_data()->co2_counter)
        {   printf("skip %i, %i\n",mhz19b_data()->co2_counter,mhz19b_data()->cs_error_counter);
            sleep_ms(1000);
            mhz19b_protocolreset();
            continue;
        }

        eink_wakeup();
        eink_clear();

        c = mhz19b_data()->co2_counter;
        sprintf(text,"%d",mhz19b_data()->co2_value);
        eink_print(20,50-25,"CO2");    
        eink_print(20,50,text);    
        eink_print(20,50+25,"ppm");   

        sprintf(text,"CO2 %d ppm\n",mhz19b_data()->co2_value);

        sprintf(text,"%d",mhz19b_data()->co2_counter);
        eink_print(20,120,text);
        sprintf(text,"%d",mhz19b_data()->cs_error_counter);
        eink_print(20,120+25,text);


        eink_update();
        eink_sleep();

        sleep_ms(60000);
    }



    //while (1)
     //   tight_loop_contents();


}
