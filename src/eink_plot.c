#include "eink_plot.h"
#include "eink.h"
#include <stdlib.h>


int a=0;

PLOT _plots[EINK_PLOT_MAX] = {   
    {EINK_WIDTH, EINK_HEIGHT, 0, 350.0F, 2000.0F,  400.0F, 1000.0F,  10.0F, 60, false, {0} }, // co2
    {EINK_WIDTH, EINK_HEIGHT, 0,  15.0F,   70.0F,   50.0F,   50.0F,   5.0F, 60, true,  {0} }, // hum
    {EINK_WIDTH, EINK_HEIGHT, 0,  10.0F,   40.0F,   20.0F,   20.0F,   1.0F, 60, true,  {0} }, // temp                  
    {EINK_WIDTH, EINK_HEIGHT, 0, 980.0F, 1035.0F, 1000.0F, 1015.0F,   1.0F, 60, true,  {0} }, // preassure                  
    };





void eink_plot_init()
{
    for(int i=0;i<EINK_PLOT_MAX;i++) 
	{   PLOT* p = &(_plots[i]);
        p->pos=0;
        for(int n=0;n<p->width;n++)
            eink_plot_add(i,p->default_value);
        
    }
}


void eink_plot_add(int index, float value)
{
    PLOT* p = &(_plots[index]);
	p->data[p->pos++] = value;//(uint8_t)( (value -p->min_value) / p->factor);
	if (p->pos==p->width)
		p->pos=0;
}

void eink_plot_get_range(PLOT* p, float* min, float* max)
{
	*min = p->data[0];
	*max = p->data[0];
	for(int i=1;i<EINK_MAX_PLOTWIDTH;i++)
	{	if (p->data[i]<*min)
			*min = p->data[i];
		if (p->data[i]>*max)
			*max = p->data[i];			
	}

}



void eink_plot_draw(int index)
{

    PLOT* p = &(_plots[index]);
	uint pos=p->pos;
	uint16_t NORMPPM = 1000; //ppm
	int lasty = -1; 
	const int margin_persent = 5;

	float min_value;
	float max_value;

	if (p->isautorange)
	{	eink_plot_get_range(p, &min_value, &max_value);
		if (max_value-min_value<p->margin_value)
		{	min_value-=p->margin_value/2;
			max_value+=p->margin_value/2;
		} else
		{	float f = (max_value - min_value) / p->ymax;
			min_value-=f;
			max_value+=f;
		}
	} else
	{
		min_value = p->min_value;
		max_value = p->max_value;
	}
    float factor =  (max_value - min_value) / p->ymax;
	uint height = p->height-1;
	bool isshowxaxis = (p->axis_value<=max_value) && (p->axis_value>=min_value);
	int y0 = height - (int)((p->axis_value-min_value)/factor);

	for(int x=p->width-1;x>=0;x--)
	{
		if (pos==0)
			pos=p->width;
		pos--;
		
		int y = height - (int)((p->data[pos]-min_value)/factor);

		if (isshowxaxis)
			if (x % 3==0)
				eink_dot(x,y0,EINK_BLACK);
		
		if (x % 25 == 0)
		{	for(int k=0;k<p->ymax;k++)
				if (k%3==0)
					eink_dot(x,height-k,EINK_BLACK);
		}

		if (lasty != -1)
		{
			int yd0 = lasty>y ? y : lasty;	
			int yd1 = lasty>y ? lasty : y;
			for(int yy = yd0; yy<=yd1;yy++)
			{	eink_dot(x,yy,EINK_BLACK);
			}

		} else
		{	eink_dot(x,y,EINK_BLACK);
		}

		lasty = y;

	}

	const int mic_font_size = 7;
	char text[20];

	int y_top_text = height-p->ymax;
	int y_btm_text = height-mic_font_size;
	sprintf(text,"%i", (int)max_value);
	eink_print(2,y_top_text,text,EINK_FNT_MIC);
	sprintf(text,"%i", (int)min_value);
	eink_print(2,y_btm_text,text,EINK_FNT_MIC);
	
	if (isshowxaxis)
	{	int y_axs_text = y0-mic_font_size;
		if ( (abs(y_top_text-y_axs_text)>mic_font_size) && (abs(y_btm_text-y_axs_text)>mic_font_size))
		{	sprintf(text,"%i", (int)p->axis_value);
			eink_print(2,y0-mic_font_size,text,EINK_FNT_MIC);
		}
	}



	
}

