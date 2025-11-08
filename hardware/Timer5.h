#ifndef TIMER_5
#define TIMER_5
#include "hardware/tm4c123gh6pm.h"
#include <stdint.h>
#include <stdbool.h>

void Timer5_Init(void(*task)(void));
void Timer5_Oneshot(unsigned long);

#endif