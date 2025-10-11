#ifndef __Buttons__
#define __Buttons__
#include <stdint.h>

// Initializes pins PE0:1 as pull up resistor input switches
void Buttons_Init(void);

extern volatile uint8_t MissileFlag;
extern volatile uint8_t LaserFlag;

#endif