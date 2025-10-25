#include "Buttons.h"
#include "tm4c123gh6pm.h"
#include <stdint.h>
//#include "UART.h"

#define GPIOE_DISABLE_PERIOD 0x4E200

volatile bool MissileFlag = false;
volatile bool LaserFlag = false;

void Debounce_GPIOE(void){
  GPIO_PORTE_IM_R |= BUTTON_MASK; // re-enable interrupts on GPIOE
}

// Initialize PE0:1 as negative logic input pin
void Buttons_Init(void){
    // GPIO initialization
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOE;
    while ((SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R4) == 0);
    GPIO_PORTE_CR_R    |= BUTTON_MASK;   // allow GPIOAFSEL, GPIOPUR, and GPIODEN bits to be written
    GPIO_PORTE_AMSEL_R &= ~BUTTON_MASK;  // clear analog functions
    GPIO_PORTE_PCTL_R  &= ~0xFF;      // mux the port to clear alternate function selection
    GPIO_PORTE_DIR_R   &= ~BUTTON_MASK;  // pin as input
    GPIO_PORTE_AFSEL_R &= ~BUTTON_MASK;  // clear alternate functions
    GPIO_PORTE_PUR_R   |= BUTTON_MASK;   // enable pull up resistors
    GPIO_PORTE_DEN_R   |= BUTTON_MASK;   // enable digital

    // Interrupts
    GPIO_PORTE_IS_R &= ~BUTTON_MASK; // 0 - the edge on the corresponding pin is detected
    GPIO_PORTE_IBE_R &= ~BUTTON_MASK; // 0 - the interrupt generation is controlled by the GPIO Interrupt Event (GPIOIEV) register
    GPIO_PORTE_IEV_R &= ~BUTTON_MASK; // 0 - the falling edge or a low level on the corresponding pin triggers an interrupt
    GPIO_PORTE_ICR_R |= BUTTON_MASK; // 1 - the corresponding interrupt is cleared
    GPIO_PORTE_IM_R |= BUTTON_MASK; // 1 - The interrupt from the corresponding pin is sent to the interrupt controller
    NVIC_PRI1_R = (NVIC_PRI1_R & 0xFFFFFF1F) | (3 << 5); // 7:5 - priority 3
    NVIC_EN0_R |= 1 << 4; // Interrupt Number (Bit in Interrupt Registers)

    // Timer 2A
    Timer2_Init(&Debounce_GPIOE); // this does not enable timer2
		//UART_Init();
}

bool Buttons_Read(uint32_t mask){
  mask &= BUTTON_MASK;
  if((GPIO_PORTE_DATA_R & mask) == 0)
    return true;
  else
    return false;
}

void GPIOE_Handler(void){
    // Read port
    uint32_t status = GPIO_PORTE_RIS_R;

    // start timer to re-enable interupts
    Timer2_Oneshot(GPIOE_DISABLE_PERIOD);

    GPIO_PORTE_ICR_R |= BUTTON_MASK; // acknowledge interrupt
    // disable interrupts for GPIOE
    GPIO_PORTE_IM_R &= ~BUTTON_MASK;

    // read port and set flags
    if (status & PE0){
				//UART_OutString("missile flag\r\n");
        MissileFlag = true;
    }
    if (status & PE1){
				//UART_OutString("laser flag\r\n");
        LaserFlag = true;
    }
    
}