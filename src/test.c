#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "mhz19b.h"
#include "eink.h"
#include "bme280_i2c.h"


#define PLOT_UPDATE_PERIOD_SEC 144 //8 hours on the screen

void wait_and_update_plot(uint ms)
{
    
    static uint64_t t0 = 0;
    uint sleep_sum = 0;
    uint sleep_step = 1000;
    while(sleep_sum<ms)
    {
        uint64_t t = time_us_64();
        uint64_t dt = (t-t0)/1000;
        if (dt>PLOT_UPDATE_PERIOD_SEC *1000)
        {   eink_addplot(mhz19b_data()->co2_value);
            t0 = t;
        }
        sleep_ms(sleep_step); 
        sleep_sum+=sleep_step;
    }

}


#define SHOW_TIME_OK 10000
#define SHOW_TIME_ERROR 60000

int main()
{

    stdio_init_all();
    printf("Init\n");

    mhz19b_init();
    bool z19_ok = mhz19b_test();

    eink_init();
    int bme_chipid = bme280_init();
    printf("BME Chip id: 0x%02X\n",bme_chipid);

    uint show_time = SHOW_TIME_OK;

    //eink_print(0,20,APP_TITLE,EINK_FNT_BIG);
    eink_print(0,0,"Air station",EINK_FNT_MID);
    eink_print(0,25,"Pico 1 zero",EINK_FNT_MID);
    eink_print(32,175,"APR 2026",EINK_FNT_MID);
    if (bme_chipid==BME280_CHIPID)
    {   eink_print(0,75,"BME280..........OK",EINK_FNT_SML);
    } else
    {   eink_print(0,75,"BME280.......ERROR",EINK_FNT_SML);
        show_time = SHOW_TIME_ERROR;
    }
    if (z19_ok)
    {   eink_print(0,100,"MH-Z19B.........OK",EINK_FNT_SML);
    } else
    {   eink_print(0,100,"MH-Z19B......ERROR",EINK_FNT_SML);
        show_time = SHOW_TIME_ERROR;
    }


    eink_update();

    
    eink_sleep();
    sleep_ms(show_time);
    
    

    printf("Start\n");
    uint c=0;
    char text[50];
    float pres;
    float temp;
    float hum;

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

        bme280_read(&pres,&temp,&hum);
        sprintf(text,"%.1f hPa",pres);
        eink_print(0,0,text,EINK_FNT_MID);
        sprintf(text,"%.1f C",temp);
        eink_print(0,25,text,EINK_FNT_MID);
        sprintf(text,"%.1f %%RH",hum);
        eink_print(0,50,text,EINK_FNT_MID);
        printf("P %.3f hPa, T %.2f C, H %.2f %%RH\n",pres,temp,hum);


        c = mhz19b_data()->co2_counter;
        sprintf(text,"%d ",mhz19b_data()->co2_value);
        uint pos = eink_print(0,85,text,EINK_FNT_BIG);    
        eink_print(pos,98,"ppm",EINK_FNT_MID);    

        printf("CO2 %d ppm.  %d,%d\n",mhz19b_data()->co2_value,mhz19b_data()->co2_counter,mhz19b_data()->cs_error_counter);

        //sprintf(text,"%d,%d",mhz19b_data()->co2_counter,mhz19b_data()->cs_error_counter);
        //eink_print(20,100,text);

        eink_drawplot();

        eink_update();
        eink_sleep();
        
        printf("Waiting...\n");
        wait_and_update_plot(60000);
        printf("Wakeup\n");
    }



    //while (1)
     //   tight_loop_contents();


}
