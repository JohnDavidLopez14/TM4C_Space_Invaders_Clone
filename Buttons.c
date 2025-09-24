#include "Buttons.h"
#include "tm4c123gh6pm.h"
#include <stdint.h>

#define PE0 (1 << 0)
#define PE1 (1 << 1)
#define PIN_MASK (PE0 | PE1)

// Initialize PE0:1 as negative logic input pins
void Buttons_Init(void){
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOE;
    while ((SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R4) == 0);
    GPIO_PORTE_CR_R    |= PIN_MASK;   // allow GPIOAFSEL, GPIOPUR, and GPIODEN bits to be written
    GPIO_PORTE_AMSEL_R &= ~PIN_MASK;  // clear analog functions
    GPIO_PORTE_PCTL_R  &= ~0xFF;      // mux the port to clear alternate function selection
    GPIO_PORTE_DIR_R   &= ~PIN_MASK;  // pin as input
    GPIO_PORTE_AFSEL_R &= ~PIN_MASK;  // clear alternate functions
    GPIO_PORTE_PUR_R   |= PIN_MASK;   // enable pull up resistors
    GPIO_PORTE_DEN_R   |= PIN_MASK;   // enable digital
}

// returns a pitmask of the values read
uint32_t Button_Read(uint32_t button_mask){
    button_mask &= PIN_MASK;
    if (button_mask){
        return ~GPIO_PORTE_DATA_BITS_R[button_mask] & button_mask; // index the pins we want, invert, then mask again to isolate the values that have been flipped
        }
    return 0;
}