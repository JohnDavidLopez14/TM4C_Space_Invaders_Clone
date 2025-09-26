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

// Include macros
#include "PLL.h" // this is fine, but double check to see if 80Mhz is correct for this lab
#include "Nokia5110.h"
#include "Sound.h"
#include "LED.h"
#include "Buttons.h"
#include "ADC.h" // need to modify this to use AIN1
#include "random.h"
#include "bitmaps.h"
#include "tm4c123gh6pm.h"
#include <stdint.h>
// timer2a interrupts - I plan to use this to spawn the enemies

// Constant macros
#define PB4 (1 << 4)
#define PB5 (1 << 5)
#define PE0 (1 << 0)
#define PE1 (1 << 1)
#define MAX_MISSILES 10
#define MAX_ENEMIES 20
#define SMOOTH_DEN 8
#define ADCMIN 2070
#define ADCMAX 4095

// Global Constants
const uint32_t LED1 = PB4;
const uint32_t LED2 = PB5;
const uint32_t Button1 = PE0;
const uint32_t Button2 = PE1;

// Function Prototypes
void DisableInterrupts(void);
void EnableInterrupts(void);
void SysTick_Init(void);
void SysTick_Handler(void);

typedef struct {
    Bitmap *sprite; // these structs are exported in bitmaps.h
    unsigned int health;
    unsigned int xPos, yPos;
} Entity;

// Global Variables
unsigned long ADCdata;
unsigned long SmoothedADC = 0;
unsigned long Flag;
unsigned long Xpos;
Entity missiles[MAX_MISSILES]; // pre-allocated missile array
Entity enemies[MAX_ENEMIES];
Entity playerShip; // global namespace since its parameters are being used in Convert

int main(void){
    EnableInterrupts();
    PLL_Init      ();   // set to 80 mHz
    Nokia5110_Init();   //
    Sound_Init    ();   // initializes PB0:3 for DAC output, also initializes Timer0A for interrupts
    LED_Init       ();  // initialize PB4:5 for LED output
    Buttons_Init  ();   // initialize PE0:1 for falling edge interrupts, not complete yet
    ADC0_Init       ();  // initialize ADC on PE2 / AIN1
    Random_Init(1);

    // Initialize playership
    playerShip.sprite = &playerShip0;
    playerShip.health = 100;
    playerShip.xPos = (SCREENW - playerShip.sprite->width) / 2; // start at the center of the screen
    playerShip.yPos = SCREENH; // start at the bottom of the screen

    while(1){ // main code logic
        Nokia5110_PrintBMP(playerShip.xPos, playerShip.yPos, playerShip.sprite->bmp, 0);
        Nokia5110_DisplayBuffer();
        Nokia5110_ClearBuffer();
				for (volatile int i = 0; i < 1000000 ; i++); // ghetto delay, need to get rid of this once the program is done
        if (Flag) {
          Flag = 0;
          playerShip.xPos = Xpos;
        }

        //playership.xPos = 10;//Random() % MAX_X;
        //playership.yPos = 10;//Random() % MAX_Y;
        // need to use Timer2 to periodically spawn enemies
        // need to design a demo to work with the player ship moving with the slide pot
    }
}

// Converts ADC to a position on the screen
unsigned long Convert(unsigned long sample){
    unsigned long xMax = SCREENW - playerShip.sprite->width;
    unsigned long result = (xMax  * (sample - ADCMIN)) / (ADCMAX - ADCMIN);
    if (result > xMax){
      result = xMax;
    }
		return result;
}

// Initialize SysTick interrupts to trigger at 40 Hz, 25 ms
void SysTick_Init(void){
  NVIC_ST_CTRL_R = 0;                                             // turn off Systick
  NVIC_ST_CURRENT_R = 0;                                          // clear the counter
  NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R & 0x00FFFFFF) | 0x40000000;  // set priority 2
  NVIC_ST_RELOAD_R = 0x1E847F; // 2000000 - 1 in hex, this is allowed since reload is 24 bits
  NVIC_ST_CTRL_R |= 0x07; // enable systic, system clock, interrupt generation
}
// executes every 25 ms, collects a sample, converts and stores in mailbox
void SysTick_Handler(void){ 
  ADCdata = ADC0_In();
  SmoothedADC = (SmoothedADC * 7 + ADCdata) / SMOOTH_DEN; // ADC smoothing, not sure excatly how this works
  Xpos = Convert(SmoothedADC); // convert ADC to a value on the screen
  Flag = 1; // set mail box
}