#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "mhz19b.h"
#include "eink.h"



#define PLOT_PERIOD_SEC 144 //8 hours on the screen

void wait(uint ms)
{
    
    uint64_t t0 = 0;
    if (t0==0)
        t0 = time_us_64();

    uint sum = 0;
    while(sum<ms)
    {
        uint64_t dt = (time_us_64()-t0)/1000;
        if (dt>PLOT_PERIOD_SEC *1000)
        {   eink_addplot(mhz19b_data()->co2_value);
            t0 = time_us_64();
        }
        sleep_ms(1000); 

    }

}


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
        sleep_ms(100);
        if (c==mhz19b_data()->co2_counter)
        {   printf("skip %i, %i\n",mhz19b_data()->co2_counter,mhz19b_data()->cs_error_counter);
            mhz19b_protocolreset();
            sleep_ms(1000);
            continue;
        }

        eink_wakeup();
        eink_clear();

        c = mhz19b_data()->co2_counter;
        sprintf(text,"%d",mhz19b_data()->co2_value);
        eink_print(20,0,"CO2");    
        eink_print(20,25,text);    
        eink_print(20,50,"ppm");   

        sprintf(text,"CO2 %d ppm\n",mhz19b_data()->co2_value);

        sprintf(text,"%d,%d",mhz19b_data()->co2_counter,mhz19b_data()->cs_error_counter);
        eink_print(20,100,text);

        eink_drawplot();

        eink_update();
        eink_sleep();

        wait(60000);
    }



    //while (1)
     //   tight_loop_contents();


}
