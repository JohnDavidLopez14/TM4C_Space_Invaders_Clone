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
// GPIOPORTE - Priority 3
// Timer0A - Priority 4
// Timer1A - Priority 5
// Timer2A - Priority 6
// Timer3A - Priority 7

// Include macros
// library
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
// hardware
#include "hardware/tm4c123gh6pm.h"
#include "hardware/ADC.h"
#include "hardware/Buttons.h"
#include "hardware/LED.h" // Timer 3A, 32 bis
#include "hardware/PLL.h" // this is fine, but double check to see if 80Mhz is correct for this lab
#include "hardware/Sound.h" // Timer 0A, 32 bit
#include "hardware/SysTick.h"
#include "hardware/Nokia5110.h"
#include "hardware/UART.h"
// game logic
#include "gameLogic/bitmaps.h"
#include "gameLogic/enemies.h" // Timer 1A, 32 bit, Timer2A 32 bit
#include "gameLogic/player.h"
#include "gameLogic/projectile.h"
#include "gameLogic/random.h"

// Constant macros
#define PB4 (1 << 4)
#define PB5 (1 << 5)

// Globals
const uint32_t LED1 = PB4;
const uint32_t LED2 = PB5;
static Player *PlayerShip;
static Projectile **Missiles;
static Projectile **Lasers;
static Enemy **Enemies;


// Function Prototypes
void DisableInterrupts(void);
void EnableInterrupts(void);

void Poll_Inputs(void){
    if (XposFlag) {
      XposFlag = false;
      PlayerShip->xPos = Xpos;
    }

    if (MissileFlag){
      MissileFlag = false;
      Fire_Missile();
    }

    if (LaserFlag){
      LaserFlag = false;
      Fire_Laser();
    }
}

void Update_Game_State(void){
  Update_Missile_Position();
  Update_Laser_Position();
  Update_Enemies_Position();
}

void Check_OOB(void){
  Check_Lasers_OOB();
  Check_Missiles_OOB();
}

void Draw_State(void){
  // Player Ship
  Nokia5110_PrintBMP(PlayerShip->xPos, PlayerShip->yPos, PlayerShip->sprite->bmp, 0);

  // sEnemies
  for (int i = 0; Enemies[i] != NULL; i++){
    if (Enemies[i]->active)
      Nokia5110_PrintBMP(Enemies[i]->xPos, Enemies[i]->yPos, Enemies[i]->sprite->bmp, 0);
  }

  // Missiles
  for (int i = 0; Missiles[i] != NULL; i++)
  {
    if (Missiles[i]->active)
      Nokia5110_PrintBMP(Missiles[i]->xPos, Missiles[i]->yPos, Missiles[i]->sprite->bmp, 0);
  }

  // Lasers
  for (int i = 0; Lasers[i] != NULL; i++){
    if (Lasers[i]->active)
      Nokia5110_PrintBMP(Lasers[i]->xPos, Lasers[i]->yPos, Lasers[i]->sprite->bmp, 0);
  }
}

int main(void){
  // Hardware Initialization
  UART_Init();
  PLL_Init(); // set to 80 mHz
  Nokia5110_Init();
  Sound_Init(); // initializes PB0:3 for DAC output, also initializes Timer0A for interrupts
  LED_Init(); // initialize PB4:5 for LED output
  Buttons_Init(); // initialize PE0:1 for falling edge interrupts, not complete yet
  ADC0_Init();  // initialize ADC on PE2 / AIN1
  Random_Init(1);

  // Game Initialization
  
  // Player Initialization
  Player_Init();
  PlayerShip = Get_Player();
  SysTick_Init(PlayerShip);

  // Enemy Initialization
  Enemies_Init();
  Enemies = Get_Enemies();
  
  // Projectile Initialization
  Projectile_Init(PlayerShip);
  Missiles = Get_Missiles(); // returns a null terminated array
  Lasers = Get_Lasers(); // returns a null terminated array

  // testing
  Spawn_Enemies(4, 100, &smallEnemy10PointA, &smallEnemy10PointB, smallEnemy10PointA.height, 0, 0);
  Spawn_Enemies(3, 100, &smallEnemy20PointA, &smallEnemy20PointB, smallEnemy20PointA.height * 2, 0, 0);
  Spawn_Enemies(2, 100, &smallEnemy30PointA, &smallEnemy30PointB, smallEnemy30PointA.height * 3, 0, 0);
  EnableInterrupts();
  while(1){ // main code logic
    Poll_Inputs(); // Read Inputs
    Update_Game_State();// Update Game State
    Check_OOB(); // check out of bounds
    Draw_State(); // draw everything

    // Display Graphics
    Nokia5110_DisplayBuffer();
    Nokia5110_ClearBuffer();
  }
}