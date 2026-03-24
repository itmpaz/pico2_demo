#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/irq.h"
#include "mhz19b.h"


#define UART_ID uart1
#define BAUD_RATE 9600
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY    UART_PARITY_NONE
#define UART_TX_PIN 8
#define UART_RX_PIN 9

#define MHZ19B_CO2    0x86 // Read CO2 concentration
#define MHZ19B_ZERO   0x87 // Calibrate Zero Point (ZERO)
#define MHZ19B_SPAN   0x88 // Calibrate Span Point (SPAN)
#define MHZ19B_ONOFF  0x79 // ON/OFF Auto Calibration
#define MHZ19B_RANGE  0x99 // Detection range setting



static int UART_IRQ = UART_ID == uart0 ? UART0_IRQ : UART1_IRQ;
static uint8_t Z19B_READ[] = {0xFF,0x01,0x86,0x00,0x00,0x00,0x00,0x00,0x79};
static MHZ19B _data={0};
static uint8_t _packet[9]={0};
static uint _packetpos = 0;



void process_packet(uint8_t*);
uint8_t mhz19b_cs(uint8_t *packet);
void on_uart_rx();


MHZ19B* mhz19b_data()
{
    return    &_data; 
}

void mhz19b_init()
{
    uart_init(UART_ID, 2400);

    gpio_set_function(UART_TX_PIN, UART_FUNCSEL_NUM(UART_ID, UART_TX_PIN));
    gpio_set_function(UART_RX_PIN, UART_FUNCSEL_NUM(UART_ID, UART_RX_PIN));
    uart_set_baudrate(UART_ID, BAUD_RATE);
    uart_set_hw_flow(UART_ID, false, false);
    uart_set_format(UART_ID, DATA_BITS, STOP_BITS, PARITY);
    uart_set_fifo_enabled(UART_ID, false);
    irq_set_exclusive_handler(UART_IRQ, on_uart_rx);
    irq_set_enabled(UART_IRQ, true);
    uart_set_irq_enables(UART_ID, true, false);


}


void mhz19b_read()
{
    uart_write_blocking(UART_ID,Z19B_READ,sizeof(Z19B_READ));
}

void mhz19b_protocolreset()
{
    memset(_packet,0,sizeof(_packet));
    _packetpos=0;
}


void on_uart_rx() 
{
    while (uart_is_readable(UART_ID)) 
    {
        _packet[_packetpos++] = uart_getc(UART_ID);
        if (_packetpos==9)
        {
            process_packet(&_packet[0]);
            mhz19b_protocolreset();
        }
    }
}

uint8_t mhz19b_cs(uint8_t *packet)
{
    uint8_t i, checksum;
    for( i = 1; i < 8; i++)
        checksum += packet[i];
    checksum = 0xff - checksum;
    checksum += 1;
    return checksum;
}




void process_packet(uint8_t* packet)
{
    uint8_t cs = mhz19b_cs(packet);
    if (cs!=packet[8])
    {   _data.cs_error_counter++;
        return;
    } 

    if (packet[1]==MHZ19B_CO2)
    {
        _data.co2_value = packet[2]*256+packet[3];
        _data.co2_counter++;
        return;
    }

    _data.cmd_error_counter++;

}