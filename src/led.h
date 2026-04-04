#pragma once
#include <stdint.h>

#define LED_OFF   0
#define LED_RED   1
#define LED_GREEN 2
#define LED_BLUE  3


void led_init();
void led_rgb(uint8_t red, uint8_t green, uint8_t blue);
void led_color(uint8_t c);