#include "printstream.h"
#include <stdint.h>
#include "pico/stdlib.h"



void print_buffer(uint8_t* buf,size_t size,char streamtype)
{
	printf(STREAM_PREFIX);
	printf("%c",streamtype);
	for(size_t i=0;i<size;i++)
		printf("%02X",*(buf+i));
	printf(STREAM_SUFFIX);

}