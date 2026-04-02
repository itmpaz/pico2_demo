/*****************************************************************************
* | File      	:   DEV_Config.c
* | Author      :   Waveshare team
* | Function    :   Hardware underlying interface
* | Info        :
*----------------
* |	This version:   V3.0
* | Date        :   2019-07-31
* | Info        :   
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of theex Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
******************************************************************************/
#include "DEV_Config.h"

#undef PICO_DEFAULT_SPI
#undef PICO_DEFAULT_SPI_SCK_PIN
#undef PICO_DEFAULT_SPI_TX_PIN
#undef PICO_DEFAULT_SPI_RX_PIN
#undef PICO_DEFAULT_SPI_CSN_PIN


#define PICO_DEFAULT_SPI 1
#define PICO_DEFAULT_SPI_SCK_PIN 10
#define PICO_DEFAULT_SPI_TX_PIN 11
#define PICO_DEFAULT_SPI_RX_PIN 12
#define PICO_DEFAULT_SPI_CSN_PIN 13


/*
static inline void cs_select() {
    asm volatile("nop \n nop \n nop");
    gpio_put(PICO_DEFAULT_SPI_CSN_PIN, 0);  // Active low
    asm volatile("nop \n nop \n nop");
}

static inline void cs_deselect() {
    asm volatile("nop \n nop \n nop");
    gpio_put(PICO_DEFAULT_SPI_CSN_PIN, 1);
    asm volatile("nop \n nop \n nop");
}

*/

/**
 * GPIO
**/
int EPD_RST_PIN;
int EPD_DC_PIN;
int EPD_CS_PIN;
int EPD_BUSY_PIN;
int EPD_PWR_PIN;
int EPD_MOSI_PIN;
int EPD_MISO_PIN;
int EPD_SCLK_PIN;
spi_inst_t* pico_spi;

/**
 * GPIO read and write
**/
void DEV_Digital_Write(UWORD Pin, UBYTE Value)
{
    //asm volatile("nop \n nop \n nop");
	gpio_put(Pin, Value);
    //asm volatile("nop \n nop \n nop");
}

UBYTE DEV_Digital_Read(UWORD Pin)
{
	UBYTE Read_value = 0;
	Read_value = gpio_get(Pin);
	return Read_value;
}

/**
 * SPI
**/


void DEV_SPI_Write_nByte(uint8_t *pData, uint32_t Len)
{
   // cs_select();
    spi_write_blocking(pico_spi, pData, Len);
    //cs_deselect();

}

void DEV_SPI_WriteByte(uint8_t Value)
{
	DEV_SPI_Write_nByte(&Value, 1);
    
}

/**
 * delay x ms
**/
void DEV_Delay_ms(UDOUBLE xms)
{
	sleep_ms(xms);
}









void DEV_GPIO_Init(void)
{

	EPD_RST_PIN     = 15;
	EPD_DC_PIN      = 14;
	EPD_CS_PIN      = PICO_DEFAULT_SPI_CSN_PIN;
    EPD_PWR_PIN     = 17;// not used
	EPD_BUSY_PIN    = 6;
    EPD_MOSI_PIN    = PICO_DEFAULT_SPI_TX_PIN;
	EPD_MISO_PIN    = PICO_DEFAULT_SPI_RX_PIN;
	EPD_SCLK_PIN    = PICO_DEFAULT_SPI_SCK_PIN ;

	pico_spi = spi1;
	

    spi_init(pico_spi, 500 * 1000);
    gpio_set_function(PICO_DEFAULT_SPI_RX_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_TX_PIN, GPIO_FUNC_SPI);
    // Make the SPI pins available to picotool
    bi_decl(bi_3pins_with_func(PICO_DEFAULT_SPI_RX_PIN, PICO_DEFAULT_SPI_TX_PIN, PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI));

    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_init(PICO_DEFAULT_SPI_CSN_PIN);
    gpio_set_dir(PICO_DEFAULT_SPI_CSN_PIN, GPIO_OUT);
    gpio_put(PICO_DEFAULT_SPI_CSN_PIN, 1);
    // Make the CS pin available to picotool
    bi_decl(bi_1pin_with_name(PICO_DEFAULT_SPI_CSN_PIN, "SPI CS"));

   	gpio_init(EPD_PWR_PIN);
	gpio_init(EPD_DC_PIN);
	gpio_init(EPD_RST_PIN );
	gpio_init(EPD_BUSY_PIN );
    gpio_set_dir(EPD_RST_PIN, GPIO_OUT); 
	gpio_set_dir(EPD_DC_PIN, GPIO_OUT); 
	gpio_set_dir(EPD_PWR_PIN, GPIO_OUT); 
	gpio_set_dir(EPD_BUSY_PIN, GPIO_IN); 
	
	DEV_Digital_Write(EPD_CS_PIN, 1);
    DEV_Digital_Write(EPD_PWR_PIN, 1);
    
}



/******************************************************************************
function:	Module Initialize, the library and initialize the pins, SPI protocol
parameter:
Info:
******************************************************************************/
UBYTE DEV_Module_Init(void)
{

	DEV_GPIO_Init();

	
}

/******************************************************************************
function:	Module exits, closes SPI and BCM2835 library
parameter:
Info:
******************************************************************************/
void DEV_Module_Exit(void)
{

}
