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
#include "hardware/Buttons.h" // Timer 2A, 32 bits
#include "hardware/LED.h" // Timer 3A, 32 bis
#include "hardware/PLL.h" // this is fine, but double check to see if 80Mhz is correct for this lab
#include "hardware/Sound.h" // Timer 0A, 32 bit
#include "hardware/SysTick.h"
#include "hardware/Nokia5110.h"
// game logic
#include "gameLogic/bitmaps.h"
#include "gameLogic/enemies.h" // Timer 1A, 32 bit, Timer2A 32 bit
#include "gameLogic/player.h"
#include "gameLogic/projectile.h"
#include "gameLogic/random.h"
#include "gameLogic/collidable.h"

// Constant macros
#define PB4 (1 << 4)
#define PB5 (1 << 5)
#define H_MARGIN 4 // penetration margin
#define V_MARGIN 5 

// Function Prototypes
void DisableInterrupts(void);
void EnableInterrupts(void);

// Globals
const uint32_t LED1 = PB4;
const uint32_t LED2 = PB5;
static Player *PlayerShip;
static Projectile **Missiles;
static Projectile **Lasers;
static Enemy **Enemies;

// State Alias
typedef enum {Init,Reset, Game, End} State;

// State function prototypes
State Initialize_State(void);
State Reset_State(void);
State Game_State(void);
State End_State(void);

// State Table
typedef State (*stateHandler) (void); // this is how you alias function pointers apparently
stateHandler StateTable[] = {
  Initialize_State,
  Reset_State,
  Game_State,
  End_State
};

// Main

int main(void){
  State current = Init;
  while(1){
    current = StateTable[current]();
  }
  return 0;
}

// Initialization

void Hardware_Init(void){
  PLL_Init(); // set to 80 mHz
  Nokia5110_Init();
  Sound_Init(); // initializes PB0:3 for DAC output, also initializes Timer0A for interrupts
  LED_Init(); // initialize PB4:5 for LED output
  Buttons_Init(); // initialize PE0:1 for falling edge interrupts
  ADC0_Init();  // initialize ADC on PE2 / AIN1
}

void Software_Init(void){
  Random_Init(1);

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
};

State Initialize_State(void){
  Hardware_Init();
  Software_Init();
  return Game;
}

// Reset

State Reset_State(void){
  Player_Reset();
  Projectile_Reset();
  Enemies_Reset();
  return Game;
}

// Game

void Poll_Inputs(void){
    if (XposFlag) {
      XposFlag = false;
      PlayerShip->base.xPos = Xpos;
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
  Nokia5110_PrintBMP(PlayerShip->base.xPos, PlayerShip->base.yPos, PlayerShip->base.sprite->bmp, 0);

  // sEnemies
  for (int i = 0; Enemies[i] != NULL; i++){
    if (Enemies[i]->active)
      Nokia5110_PrintBMP(Enemies[i]->base.xPos, Enemies[i]->base.yPos, Enemies[i]->base.sprite->bmp, 0);
  }

  // Missiles
  for (int i = 0; Missiles[i] != NULL; i++)
  {
    if (Missiles[i]->active)
      Nokia5110_PrintBMP(Missiles[i]->base.xPos, Missiles[i]->base.yPos, Missiles[i]->base.sprite->bmp, 0);
  }

  // Lasers
  for (int i = 0; Lasers[i] != NULL; i++){
    if (Lasers[i]->active)
      Nokia5110_PrintBMP(Lasers[i]->base.xPos, Lasers[i]->base.yPos, Lasers[i]->base.sprite->bmp, 0);
  }
}

bool BitmapOverlap(Collidable *baseA, Collidable *baseB, int hMargin, int vMargin)
{
  int aLeft, aRight, aTop, aBottom, bLeft, bRight, bTop, bBottom;
  aLeft = baseA->xPos + hMargin;
  aRight = baseA->xPos + baseA->sprite->width - hMargin;
  aTop = baseA->yPos - baseA->sprite->height + vMargin;
  aBottom = baseA->yPos - vMargin;

  bLeft = baseB->xPos;
  bRight = baseB->xPos + baseB->sprite->width;
  bTop = baseB->yPos - baseB->sprite->height;
  bBottom = baseB->yPos;
  return (
    aLeft <= bRight &&
    aRight >= bLeft &&
    aTop <= bBottom &&
    aBottom >= bTop
  );
}

void For_All_Active_Enemies(void (*enemyFunc)(Enemy *enemy)){
  for(Enemy **ptr = Enemies; *ptr != NULL; ptr++){
    Enemy *enemy = *ptr;
    if (enemy->active)
      enemyFunc(enemy);
  }
}

static void Check_Lasers(Enemy *enemy){
  for(Projectile **ptr = Lasers; *ptr != NULL; ptr++){
    Projectile *laser = *ptr;
    if(BitmapOverlap(&enemy->base, &laser->base, H_MARGIN, V_MARGIN)){
      PlayerShip->score += enemy->points;
      enemy->active = false;
      laser->active = false;
    }
  }
}

static void Check_Missiles(Enemy *enemy){
  for(Projectile **ptr = Missiles; *ptr != NULL; ptr++){
    Projectile *missile = *ptr;
    if(BitmapOverlap(&enemy->base, &missile->base, H_MARGIN, V_MARGIN)){
      PlayerShip->score += enemy->points;
      enemy->active = false;
      missile->active = false;
    }
  }
}

static void Check_Player(Enemy *enemy){
  if(BitmapOverlap(&enemy->base, &PlayerShip->base, H_MARGIN, V_MARGIN)){
    enemy->active = false;
    PlayerShip->health -= enemy->dmg;
  }
}

void Check_Collisions(void){
  For_All_Active_Enemies(Check_Lasers);
  For_All_Active_Enemies(Check_Missiles); // if there is a collision, projectile and enemies are deactivated
  For_All_Active_Enemies(Check_Player);
  //For_All_Enemies(Check_Bunker);
}

bool Check_End_Conditions(void){
  // Check if the player health is below 0
  if (PlayerShip->health < 0)
    return true;

  // Check if an Enemy has made it to the bottom of the screen
	if (Check_Enemy_End()) // need to think of how I want to handle this in Enemies
		return true;

  return false;
}

State Game_State(void){
  EnableInterrupts();
  // testing
  Spawn_Enemies(4, &smallEnemy10Point_Enemy, smallEnemy10Point_Enemy.base.sprite->height * 1);
  Spawn_Enemies(3, &smallEnemy20Point_Enemy, smallEnemy20Point_Enemy.base.sprite->height * 2);
  Spawn_Enemies(2, &smallEnemy30Point_Enemy, smallEnemy30Point_Enemy.base.sprite->height * 3);
  while(1){ // main code logic
    Poll_Inputs(); // Read Inputs
    Update_Game_State();// Update Game State
    Check_OOB(); // check out of bounds
    Check_Collisions(); // collisions with enemies to projectiles, player, bunkers
    bool gameOver = Check_End_Conditions();

    // Check to see if player has lost health or an enemy has made it to the bottom of the screen
    if (gameOver){
      DisableInterrupts();
      return End;
    }

    Draw_State(); // draw everything

    // Display Graphics
    Nokia5110_DisplayBuffer();
    Nokia5110_ClearBuffer();
  }
}

// End
State End_State(void){
	char scoreBuffer[13];
  snprintf(scoreBuffer, sizeof(scoreBuffer), "%d", PlayerShip->score);

  Nokia5110_Clear();
  Nokia5110_SetCursor(0, 0);
	Nokia5110_OutString("GAME OVER");
	Nokia5110_SetCursor(0, 1);
	Nokia5110_OutString("Final score:");
  Nokia5110_SetCursor(0, 2);
  Nokia5110_OutString(scoreBuffer);
  Nokia5110_SetCursor(0, 3);
  Nokia5110_OutString("Press Both");
  Nokia5110_SetCursor(0, 4);
  Nokia5110_OutString("Buttons");
	Nokia5110_SetCursor(0, 5);
	Nokia5110_OutString("To Continue");

  while (!Buttons_Read(BUTTON_MASK)); // cleaner to poll hardware vs polling flags in a loop here
  return Reset;
}