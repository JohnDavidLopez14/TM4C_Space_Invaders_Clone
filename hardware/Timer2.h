#ifndef TIMER_2
#define TIMER_2
#include "hardware/tm4c123gh6pm.h"
#include <stdint.h>

void Timer2_Init(void(*task)(void), unsigned long period);

#endif