#ifndef __Buttons__
#define __Bottons__
#include <stdint.h>

// Initializes pins PE0:1 as pull up resistor input switches
void Buttons_Init(void);

// get the current state of the button, will return positive logic
unsigned int Button_Read(uint32_t button_mask);

#endif