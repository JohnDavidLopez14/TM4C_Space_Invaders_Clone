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
#include "PLL.h" // this is fine, but double check to see if 80Mhz is correct for this lab
#include "Nokia5110.h"
#include "Sound.h"
#include "LED.h"
#include "Buttons.h"
#include "ADC.h" // need to modify this to use AIN1
#include "random.h"
#include "bitmaps.h"
#include "tm4c123gh6pm.h"
#include "UART.h"
#include <stdint.h>
#include <stdio.h>
// timer2a interrupts - I plan to use this to spawn the enemies

// Constant macros
#define PB4 (1 << 4)
#define PB5 (1 << 5)
#define MAX_MISSILES 10
#define MAX_LASERS 20
#define MISSILEV .05 // pixel/tick
#define LASERV .05 // pixel/tick
#define SMOOTH_DEN 8
#define ADCMIN 2000
#define ADCMAX 3400
#define MAX_WAVES 4
#define WAVEV .001
#define VWAVESPACING 14
#define MAX_WAVE_ENEMIES 5
#define MAX_ENEMIES (MAX_WAVES * MAX_WAVE_ENEMIES) // can only fit enemies across the screen max
#define ENEMYWIDTH 16

// Global Constants
const uint32_t LED1 = PB4;
const uint32_t LED2 = PB5;
unsigned int enemySpawn = 5000;

// Function Prototypes
void DisableInterrupts(void);
void EnableInterrupts(void);
void SysTick_Init(void);
void Fire_Missile(void);
void Fire_Laser(void);
void Update_Missile_Position(void);
void Update_Laser_Position(void);
void Check_Missiles(void);
void Check_Lasers(void);
void SpawnWave(int *);

typedef struct {
    const Bitmap *sprite; // these structs are exported in bitmaps.h
    unsigned int health;
    unsigned int xPos, yPos;
} Player;

typedef struct {
  const Bitmap *sprite;
  unsigned int active;
  unsigned int xPos, yPos; // stores the screen position
	float xReal, yReal; // stores the actual position
  float dy; // this will be pixels/tick
} Projectile;

typedef struct{
  unsigned int active;
  unsigned int xPos;
  float xReal;
} Enemy;

typedef struct{
  unsigned int active;
  Enemy *enemyList;
  unsigned int listLength;
  unsigned int yPos;
  float yReal;
  float dy;
} Wave;

// Global Variables
volatile unsigned long ADCdata;
volatile unsigned long SmoothedADC = 0;
volatile int XposFlag = 0;
volatile unsigned long Xpos;
volatile int EnemyFlag = 0;
Player PlayerShip; // global namespace since its parameters are being used in Convert
Projectile Missiles[MAX_MISSILES]; // pre-allocated missile array, probably move these into the main namespace
Projectile Lasers[MAX_LASERS];
Enemy Enemies[MAX_ENEMIES];
Wave Waves[MAX_WAVES];

int main(void){
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

  // Initialize playership
  PlayerShip.sprite = &playerShip0;
  PlayerShip.health = 100;
  PlayerShip.xPos = (SCREENW - PlayerShip.sprite->width) / 2; // start at the center of the screen
  PlayerShip.yPos = SCREENH - 1;  // start at the bottom of the screen

  // Initialize missile array
  for (int i = 0; i < MAX_MISSILES; i++){
    Projectile *missile = &Missiles[i];
    missile->sprite = &missile0;
    missile->active = 0;
    missile->xPos = 0;
    missile->yPos = 0;
    missile->xReal = 0;
    missile->yReal = 0;
    missile->dy = 0;
  }

  // Initialize laser array
  for (int i = 0; i < MAX_LASERS; i++){
    Projectile *laser = &Lasers[i];
    laser->sprite = &laser0;
    laser->active = 0;
    laser->xPos = 0;
    laser->yPos = 0;
    laser->xReal = 0;
    laser->yReal = 0;
    laser->dy = 0;
  }

  // Initialize wave array
  for (int i = 0; i < MAX_WAVES; i++){
    Wave *wave = &Waves[i];
    wave->active = 0;
    wave->enemyList = &Enemies[i * MAX_WAVE_ENEMIES];
    wave->listLength = 0;
    wave->yPos = 0;
    wave->yReal = 0;
    wave->dy = 0;
  }

  // used to keep track of most recent wave that has been spawned
  int currentWaveIndex = -1;

  while(1){ // main code logic
      // Read Inputs / Poll Flags
      if (XposFlag) {
        XposFlag = 0;
        PlayerShip.xPos = Xpos;
      }

      if (MissileFlag){
        MissileFlag = 0; // clear the missile flag
        Fire_Missile(); // sets missile to active and initializes parameters
      }

      if (LaserFlag){
        LaserFlag = 0; // clear the laser flag
        Fire_Laser(); // sets laser to active and initializes parameters
      }

      if (EnemyFlag){ // need to initialize a timer to set this flag later on
        EnemyFlag = 0;
        SpawnWave(&currentWaveIndex);
      }

      // Collision Detection

      // Update Game State
      Update_Missile_Position();
      Update_Laser_Position();

      // Out of bounds detection
      Check_Missiles();
      Check_Lasers();

         // Draw Everything
      Nokia5110_PrintBMP(PlayerShip.xPos, PlayerShip.yPos, PlayerShip.sprite->bmp, 0);

      for (int i = 0; i < MAX_MISSILES; i++){
				if (Missiles[i].active){
					Nokia5110_PrintBMP(Missiles[i].xPos, Missiles[i].yPos, Missiles[i].sprite->bmp, 0);
				}
      }

      for (int i = 0; i < MAX_LASERS; i ++){
				if (Lasers[i].active){
					Nokia5110_PrintBMP(Lasers[i].xPos, Lasers[i].yPos, Lasers[i].sprite->bmp, 0);
				}
      }

      // Display Graphics
      Nokia5110_DisplayBuffer();
      Nokia5110_ClearBuffer();
  }
}

// Searches for a Wave that is inactive
// returns the inactive wave and sets currentWaveIndex to that Wave's index
Wave *WaveSearch(Wave *waveList, unsigned int length, int *currentWaveIndex){
  for(unsigned int i = 0; i < length; i++){
    Wave *currentWave = &waveList[i];
    if(currentWave->active == 0){
      *currentWaveIndex = i;
      return currentWave;
    }
  }
  return NULL;
}


// generate a location for the enemy given the number of enemies and which enemy
unsigned int EnemySpacing(int enemyNumber, int enemyIndex){
  float totalWidth = enemyNumber * ENEMYWIDTH;
  float spacing = (SCREENW - totalWidth) / (enemyNumber + 1.0f);
  float x = spacing * (enemyIndex + 1) + ENEMYWIDTH * enemyIndex;
  return (unsigned int)(x + 0.5f);  // round to nearest int
}


// initializes the enemies
void InitializeWave(Wave *currentWave,int *currentWaveIndex,unsigned long enemyNumber, float velocity){
  // Initialize wave
  currentWave->active = 1;
  currentWave->dy = velocity;
  currentWave->listLength = enemyNumber;
  currentWave->yPos = 0;
  currentWave->yReal = (float) currentWave->yPos;

  // initialize wave enemies
  for (int i = 0; i < currentWave->listLength; i++){
    Enemy *currentEnemy = &currentWave->enemyList[i];
    currentEnemy->active = 1;
    currentEnemy->xPos = EnemySpacing(enemyNumber, i);
    currentEnemy->xReal = (float) currentEnemy->xPos;
  }
}

void SpawnWave(int *currentWaveIndex){
  // if the no waves are spawned, the most recent wave in active, or if the most recent wave is far enough down the screen to spawn a new wave
  if (*currentWaveIndex == -1 || Waves[*currentWaveIndex].active == 0 || Waves[*currentWaveIndex].yPos > VWAVESPACING){
    Wave *currentWave = WaveSearch(Waves, MAX_WAVES, currentWaveIndex);
    if (currentWave != NULL){ // if we have a wave available to spawn
      unsigned long enemyNumber = (Random() % 5) + 1;
      InitializeWave(currentWave, currentWaveIndex, enemyNumber, WAVEV);
    }
  }
}

void Check_Projectile(Projectile *projectileList, int length){
  for(int i = 0; i < length; i++){
    if(projectileList[i].active == 1){
      Projectile *projectile = &projectileList[i];
      if (projectile->yPos < 0 || projectile->yPos > SCREENH || projectile->xPos < 0 || projectile->xPos > SCREENW){
        projectile->active = 0;
      }
    }
  }
}

void Check_Missiles(){
  Check_Projectile(Missiles, MAX_MISSILES);
}

void Check_Lasers(){
  Check_Projectile(Lasers, MAX_LASERS);
}

void Update_Projectile_Position(Projectile *projectileList, int length){
  for (int i = 0; i < length; i++){
    Projectile *projectile = &projectileList[i];
    if(projectile->active == 1){ // iterate through active projectiles
			projectile->yReal -= projectile->dy; // subtract dy every frame, this is because we're doing this each tick, ie yo - dy * delta t
      projectile->yPos =  (int)(projectile->yReal + 0.5f); // round up and then type cast
    }
  }
}

void Update_Missile_Position(void){
  Update_Projectile_Position(Missiles, MAX_MISSILES);
}

void Update_Laser_Position(void){
  Update_Projectile_Position(Lasers, MAX_LASERS);
}

// searches projectile list for active status
// bool - 1 for active, 0 for inactive
// returns a NULL if nothing is found, a projectile pointer if found
Projectile * Projectile_Search(Projectile *projectileList, int length, unsigned int state){
  for (int i = 0; i < length; i++){
    if(projectileList[i].active == state)
      return &projectileList[i];
  }
  return NULL;
}

void Fire_Projectile(Projectile *projectileList, int length, const Bitmap *bitmapObject, float velocity){
  Projectile *projectile;
  projectile = Projectile_Search(projectileList, length, 0); // search for inactive projectile
  if (projectile != NULL){
    projectile->sprite = bitmapObject;
    projectile->active = 1;
    projectile->xPos = PlayerShip.xPos + (PlayerShip.sprite->width - projectile->sprite->width) / 2; //center the projectile on the ship
    projectile->yPos = PlayerShip.yPos - PlayerShip.sprite->height;
		projectile->xReal = projectile->xPos;
		projectile->yReal = projectile->yPos;
    projectile->dy = velocity;
  }
}

// Fires missile
void Fire_Missile(void){
  Fire_Projectile(Missiles, MAX_MISSILES, &missile0, MISSILEV);
}

// Fires laser
void Fire_Laser(void){
  Fire_Projectile(Lasers, MAX_LASERS, &laser0, LASERV);
}

// Converts ADC to a position on the screen
unsigned long Convert(unsigned long sample){
  if (sample < ADCMIN){
    sample = ADCMIN;
  }
  if (sample > ADCMAX){
    sample = ADCMAX;
  }
  unsigned long xMax = SCREENW - PlayerShip.sprite->width;
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

  // enemy spawn shoved int Systick until I create another timer to handle this
  enemySpawn--;
  if (enemySpawn == 0){
    EnemyFlag = 0;
  }
}