#ifndef __Buttons__
#define __Buttons__
#include <stdint.h>
#include <stdbool.h>
#include "hardware/Timer2.h"

#define PE0 (1 << 0)
#define PE1 (1 << 1)

// Initializes pins PE0:1 as pull up resistor input switches
void Buttons_Init(void);

extern volatile bool MissileFlag;
extern volatile bool LaserFlag;

#endif