#ifndef __LED__
#define __LED__

#include <stdint.h>

// Initializes PB4:5 as output LEDs
void LED_Init(void);

// Turns on the led pin in led_mask
void LED_On(uint8_t led_mask);

// turns off the led pin in led_mask
void LED_Off(uint8_t led_mask);

#endif