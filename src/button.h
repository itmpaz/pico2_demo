#include <stdio.h>
#include "pico/stdlib.h"


#define BUTTON_PIN 7
#define BUTTON_DEBOUNCE_TIME_MS       20
#define BUTTON_DBLCLICK_MAXTIME_MS   350

void button_init();
void button_update(); 
void button_sleep(uint32_t delay); 
int button_get_singleclick_counter();
int button_get_doubleclick_counter();

