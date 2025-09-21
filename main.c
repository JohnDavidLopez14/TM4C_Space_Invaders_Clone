// Required Hardware I/O Connections
// Slide pot pin 1 connected to ground
// Slide pot pin 2 connected to PE2/AIN1
// Slide pot pin 3 connected to +3.3V 
// fire button connected to PE0
// special weapon fire button connected to PE1
// 8*R resistor DAC bit 0 on PB0 (least significant bit)
// 4*R resistor DAC bit 1 on PB1
// 2*R resistor DAC bit 2 on PB2
// 1*R resistor DAC bit 3 on PB3 (most significant bit)
// LED on PB4
// LED on PB5

// Purple Soldered Electronics Nokia 5110
// --------------- (assumes you are facing it, going left to right)
// Signal         (Nokia 5110) LaunchPad pin
// Backlight      (LED, pin 1)  3.3V or GPIO
// Output Enable  (OE,  pin 2)  3.3V
// Reset          (RST,  pin 3) PA7
// SSFI0Fss       (CS, pin 4)   PA3
// Data/Command   (DC, pin 5)   PA6
// SSI0Tx         (MOSI, pin 6) PA5
// SSI0Clk        (CLK,  pin 7) PA2
// Ground         (GN pin 8)    ground
// 3.3V           (VCC, pin 9)  3.3V

// Pinout Summary
// Port A   - SSI/Nokia 5110    - Nokia5110.h
// PA2      - SSI0Clk           - Nokia5110.h
// PA3      - SSI0Fss           - Nokia5110.h
// PA4      -  N/A (not initialized since we are not using SSI0Rx)
// PA5      - SSI0Tx            - Nokia5110.h
// PA6      - GPIO, output      - Nokia5110.h
// PA7      - GPIO, output      - Nokia5110.h

// Port B
// PB0 - GPIO, output - DAC.h
// PB1 - GPIO, output - DAC.h
// PB2 - GPIO, output - DAC.h
// PB3 - GPIO, output - DAC.h
// PB4 - GPIO, output - LED.h
// PB5 - GPIO, output - LED.h

// Port E
// PE0 - GPIO, input, pull up - Buttons.h
// PE1 - GPIO, input, pull up - Buttons.h
// PE2 - AIN1, ADC

// include macros
#include "PLL.h" // this is fine, but double check to see if 80Mhz is correct for this lab
#include "Nokia5110.h"
#include "DAC.h"
#include "LED.h"
#include "Buttons.h"
#include "ADC.h" // need to modify this to use AIN1
#include "tm4c123gh6pm.h"
#include <stdint.h>
// #include sound.h - need to create a on output for systick interrupts to output the sound
// timer2a interrupts - I plan to use this to spawn the enemies

// constant macros
#define PB4 (1 << 4)
#define PB5 (1 << 5)
#define PE0 (1 << 0)
#define PE1 (1 << 1)

// global constants
const uint32_t LED1 = PB4;
const uint32_t LED2 = PB5;
const uint32_t Button1 = PE0;
const uint32_t Button2 = PE1;


int main(void){
    EnableInterrupts();
    PLL_Init(); // set to 80 mHz
    Nokia5110_Init();
    DAC_Init(); // initialize PB0:3 for DAC output
    LED_Init(); // initialize PB4:5 for LED output
    Buttons_Init(); // initialize PE0:1 for falling edge interrupts, not complete yet
    ADC_Init();
}