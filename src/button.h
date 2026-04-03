#include <stdio.h>
#include "pico/stdlib.h"


#define BUTTON_PIN 7
#define BUTTON_DEBOUNCE_TIME_MS       20
#define BUTTON_DBLCLICK_MAXTIME_MS   350

void button_init();
void button_update(); 
bool button_sleep(uint32_t delay); 
void button_clickreset(); 
uint32_t button_get_singleclick_counter();
uint32_t button_get_doubleclick_counter();

