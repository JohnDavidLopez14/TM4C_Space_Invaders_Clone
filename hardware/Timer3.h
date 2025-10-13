#ifndef TIMER_3
#define TIMER_3
#include "hardware/tm4c123gh6pm.h"
#include <stdint.h>

void Timer3_Init(void(*task)(void), unsigned long period);

#endif