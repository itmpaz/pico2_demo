
#include "eink.h"
#include "EPD_1in54_V2.h"
#include "fonts.h"
#include "Debug.h"


#define EINK_WIDTH  EPD_1IN54_V2_WIDTH
#define EINK_HEIGHT EPD_1IN54_V2_HEIGHT

#define EINK_BYTESPERROW ((EINK_WIDTH+7) / 8)
#define EINK_IMGSIZE (EINK_BYTESPERROW * EINK_HEIGHT) 


#define EINK_WHITE 0xFF
#define EINK_BLACK 0x00

#define EINK_PLOTWIDTH  EINK_WIDTH


extern sFONT Font24;
extern sFONT Font50;
extern sFONT Font16;

uint8_t eink_img[EINK_IMGSIZE]={0};

uint8_t eink_plot[EINK_PLOTWIDTH]={};

uint eink_plotpos=0;

#define EINK_NORMPPM 400

#define EINK_PLOT_FACTOR 50


#define EINK_ROTATE 270

#if defined(EINK_ROTATE) && EINK_ROTATE!=90 && EINK_ROTATE!=180 && EINK_ROTATE!=270 && EINK_ROTATE!=0 
#error Wrong EINK_ROTATE macro value
#endif


void eink_rotate_screen(int* x, int* y)
{
#if	EINK_ROTATE==270
	int t = *x;
	*x = *y;
	*y = EINK_WIDTH - t;
#elif EINK_ROTATE==180
	*x = EINK_WIDTH - *x;
	*y = EINK_HEIGHT -*y;
#elif EINK_ROTATE==90
	int t = *x;
	*x = EINK_HEIGHT -*y;
	*y =  t;
#endif

}


sFONT* eink_getfont(int fontid)
{
	if (fontid == EINK_FNT_BIG)
		return &Font50;
	if (fontid == EINK_FNT_MID)
		return &Font24;
	return &Font16;

}


void eink_addplot(int value)
{
	eink_plot[eink_plotpos]=value/EINK_PLOT_FACTOR;
	//printf("Plot[%i] = %i\n",eink_plotpos,value);
	eink_plotpos++;
	if (eink_plotpos==EINK_PLOTWIDTH)
		eink_plotpos=0;

}





void eink_dot(int x, int y, uint8_t color)
{
	if ((x>=EINK_WIDTH)||(x<0)||(y>=EINK_HEIGHT)||(y<0))
		return;

	eink_rotate_screen(&x,&y);		

	uint x_byte = x / 8;
	uint x_pos = 7- (x % 8);
	uint8_t one = 1;
	uint img_n = y*EINK_BYTESPERROW+x_byte;
	if (color==EINK_BLACK)
	{	eink_img[ img_n ]  &= ~(one<<x_pos);
	} else
	{	eink_img[ img_n ]  |= (one<<x_pos);
	}
}

void eink_drawplot()
{
	uint pos=eink_plotpos;
	uint16_t NORMPPM = 1000; //ppm
	int lasty = -1; 

	for(int x=EINK_PLOTWIDTH-1;x>=0;x--)
	{
		if (pos==0)
			pos=EINK_PLOTWIDTH;
		pos--;
		
		int y = EINK_HEIGHT - eink_plot[pos];

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


		int y0 = EINK_HEIGHT - NORMPPM/EINK_PLOT_FACTOR;

		if (x % 3==0)
			eink_dot(x,y0,EINK_BLACK);
		
		if (x % 25 == 0)
		{	for(int k=0;k<60;k++)
				if (k%3==0)
					eink_dot(x,EINK_HEIGHT-k,EINK_BLACK);
		}

	}

	
}


void eink_print_char(uint8_t* image, int x, int y, char ch,int fontid)
{
	sFONT* font = eink_getfont(fontid);
    uint x_byte = x / 8;
    uint fnt_w_byte = (font->Width+7) / 8;
    uint fnt_ch_size = fnt_w_byte*font->Height;
    uint ch_index = ch - ' ';
    uint8_t one=1;
    for(uint j=0;j<font->Height;j++)
 		for(uint i=0;i<fnt_w_byte;i++)
			for(uint k=0;k<8;k++)
			{
				uint fnt_n = ch_index*fnt_ch_size + j*fnt_w_byte + i ;
				uint8_t fnt_bit = font->table[ fnt_n  ] & (one << (7-k));
				if (fnt_bit!=0)
					eink_dot(x+i*8+k, y+j, EINK_BLACK);
        	}

}

uint16_t eink_print(int x, int y, char* text,int fontid)
{

	sFONT* font = eink_getfont(fontid);
    uint16_t step = font->Width; 
    uint16_t pos=0;
    while( text[pos]!=0)
    {
        eink_print_char(eink_img, x+pos*step, y, text[pos],fontid);
        pos++;
    }
	return x+pos*step;
}


void eink_clear()
{
    memset(eink_img,0xFF,sizeof(eink_img));
}

void eink_sleep()
{
    EPD_1IN54_V2_Sleep();
    DEV_Delay_ms(100);

}


void eink_init()
{

    DEV_Module_Init();
    EPD_1IN54_V2_Init();
    DEV_Delay_ms(100);

    eink_clear();

    EPD_1IN54_V2_Clear();
    DEV_Delay_ms(100);
	
	
	
	eink_plotpos=0;
	for(int n=0;n<EINK_PLOTWIDTH;n++)
		eink_addplot(EINK_NORMPPM);

}

void eink_wakeup()
{
    EPD_1IN54_V2_Init();
    DEV_Delay_ms(100);
}


void eink_update()
{

    EPD_1IN54_V2_Display(eink_img);
    DEV_Delay_ms(100);


}