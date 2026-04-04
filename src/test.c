#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "mhz19b.h"
#include "eink.h"
#include "eink_plot.h"
#include "bme280_i2c.h"
#include "button.h"
#include "led.h"

typedef struct 
{
    float pres;
    float temp;
    float hum;
    float co2;

} SENSORS;



#define PLOT_UPDATE_PERIOD_SEC 144 //8 hours on the screen





void wait_and_update_plot(uint ms,SENSORS* s)
{
    
    static uint64_t t0 = 0;
    uint sleep_sum = 0;
    uint sleep_step = 1000;
    while(sleep_sum<ms)
    {
        uint64_t t = time_us_64();
        uint64_t dt = (t-t0)/1000;
        if (dt>PLOT_UPDATE_PERIOD_SEC *1000)
        {   eink_plot_add(EINK_PLOT_CO2,s->co2);
            eink_plot_add(EINK_PLOT_HUM,s->hum);
            eink_plot_add(EINK_PLOT_TMP,s->temp);
            eink_plot_add(EINK_PLOT_PRS,s->pres);
            t0 = t;
        }
        if (button_sleep(sleep_step))
            break;
        sleep_sum+=sleep_step;
    }

}

void print_sensors(SENSORS* s,int plotid)
{
    char text[50];
    int y = 0;
    const int smallstep = 25;
    const int bigstep = 55;

    if (plotid!=EINK_PLOT_PRS)
    {   sprintf(text,"%.1f hPa",s->pres);
        eink_print(0,y,text,EINK_FNT_MID);
        y+=smallstep;
    } else
    {   sprintf(text,"%d ",(uint)s->pres);
        uint pos = eink_print(0,y,text,EINK_FNT_BIG);    
        eink_print(pos,y+13,"hPa",EINK_FNT_MID); 
        y+=bigstep;
    }
        
    if (plotid!=EINK_PLOT_TMP)
    {   sprintf(text,"%.1f 'C",s->temp);
        eink_print(0,y,text,EINK_FNT_MID);
        y+=smallstep;
    } else
    {   sprintf(text,"%.1f 'C",s->temp);
        eink_print(0,y,text,EINK_FNT_BIG);
        y+=bigstep;
    }

    if (plotid!=EINK_PLOT_HUM)
    {   sprintf(text,"%.1f %%",s->hum);
        eink_print(0,y,text,EINK_FNT_MID);
         y+=smallstep;
    } else
    {   sprintf(text,"%.1f %%",s->hum);
        eink_print(0,y,text,EINK_FNT_BIG);
         y+=bigstep;
    }

    if (plotid!=EINK_PLOT_CO2)
    {   sprintf(text,"%d ppm",(uint)s->co2);
        eink_print(0,y,text,EINK_FNT_MID);
        y+=smallstep;
    } else
    {   sprintf(text,"%d ",(uint)s->co2);
        uint pos = eink_print(0,y,text,EINK_FNT_BIG);    
        eink_print(pos,y+13,"ppm",EINK_FNT_MID);    
        y+=bigstep;
        
    }



}




#define SHOW_TIME_OK 10000
#define SHOW_TIME_ERROR 60000
#define MEASUREMENT_DELAY 60000
#define SHOW_TITLE_SCREEN
//#define MEASUREMENT_DELAY 10000



int main()
{

    stdio_init_all();
    printf("Init\n");

    led_init();
    led_color(LED_BLUE);

    button_init();  

    mhz19b_init();
    bool z19_ok = mhz19b_test();

    eink_plot_init();
    eink_init();
    int bme_chipid = bme280_init();
    bool bme_ok = (bme_chipid==BME280_CHIPID);
    if (!bme_ok)
        printf("BME Chip id: 0x%02X\n",bme_chipid);

    uint show_time = SHOW_TIME_OK;

    led_color( ((bme_ok) && (z19_ok)) ? LED_GREEN : LED_RED);

#ifdef SHOW_TITLE_SCREEN   

    eink_print(0,0,"Air station",EINK_FNT_MID);
    eink_print(0,25,"Pico 1 zero",EINK_FNT_MID);
    eink_print(32,175,"APR 2026",EINK_FNT_MID);
    if (bme_ok)
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
    
#endif    

    led_color(LED_OFF);

    printf("Start\n");
    uint c=0;
    SENSORS s={0};
    int plotid = EINK_PLOT_CO2;
    int co2led_mode = 0;
    const int co2led_mode_max = 4;
    const int co2led_mode_step = 256 / co2led_mode_max;

    while(1)
    {

        
        mhz19b_read();
        button_sleep(100);
        if (c==mhz19b_data()->co2_counter)
        {   printf("skip %i, %i\n",mhz19b_data()->co2_counter,mhz19b_data()->cs_error_counter);
            mhz19b_protocolreset();
        } else
        {   s.co2 = mhz19b_data()->co2_value; 
            c = mhz19b_data()->co2_counter;
            printf("CO2 %d ppm.  %d,%d\n",mhz19b_data()->co2_value,mhz19b_data()->co2_counter,mhz19b_data()->cs_error_counter);
        }


        bme280_read(&s.pres,&s.temp,&s.hum);
        printf("P %.3f hPa, T %.2f C, H %.2f %%RH\n",s.pres,s.temp,s.hum);


        int n = button_get_doubleclick_counter();
        int m = button_get_singleclick_counter();
        while(m>0)
        {   plotid++;
            if (plotid>=EINK_PLOT_MAX)
                plotid = 0;
            m--;
            printf("Plot #%i\n",plotid);
        }
        while (n>0)
        {
            co2led_mode++;
            if  (co2led_mode>co2led_mode_max)
                co2led_mode = 0;
            n--;
            printf("CO2 led %i\n",co2led_mode);
        }
        button_clickreset();  


        led_co2(s.co2,co2led_mode*co2led_mode_step);


#ifndef SHOW_TITLE_SCREEN         

        s.co2 = 555.555;
        s.hum = 33.333;
        s.temp = 22.222;
        s.pres = 1000.1;

#endif

        eink_wakeup();
        eink_clear();

        print_sensors(&s,plotid);

        eink_plot_draw(plotid);

        eink_update();
        eink_sleep();
        
        printf("Waiting...\n");
        wait_and_update_plot(MEASUREMENT_DELAY,&s);
        printf("Wakeup\n");
    }






}
