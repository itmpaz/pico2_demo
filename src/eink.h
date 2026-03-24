#include <stdint.h>


#define EINK_WHITE 0xFF
#define EINK_BLACK 0x00

#define EINK_FNT_BIG 0
#define EINK_FNT_MID 1
#define EINK_FNT_SML 2


uint16_t eink_print(int x, int y, char* text,int fontid);
void eink_init();
void eink_update();
void eink_clear();
void eink_wakeup();
void eink_sleep();

void eink_drawplot();
void eink_addplot(int value);