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