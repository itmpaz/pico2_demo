#include "eink_plot.h"
#include "eink.h"


int a=0;

PLOT _plots[EINK_PLOT_MAX] = {   
    {EINK_WIDTH, EINK_HEIGHT,0,1.0F,350.0F,2000.0F,400.0F, 1000.0F,60, {0} }, // co2
    {EINK_WIDTH, EINK_HEIGHT,0,1.0F, 15.0F,  70.0F, 50.0F,   50.0F,60, {0} }, // hum
    {EINK_WIDTH, EINK_HEIGHT,0,1.0F, 10.0F,  40.0F, 20.0F,   20.0F,60, {0} }, // temp                  
    {EINK_WIDTH, EINK_HEIGHT,0,1.0F,980.0F,1035.0F,1000.0F,1015.0F,60, {0} }, // preassure                  
    };





void eink_plot_init()
{
    for(int i=0;i<EINK_PLOT_MAX;i++) 
	{   PLOT* p = &(_plots[i]);
        p->pos=0;
        p->factor =  (p->max_value - p->min_value) / p->ymax;
        for(int n=0;n<p->width;n++)
            eink_plot_add(i,p->default_value);
        
    }
}


void eink_plot_add(int index, float value)
{
    PLOT* p = &(_plots[index]);
	p->data[p->pos++] = (uint8_t)( (value -p->min_value) / p->factor);
	if (p->pos==p->width)
		p->pos=0;
}




void eink_plot_draw(int index)
{

    PLOT* p = &(_plots[index]);
	uint pos=p->pos;
	uint16_t NORMPPM = 1000; //ppm
	int lasty = -1; 

	int y0 = p->height - (int)((p->axis_value-p->min_value)/p->factor);

	for(int x=p->width;x>=0;x--)
	{
		if (pos==0)
			pos=p->width;
		pos--;
		
		int y = p->height - p->data[pos];// - (int)(p->min_value/p->factor);

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


		


		if (x % 3==0)
			eink_dot(x,y0,EINK_BLACK);
		
		if (x % 25 == 0)
		{	for(int k=0;k<p->ymax;k++)
				if (k%3==0)
					eink_dot(x,p->height-k,EINK_BLACK);
		}

	}

	const int mic_font_size = 7;
	char text[20];

	sprintf(text,"%i", (int)p->max_value);
	eink_print(2,p->height-p->ymax,text,EINK_FNT_MIC);
	
	sprintf(text,"%i", (int)p->axis_value);
	eink_print(2,y0-mic_font_size,text,EINK_FNT_MIC);

	sprintf(text,"%i", (int)p->min_value);
	eink_print(2,p->height-mic_font_size,text,EINK_FNT_MIC);

	
}

