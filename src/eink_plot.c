#include "eink_plot.h"
#include "eink.h"


int a=0;

PLOT _plots[] = {   {EINK_WIDTH, EINK_HEIGHT,0,1.0F,350.0F,2000.0F,400.0F,1000.0F,60, {0} },
                    {EINK_WIDTH, EINK_HEIGHT,0,1.0F,400.0F,2000.0F,400.0F,1000.0F,60, {0} },
                    {EINK_WIDTH, EINK_HEIGHT,0,1.0F,400.0F,2000.0F,400.0F,1000.0F,60, {0} },                    
                    {EINK_WIDTH, EINK_HEIGHT,0,1.0F,400.0F,2000.0F,400.0F,1000.0F,60, {0} },                    
                    };

uint _plotscount = sizeof(_plots) / sizeof(PLOT);    



void eink_plot_init()
{
    for(int i=0;i<_plotscount;i++) 
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


		int y0 = p->height - (int)((p->axis_value-p->min_value)/p->factor);


		if (x % 3==0)
			eink_dot(x,y0,EINK_BLACK);
		
		if (x % 25 == 0)
		{	for(int k=0;k<p->ymax;k++)
				if (k%3==0)
					eink_dot(x,p->height-k,EINK_BLACK);
		}

	}

	
}

