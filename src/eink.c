
#include "eink.h"
#include "EPD_1in54_V2.h"
#include "fonts.h"
#include "Debug.h"
#include "printstream.h"






extern sFONT Font24;
extern sFONT Font50;
extern sFONT Font16;
extern sFONT Font05;

uint8_t eink_img[EINK_IMGSIZE]={0};








#define EINK_ROTATE 270

#if defined(EINK_ROTATE) && EINK_ROTATE!=90 && EINK_ROTATE!=180 && EINK_ROTATE!=270 && EINK_ROTATE!=0 
#error Wrong EINK_ROTATE macro value
#endif


void eink_rotate_screen(int* x, int* y)
{
#if	EINK_ROTATE==270
	int t = *x;
	*x = *y;
	*y = (EINK_WIDTH-1) - t;
#elif EINK_ROTATE==180
	*x = (EINK_WIDTH -1) - *x;
	*y = (EINK_HEIGHT-1) -*y;
#elif EINK_ROTATE==90
	int t = *x;
	*x = (EINK_HEIGHT-1) -*y;
	*y =  t;
#endif

}


sFONT* eink_getfont(int fontid)
{
	if (fontid == EINK_FNT_BIG)
		return &Font50;
	if (fontid == EINK_FNT_MID)
		return &Font24;
	if (fontid == EINK_FNT_MIC)
		return &Font05;

	return &Font16;

}




void eink_dot(int x, int y, uint8_t color)
{
	eink_rotate_screen(&x,&y);	
	
	if ((x>=EINK_WIDTH)||(x<0)||(y>=EINK_HEIGHT)||(y<0))
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



int eink_print_char_v(uint8_t* image, int x, int y, char ch,sFONT* font)
{
	assert(font->Height<=8);

    uint fnt_ch_size = font->Width;
    uint ch_index = ch - ' ';
    uint8_t one=1;
    for(uint j=0;j<font->Height;j++)
	{
 		for(uint i=0;i<font->Width;i++)
		{
			uint fnt_n = ch_index*fnt_ch_size + i;
			uint8_t fnt_bit = font->table[ fnt_n  ] & (one << (j));
			if (fnt_bit!=0)
				eink_dot(x+i, y+j, EINK_BLACK);
		}


	}
	
	return font->Width+1;

}


int eink_print_char_h(uint8_t* image, int x, int y, char ch,sFONT* font)
{
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
	return font->Width;
}

int eink_print_char(uint8_t* image, int x, int y, char ch,int fontid)
{
	sFONT* font = eink_getfont(fontid);
	if (font->Pack == FONT_PACK_H)
	{	return eink_print_char_h(image,  x, y, ch,font);
	} else
	{	return eink_print_char_v(image,  x, y, ch,font);
	}
}


uint16_t eink_print(int x, int y, char* text,int fontid)
{

	sFONT* font = eink_getfont(fontid);
    uint16_t step = font->Width; 
    uint16_t pos=0;
    while( text[pos]!=0)
    {
        uint16_t step = eink_print_char(eink_img, x, y, text[pos],fontid);
		x+=step;
        pos++;
    }
	return x;
}


void eink_clear()
{
	uint8_t fill = 0xFF;
	print_buffer(&fill,1,STREAM_TYPE_FILL);
    memset(eink_img,fill,sizeof(eink_img));
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
	
	


}

void eink_wakeup()
{
    EPD_1IN54_V2_Init();
    DEV_Delay_ms(100);
}





void eink_update()
{
	print_buffer(eink_img,EINK_IMGSIZE,STREAM_TYPE_DATA);

    EPD_1IN54_V2_Display(eink_img);
    DEV_Delay_ms(100);


}