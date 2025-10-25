#ifndef __Buttons__
#define __Buttons__
#include <stdint.h>
#include <stdbool.h>
#include "hardware/Timer2.h"

#define PE0 (1 << 0)
#define PE1 (1 << 1)
#define BUTTON_MASK (PE0 | PE1)

// Initializes pins PE0:1 as pull up resistor input switches
void Buttons_Init(void);
bool Buttons_Read(uint32_t);

extern volatile bool MissileFlag;
extern volatile bool LaserFlag;

#endif