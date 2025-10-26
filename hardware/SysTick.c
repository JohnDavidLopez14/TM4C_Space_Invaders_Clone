#include "hardware/SysTick.h"

#define SMOOTH_DEN 8
#define ADCMIN 2200
#define ADCMAX 3400

// Global Variables
volatile bool XposFlag = false;
volatile unsigned long Xpos;
static Player *PlayerShip;

static unsigned long SmoothedADC = 0;

// Initialize SysTick interrupts to trigger at 40 Hz, 25 ms
void SysTick_Init(Player *playerShip){
    PlayerShip = playerShip;
    NVIC_ST_CTRL_R = 0;                                             // turn off Systick
    NVIC_ST_CURRENT_R = 0;                                          // clear the counter
    NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R & 0x00FFFFFF) | 0x40000000;  // set priority 2
    NVIC_ST_RELOAD_R = 0x1E847F; // 2000000 - 1 in hex, this is allowed since reload is 24 bits
    NVIC_ST_CTRL_R |= 0x07; // enable systic, system clock, interrupt generation
}

// Converts ADC to a position on the screen
unsigned long Convert(unsigned long sample){
    if (sample < ADCMIN){
        sample = ADCMIN;
    }
    if (sample > ADCMAX){
        sample = ADCMAX;
    }
    unsigned long xMax = SCREENW - PlayerShip->base.sprite->width;
    unsigned long result = (xMax  * (sample - ADCMIN)) / (ADCMAX - ADCMIN);
    return result;
}

// executes every 25 ms, collects a sample, converts and stores in mailbox
void SysTick_Handler(void){ 
    SmoothedADC = (SmoothedADC * 7 + ADC0_In()) / SMOOTH_DEN; // ADC smoothing, not sure exactly how this works
    Xpos = Convert(SmoothedADC); // convert ADC to a value on the screen
    XposFlag = true; // set mail box
}