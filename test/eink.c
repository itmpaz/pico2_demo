
#include "eink.h"
#include "EPD_1in54_V2.h"
#include "fonts.h"
#include "Debug.h"


#define EINK_BYTESPERROW ((EPD_1IN54_V2_WIDTH+7) / 8)
#define EINK_IMGSIZE (EINK_BYTESPERROW * EPD_1IN54_V2_HEIGHT) 


#define EINK_WHITE 0xFF
#define EINK_BLACK 0x00

#define EINK_PLOTWIDTH  EPD_1IN54_V2_WIDTH

extern sFONT Font24;

uint8_t eink_img[EINK_IMGSIZE]={0};

uint8_t eink_plot[EINK_PLOTWIDTH]={};

uint eink_plotpos=0;

#define EINK_NORMPPM 400

#define EINK_PLOT_FACTOR 50

void eink_addplot(int value)
{
	eink_plot[eink_plotpos]=value/EINK_PLOT_FACTOR;
	eink_plotpos++;
	if (eink_plotpos==EINK_PLOTWIDTH)
		eink_plotpos=0;
}





void eink_dot(int x, int y, uint8_t color)
{
	if ((x>=EPD_1IN54_V2_WIDTH)||(x<0)||(y>=EPD_1IN54_V2_HEIGHT)||(y<0))
		return;
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
		
		int y = EPD_1IN54_V2_HEIGHT - eink_plot[pos];

		if (lasty != -1)
		{
			

			int yd0 = lasty>y ? y : lasty;	
			int yd1 = lasty>y ? lasty : y;
			printf("%i,%i(%i)[%i]{%i} - ",x,y,eink_plot[pos],pos,eink_plot[20]);
			for(int yy = yd0; yy<=yd1;yy++)
			{	eink_dot(x,yy,EINK_BLACK);
				printf("%i,",yy);
			}
			printf("\n");

		} else
		{	eink_dot(x,y,EINK_BLACK);
		}

		lasty = y;


		int y0 = EPD_1IN54_V2_HEIGHT - NORMPPM/EINK_PLOT_FACTOR;

		if (x % 3==0)
			eink_dot(x,y0,EINK_BLACK);
		
		if (x % 25 == 0)
		{	for(int k=0;k<60;k++)
				if (k%3==0)
					eink_dot(x,EPD_1IN54_V2_HEIGHT-k,EINK_BLACK);
		}

	}
	printf("plot done\n");
	
}


void eink_print_char(uint8_t* image, int x, int y, char ch)
{

    uint x_byte = x / 8;
    uint fnt_w_byte = (Font24.Width+7) / 8;
    uint fnt_ch_size = fnt_w_byte*Font24.Height;
    uint ch_index = ch - ' ';
    uint8_t one=1;
    for(uint j=0;j<Font24.Height;j++)
 		for(uint i=0;i<fnt_w_byte;i++)
			for(uint k=0;k<8;k++)
			{
				uint fnt_n = ch_index*fnt_ch_size + j*fnt_w_byte + i ;
				uint8_t fnt_bit = Font24.table[ fnt_n  ] & (one << (7-k));
				if (fnt_bit!=0)
					eink_dot(x+i*8+k, y+j, EINK_BLACK);
        	}

}

void eink_print(int x, int y, char* text)
{
    uint step = Font24.Width; 
    uint pos=0;
    while( text[pos]!=0)
    {
        eink_print_char(eink_img, x+pos*step, y, text[pos]);
        pos++;
    }
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