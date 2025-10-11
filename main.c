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

// ISR Priority
// Systick - Priority 2
// Timer2A - Priority 4
// GPIOPORTE - Priority 5

// Include macros
// library
#include <stdint.h>
#include <stdio.h>
// hardware
#include "hardware/tm4c123gh6pm.h"
#include "hardware/ADC.h"
#include "hardware/Buttons.h"
#include "hardware/LED.h"
#include "hardware/PLL.h" // this is fine, but double check to see if 80Mhz is correct for this lab
#include "hardware/Sound.h"
#include "hardware/Nokia5110.h"
#include "hardware/UART.h"
// game logic
#include "gameLogic/bitmaps.h"
#include "gameLogic/player.h"
#include "gameLogic/projectile.h"
#include "gameLogic/random.h"
// timer2a interrupts - I plan to use this to spawn the enemies

// Constant macros
#define PB4 (1 << 4)
#define PB5 (1 << 5)
#define SMOOTH_DEN 8
#define ADCMIN 2000
#define ADCMAX 3400

// Global Constants
const uint32_t LED1 = PB4;
const uint32_t LED2 = PB5;

// Global Variables
volatile unsigned long ADCdata;
volatile unsigned long SmoothedADC = 0;
volatile int XposFlag = 0;
volatile unsigned long Xpos;

// Function Prototypes
void DisableInterrupts(void);
void EnableInterrupts(void);
void SysTick_Init(void);

int main(void){
  // Hardware Initialization
  UART_Init();
  EnableInterrupts();
  PLL_Init(); // set to 80 mHz
  Nokia5110_Init();
  Sound_Init(); // initializes PB0:3 for DAC output, also initializes Timer0A for interrupts
  LED_Init(); // initialize PB4:5 for LED output
  Buttons_Init(); // initialize PE0:1 for falling edge interrupts, not complete yet
  SysTick_Init();
  ADC0_Init();  // initialize ADC on PE2 / AIN1
  Random_Init(1);

  // Game Initialization
  
  // Player Initialization
  Player_Init();
  Player* playerShip = Get_Player();
  

  // Projectile Initialization
  Projectile_Init(playerShip);
  Projectile **Missiles = Get_Missiles(); // returns a null terminated array
  Projectile **Lasers = Get_Lasers(); // returns a null terminated array


  // Wave_Init();

  while(1){ // main code logic
      // Read Inputs / Poll Flags
      if (XposFlag) {
        XposFlag = 0;
        playerShip->xPos = Xpos;
      }

      if (MissileFlag){
        MissileFlag = 0; // clear the missile flag
        Fire_Missile(); // sets missile to active and initializes parameters
      }

      if (LaserFlag){
        LaserFlag = 0; // clear the laser flag
        Fire_Laser(); // sets laser to active and initializes parameters
      }
		
      // Collision Detection

      // Update Game State
      Update_Missile_Position();
      Update_Laser_Position();

      // Out of bounds detection, sets missile to inactive if their new position is out of bounds
      Check_Missiles_OOB();
      Check_Lasers_OOB();

         // Draw Everything
      Nokia5110_PrintBMP(playerShip->xPos, playerShip->yPos, playerShip->sprite->bmp, 0);
      
      int i = 0;
      while (Missiles[i] != NULL){
        Nokia5110_PrintBMP(Missiles[i]->xPos, Missiles[i]->yPos, Missiles[i]->sprite->bmp, 0);
        i++;
      }

      int i = 0;
      while (Lasers[i] != NULL){
        Nokia5110_PrintBMP(Lasers[i]->xPos, Lasers[i]->yPos, Lasers[i]->sprite->bmp, 0);
        i++;
      }

      // Display Graphics
      Nokia5110_DisplayBuffer();
      Nokia5110_ClearBuffer();
  }
}


// Converts ADC to a position on the screen
unsigned long Convert(unsigned long sample){
  if (sample < ADCMIN){
    sample = ADCMIN;
  }
  if (sample > ADCMAX){
    sample = ADCMAX;
  }
  unsigned long xMax = SCREENW - playerShip->sprite->width;
  unsigned long result = (xMax  * (sample - ADCMIN)) / (ADCMAX - ADCMIN);
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
  SmoothedADC = (SmoothedADC * 7 + ADCdata) / SMOOTH_DEN; // ADC smoothing, not sure exactly how this works
  Xpos = Convert(SmoothedADC); // convert ADC to a value on the screen
  XposFlag = 1; // set mail box
}