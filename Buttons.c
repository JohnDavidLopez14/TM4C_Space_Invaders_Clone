#include "Buttons.h"
#include "tm4c123gh6pm.h"
#include <stdint.h>

#define PE0 (1 << 0)
#define PE1 (1 << 1)
#define PIN_MASK (PE0 | PE1)

volatile uint8_t MissileFlag = 0;
volatile uint8_t LaserFlag = 0;


// Initialize PE0:1 as negative logic input pin
void Buttons_Init(void){
    // GPIO initialization
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOE;
    while ((SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R4) == 0);
    GPIO_PORTE_CR_R    |= PIN_MASK;   // allow GPIOAFSEL, GPIOPUR, and GPIODEN bits to be written
    GPIO_PORTE_AMSEL_R &= ~PIN_MASK;  // clear analog functions
    GPIO_PORTE_PCTL_R  &= ~0xFF;      // mux the port to clear alternate function selection
    GPIO_PORTE_DIR_R   &= ~PIN_MASK;  // pin as input
    GPIO_PORTE_AFSEL_R &= ~PIN_MASK;  // clear alternate functions
    GPIO_PORTE_PUR_R   |= PIN_MASK;   // enable pull up resistors
    GPIO_PORTE_DEN_R   |= PIN_MASK;   // enable digital

    // Interrupts
    GPIO_PORTE_IS_R &= ~PIN_MASK; // 0 - the edge on the corresponding pin is detected
    GPIO_PORTE_IBE_R &= ~PIN_MASK; // 0 - the interrupt generation is controlled by the GPIO Interrupt Event (GPIOIEV) register
    GPIO_PORTE_IEV_R &= ~PIN_MASK; // 0 - the falliwng edge or a low level on the corresponding pin triggers an interrupt
    GPIO_PORTE_ICR_R |= PIN_MASK; // 1 - the corresponding interrupt is cleared
    GPIO_PORTE_IM_R |= PIN_MASK; // 1 - The interrupt from the corresponding pin is sent to the interrupt controller
    NVIC_PRI5_R |= (NVIC_PRI5_R & 0xFFFFFF1F) | (5 << 5); // 7:5 - priority 5
    NVIC_EN0_R |= 1 << 20;
}

void GPIOPortE_Handler(void){
    uint32_t status = GPIO_PORTE_RIS_R;
    if (status & PE0){
        MissileFlag = 1;
    }
    if (status & PE1){
        LaserFlag = 1;
    }
    GPIO_PORTE_ICR_R |= PIN_MASK; // acknowledge interrupt
}