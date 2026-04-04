#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "ws2812.pio.h"

#include "led.h"


static PIO _pio = pio0;


void put_pixel(uint32_t pixel_grb)
{
    pio_sm_put_blocking(_pio, 0, pixel_grb << 8u);
}

void led_init()
{
    
    int sm = 0;
    uint offset = pio_add_program(_pio, &ws2812_program);

    ws2812_program_init(_pio, sm, offset, 16, 800000, true);

}

void led_rgb(uint8_t red, uint8_t green, uint8_t blue)
{
    //uint32_t mask = (green << 16) | (red << 8) | (blue << 0);
    uint32_t mask = (red << 16) | (green << 8) | (blue << 0);
    put_pixel(mask);
}


void led_color(uint8_t c)
{
    switch (c)
    {
    case LED_RED:
        led_rgb(0xFF,0,0);
        break;
    case LED_GREEN:
        led_rgb(0,0xFF,0);
        break;
    case LED_BLUE:
        led_rgb(0,0,0xFF);
        break;
    default:
        led_rgb(0,0,0);
        break;
    }

}


void led_value_to_rgb(float curr, float min, float norm, float max,uint8_t level, uint8_t *r, uint8_t *g, uint8_t *b)
{
    /* Clamp curr to [min, max] */
    if (curr < min) curr = min;
    if (curr > max) curr = max;

    float ratio;

    if (curr <= norm) {
        /* Green → Yellow: min..norm */
        ratio = (norm > min) ? (curr - min) / (norm - min) : 0.0f;
        *r = (uint8_t)(level * ratio);   
        *g = level;                       
        *b = 0;
    } else {
        /* Yellow → Red: norm..max */
        ratio = (max > norm) ? (curr - norm) / (max - norm) : 1.0f;
        *r = level;                            
        *g = (uint8_t)(level * (1.0f - ratio)); 
        *b = 0;
    }
}

void led_co2(float value,int level)
{
    if (level<=0)
    {
        led_off();
        return;
    }
    if (level>255)
        level = 255;

    const float co2_min = 500;
    const float co2_norm = 1000;
    const float co2_max = 2000;

    uint8_t r,g,b;

    led_value_to_rgb(value, co2_min, co2_norm, co2_max,level, &r, &g, &b);

    led_rgb(r,g,b);

    printf("CO2 RGB %i,%i,%i\n",r,g,b);

}

void led_off()
{
    led_rgb(0,0,0);
}