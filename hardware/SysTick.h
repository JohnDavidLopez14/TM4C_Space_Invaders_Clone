#ifndef SYSTICK_H
#define SYSTICK_H

#include <stdint.h>
#include <stdbool.h>
#include "hardware/ADC.h"
#include "hardware/Nokia5110.h"
#include "hardware/tm4c123gh6pm.h"
#include "gameLogic/player.h"

extern volatile bool XposFlag;
extern volatile unsigned long Xpos;

void SysTick_Init(Player *);

#endif