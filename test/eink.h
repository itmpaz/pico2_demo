#include <stdint.h>

void eink_print(int x, int y, char* text);
void eink_init();
void eink_update();
void eink_clear();
void eink_wakeup();
void eink_sleep();