#pragma once

#include <stdint.h>
#include <stdio.h>

#define STREAM_PREFIX  "<!--"   
#define STREAM_SUFFIX  "-->"
#define STREAM_TYPE_DATA    'D'
#define STREAM_TYPE_FILL    'F'


void print_buffer(uint8_t* buf,size_t size,char streamtype);