#ifndef TIMER_5
#define TIMER_5
#include "hardware/tm4c123gh6pm.h"
#include <stdint.h>

void Timer5_Init(void(*task)(void), unsigned long period);

#endif