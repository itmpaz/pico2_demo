
#include "eink.h"
#include "EPD_1in54_V2.h"
#include "fonts.h"


#define EINK_IMGSIZE (((EPD_1IN54_V2_WIDTH+7) / 8 ) * EPD_1IN54_V2_HEIGHT) 

extern sFONT Font24;

uint8_t eink_img[EINK_IMGSIZE]={0};


void eink_print_char(uint8_t* image, int x, int y, char ch)
{

    uint x_byte = x / 8;
    uint fnt_w_byte = (Font24.Width+7) / 8;
    uint img_w_byte = (EPD_1IN54_V2_WIDTH+7) / 8;
    uint fnt_ch_size = fnt_w_byte*Font24.Height;
    uint ch_index = ch - ' ';
    for(uint i=0;i<fnt_w_byte;i++)
        for(uint j=0;j<Font24.Height;j++)
        {
            uint img_n = (y+j)*img_w_byte+x_byte+i;
            uint fnt_n = ch_index*fnt_ch_size + j*fnt_w_byte + i ;
            image[ img_n ]  = ~Font24.table[ fnt_n  ];
            //printf("%i,%i img[%i] <- fnt[%i]\n",i,j,img_n,fnt_n);
        }


}

void eink_print(int x, int y, char* text)
{
    uint step = ((Font24.Width+7) / 8) * 8; 
    uint pos=0;
    while( text[pos]!=0)
    {
        eink_print_char(eink_img, x+pos*step, y, text[pos]);
        pos++;
    }
}


void eink_clear()
{
    memset(eink_img,0xFF,EINK_IMGSIZE);
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

    EPD_1IN54_V2_Display(eink_img);
    DEV_Delay_ms(100);


}