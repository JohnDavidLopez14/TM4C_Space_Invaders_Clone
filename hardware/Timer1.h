#ifndef TIMER_1
#define TIMER_1
#include "hardware/tm4c123gh6pm.h"
#include <stdint.h>

void Timer1_Init(void(*task)(void), unsigned long period);

#endif