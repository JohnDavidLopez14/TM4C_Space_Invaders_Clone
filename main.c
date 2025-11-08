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
// Timer4A - Priority 7
// Timer5a - Priority 7

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
#include "hardware/LED.h" // Timer 4A, 32 bits
#include "hardware/PLL.h" // this is fine, but double check to see if 80Mhz is correct for this lab
#include "hardware/Sound.h" // Timer 0A, 32 bit
#include "hardware/SysTick.h"
#include "hardware/Nokia5110.h"
#include "hardware/Timer5.h" // used for delays 
// game logic
#include "gameLogic/bitmaps.h"
#include "gameLogic/enemies.h" // Timer 1A, 32 bit
#include "gameLogic/explosions.h" // Timer 3A, 32 bits
#include "gameLogic/player.h"
#include "gameLogic/projectile.h"
#include "gameLogic/random.h"
#include "gameLogic/collidable.h"

// Constant macros
#define PB4 (1 << 4)
#define PB5 (1 << 5)
#define H_MARGIN 2 // penetration margin
#define V_MARGIN 2
#define HEALTH_LED 30 // if player health is below this, red led starts blinking 
#define POINT_LED 100 // the threshold for blinking the green led

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
static Explosion **Explosions;
unsigned int point_event_counter = 0; // counts everytime 100 points are reached
volatile bool TimerFlag = false;

// State Alias
typedef enum {Init,Reset, Game, Lose, End} State;

// State function prototypes
State Initialize_State(void);
State Reset_State(void);
State Game_State(void);
State Lose_State(void);
State End_State(void);

// State Table
typedef State (*stateHandler) (void); // this is how you alias function pointers apparently
stateHandler StateTable[] = {
  Initialize_State,
  Reset_State,
  Game_State,
  Lose_State,
  End_State
};

// Timers

void Timer_Periodic_Task(void){
  TimerFlag = true;
}


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
  Timer5_Init(Timer_Periodic_Task);
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
  
  // Explosion Initialization
  Explosions_Init();
  Explosions = Get_Explosions();
};

State Initialize_State(void){
  Hardware_Init();
  Software_Init();
  return Game;
}

// Reset

void Flag_Reset(void){
  MissileFlag = false;
  LaserFlag = false;
}

State Reset_State(void){
  Player_Reset();
  Projectile_Reset();
  Enemies_Reset();
  Flag_Reset();
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
  if (Explosion_Update_Flag){
    Explosion_Update_Flag = false;
    Update_Explosion_Frames();
  }
}

void Check_OOB(void){
  Check_Lasers_OOB();
  Check_Missiles_OOB();
}

void Draw_State(State current){
  // Player Ship
  if (current == Game)
    Nokia5110_PrintBMP(PlayerShip->base.xPos, PlayerShip->base.yPos, PlayerShip->base.sprite->bmp, 0);

  // Enemies
  for (Enemy **ptr = Enemies; *ptr != NULL; ptr++){
    if ((*ptr)->active)
      Nokia5110_PrintBMP((*ptr)->base.xPos, (*ptr)->base.yPos, (*ptr)->base.sprite->bmp, 0);
  }

  // Missiles
  for (Projectile **ptr = Missiles; *ptr != NULL; ptr++)
  {
    if ((*ptr)->active)
      Nokia5110_PrintBMP((*ptr)->base.xPos, (*ptr)->base.yPos, (*ptr)->base.sprite->bmp, 0);
  }

  // Lasers
  for (Projectile **ptr = Lasers; *ptr != NULL; ptr++){
    if ((*ptr)->active)
      Nokia5110_PrintBMP((*ptr)->base.xPos, (*ptr)->base.yPos, (*ptr)->base.sprite->bmp, 0);
  }
	
  // Explosions
	for (Explosion **ptr = Explosions; *ptr != NULL; ptr++){
		if((*ptr)->active){
			Nokia5110_PrintBMP((*ptr)->base.xPos, (*ptr)->base.yPos, (*ptr)->base.sprite->bmp, 0);
		}
	}
}

bool BitmapOverlap(Collidable *baseA, Collidable *baseB, int hMargin, int vMargin)
{
  int aLeft, aRight, aTop, aBottom, bLeft, bRight, bTop, bBottom;
  aLeft = baseA->xPos + hMargin;
  aRight = baseA->xPos + baseA->sprite->width - hMargin;
  aTop = baseA->yPos - baseA->sprite->height + vMargin;
  aBottom = baseA->yPos - vMargin;

  bLeft = baseB->xPos + hMargin;
  bRight = baseB->xPos + baseB->sprite->width - hMargin;
  bTop = baseB->yPos - baseB->sprite->height + vMargin;
  bBottom = baseB->yPos - vMargin;
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
		if (!laser->active) continue;
    if(BitmapOverlap(&enemy->base, &laser->base, H_MARGIN, V_MARGIN)){
      PlayerShip->score += enemy->points;
      enemy->active = false;
      laser->active = false;
      Spawn_Enemy_Explosion(&enemy->base);
    }
  }
}

static void Check_Missiles(Enemy *enemy){
  for(Projectile **ptr = Missiles; *ptr != NULL; ptr++){
    Projectile *missile = *ptr;
		if(!missile->active) continue;
    if(BitmapOverlap(&enemy->base, &missile->base, H_MARGIN, V_MARGIN)){
      PlayerShip->score += enemy->points;
      enemy->active = false;
      missile->active = false;
      Spawn_Enemy_Explosion(&enemy->base);
    }
  }
}

static void Check_Player(Enemy *enemy){
  if(BitmapOverlap(&enemy->base, &PlayerShip->base, H_MARGIN, V_MARGIN)){
    enemy->active = false;
    PlayerShip->health -= enemy->dmg;
    Spawn_Enemy_Explosion(&enemy->base);
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
  if (PlayerShip->health <= 0)
    return true;

  // Check if an Enemy has made it to the bottom of the screen
	if (Check_Enemy_End()) // need to think of how I want to handle this in Enemies
		return true;

  return false;
}

void Check_Player_Health(void){
  if (PlayerShip->health <= HEALTH_LED && !PB5_Get_State())
    PB5_Blink_Start(0,1);
}

void Check_Current_Points(void){
  unsigned int current_point_counter = PlayerShip->score / POINT_LED;
  if(current_point_counter > point_event_counter) // if another 100 point increment has goten hit
    if(!PB4_Get_State()){ // if the led is not blinking
      PB4_Blink_Start(2,1);
      point_event_counter = current_point_counter;
    }
}

State Game_State(void){
  EnableInterrupts();
  // testing
  Spawn_Enemies(4, &smallEnemy10Point_Enemy, smallEnemy10Point_Enemy.base.sprite->height * 2);
  Spawn_Enemies(4, &smallEnemy10Point_Enemy, smallEnemy10Point_Enemy.base.sprite->height * 3);
  Spawn_Enemies(4, &smallEnemy10Point_Enemy, smallEnemy10Point_Enemy.base.sprite->height * 4);
  while(1){ // main code logic
    Poll_Inputs(); // Read Inputs
    Update_Game_State();// Update Game State
    Check_OOB(); // check out of bounds
    Check_Collisions(); // collisions with enemies to projectiles, player, bunkers
    Check_Player_Health(); // checks player health, will activate led if below 30 - this will not turn off until end state
		Check_Current_Points();
    bool gameOver = Check_End_Conditions();

    // Check to see if player has lost health or an enemy has made it to the bottom of the screen
    if (gameOver){
      return Lose;
    }

    Draw_State(Game); // draw everything

    // Display Graphics
    Nokia5110_DisplayBuffer();
    Nokia5110_ClearBuffer();
  }
}

bool Check_If_Explosions_Active(void){
  for(Explosion **ptr = Explosions; *ptr != NULL; ptr++){
    if((*ptr)->active)
      return true;
  }
  return false;
}

State Lose_State(void){
  Clear_All_LED_Events();
  Spawn_Player_Explosion(&PlayerShip->base);
  while (Check_If_Explosions_Active()){
    Update_Game_State();
    Draw_State(Lose);
    Nokia5110_DisplayBuffer();
    Nokia5110_ClearBuffer();
  }
  Timer5_Oneshot(0x09896800);
  while(!TimerFlag);
  TimerFlag = false;
  return End;
}

// End
State End_State(void){
	LED_Off(PIN_MASK); // turn off all leds
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

  while (!Buttons_Read(BUTTON_MASK)); // wait for both buttons to be pressed
  while (Buttons_Read(BUTTON_MASK)); // wait for both buttons to be released
  Timer5_Oneshot(0x09896800); // timer delay before resetting
  while(!TimerFlag);
  TimerFlag = false;
  return Reset;
}