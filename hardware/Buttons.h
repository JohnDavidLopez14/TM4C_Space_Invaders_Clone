#ifndef __Buttons__
#define __Buttons__
#include <stdint.h>

#define PE0 (1 << 0)
#define PE1 (1 << 1)

// Initializes pins PE0:1 as pull up resistor input switches
void Buttons_Init(void);

extern volatile uint8_t MissileFlag;
extern volatile uint8_t LaserFlag;

#endif