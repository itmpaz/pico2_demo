#pragma once

#include <stdint.h>


#define EINK_WHITE 0xFF
#define EINK_BLACK 0x00

#define EINK_FNT_BIG 0
#define EINK_FNT_MID 1
#define EINK_FNT_SML 2

#define EINK_WIDTH  EPD_1IN54_V2_WIDTH
#define EINK_HEIGHT EPD_1IN54_V2_HEIGHT

#define EINK_BYTESPERROW ((EINK_WIDTH+7) / 8)
#define EINK_IMGSIZE (EINK_BYTESPERROW * EINK_HEIGHT) 


#define EINK_WHITE 0xFF
#define EINK_BLACK 0x00


uint16_t eink_print(int x, int y, char* text,int fontid);
void eink_init();
void eink_update();
void eink_clear();
void eink_wakeup();
void eink_sleep();

void eink_dot(int x, int y, uint8_t color);