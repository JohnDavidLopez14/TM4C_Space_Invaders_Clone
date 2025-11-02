#ifndef TIMER_4
#define TIMER_4
#include "hardware/tm4c123gh6pm.h"
#include <stdint.h>

void Timer4_Init(void(*task)(void), unsigned long period);
void Timer4_Enable(void);
void Timer4_Disable(void);

#endif