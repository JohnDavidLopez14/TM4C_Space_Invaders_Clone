#include "DAC.h"
#include "tm4c123gh6pm.h"
#include <stdint.h>

#define PB0 (1 << 0)
#define PB1 (1 << 1)
#define PB2 (1 << 2)
#define PB3 (1 << 3)
#define PIN_MASK (PB0 | PB1 | PB2 | PB3)

void DAC_Init(){
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOB;
    while             ((SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R1) == 0);  // Poll until ready
    GPIO_PORTB_CR_R    |= PIN_MASK;                                 // allow GPIOAFSEL, GPIOPUR, and GPIODEN bits to be written
    GPIO_PORTB_AMSEL_R &= ~PIN_MASK;                                // clear analog functions
    GPIO_PORTB_PCTL_R  &= ~0x0000FFFF;
    GPIO_PORTB_DIR_R   |= PIN_MASK;                                 // pins as output
    GPIO_PORTB_AFSEL_R &= ~PIN_MASK;                                // clear alternate functions
    GPIO_PORTB_DEN_R   |= PIN_MASK;                                 // enable digital for all pins
    GPIO_PORTB_DATA_BITS_R[PIN_MASK] = 0;
}

void DAC_Out(unsigned char data){
    data &= 0x0F;
    GPIO_PORTB_DATA_BITS_R[PIN_MASK] = data; // using PIN_MASK since we want to clear the other output pins
}