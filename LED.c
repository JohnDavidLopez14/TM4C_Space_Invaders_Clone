#include "LED.h"
#include "tm4c123gh6pm.h"
#include <stdint.h>

#define PB4 (1 << 4)
#define PB5 (1 << 5)
#define PIN_MASK (PB4 | PB5)

void LED_Init(void){
        SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOB;
    while             ((SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R1) == 0);  // Poll until ready
    GPIO_PORTB_CR_R    |= PIN_MASK;                                 // allow GPIOAFSEL, GPIOPUR, and GPIODEN bits to be written
    GPIO_PORTB_AMSEL_R &= ~PIN_MASK;                                // clear analog functions
    GPIO_PORTB_PCTL_R  &= ~0x00FF0000;                                // clear alternate 
    GPIO_PORTB_DIR_R   |= PIN_MASK;                                 // pins as output
    GPIO_PORTB_AFSEL_R &= ~PIN_MASK;                                // clear alternate functions
    GPIO_PORTB_DEN_R   |= PIN_MASK;                                 // enable digital for all pins

    // clear output pins
    GPIO_PORTB_DATA_BITS_R[PIN_MASK] = 0;
}

void LED_On(uint8_t led_mask){
    led_mask &= PIN_MASK;  // mask PB4:5
    if (led_mask){
        GPIO_PORTB_DATA_BITS_R[led_mask] = led_mask;
    }
}

void LED_Off(uint8_t led_mask){
    led_mask &= PIN_MASK;
    if (led_mask){
        GPIO_PORTB_DATA_BITS_R[led_mask] = 0; // can also use ~led_mask
    }
}