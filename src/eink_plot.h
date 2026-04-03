#pragma once

#include "eink.h"
#include <stdint.h>
#include "EPD_1in54_V2.h"


#define EINK_MAX_PLOTWIDTH  EINK_WIDTH

#define EINK_PLOT_CO2 0


typedef struct tag_plot
{
    uint width;
    uint height;
    uint pos;
    float factor;
    float default_value; 
    float zero_value;   
    uint8_t data[EINK_MAX_PLOTWIDTH];

} PLOT;



void eink_plot_init();
void eink_plot_add(int index, float value);
void eink_plot_draw(int index);