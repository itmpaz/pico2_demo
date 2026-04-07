#pragma once

#include "eink.h"
#include <stdint.h>
#include "EPD_1in54_V2.h"


#define EINK_MAX_PLOTWIDTH  EINK_WIDTH

#define EINK_PLOT_CO2 0
#define EINK_PLOT_HUM 1
#define EINK_PLOT_TMP 2
#define EINK_PLOT_PRS 3
#define EINK_PLOT_MAX 4


typedef struct tag_plot
{
    uint width;
    uint height;
    uint pos;
    float min_value;
    float max_value;
    float default_value; 
    float axis_value;   
    float margin_value;   
    uint ymax;
    bool isautorange;
    float data[EINK_MAX_PLOTWIDTH];

} PLOT;



void eink_plot_init();
void eink_plot_add(int index, float value);
void eink_plot_draw(int index);