#include "hardware/Timer3.h"

#define TIMER_NUM 3
#define PRIORITY 7

static void (*PeriodicTask)(void);
static unsigned long Period;

void Timer3_Init(void(*task)(void), unsigned long period){
    PeriodicTask = task;
    SYSCTL_RCGCTIMER_R |= (1 << TIMER_NUM); // 0) Activate TimerA
    // offset 0x604
    TIMER3_CTL_R &= ~0X01; // 1) disable TimerA during setup
    // offset 0x00C
    TIMER3_CFG_R &= ~0x07; // 2) configure for 32 bit mode
    // offset 0x000
    TIMER3_TAMR_R = (TIMER3_TAMR_R & ~0x03) | 0x02; // 3) configure for periodic mode, defalut down-count settings
    // offset 0x004
    Period = period - 1; // 4) reload value
    // offset 0x028
    TIMER3_TAPR_R &= ~0xFFFF; // 5) bus clock resolution
    // offset 0x038
    TIMER3_ICR_R |= TIMER_ICR_TATOCINT; // 6) Clear TimerA timeout flag    
    // offset 0x024
    TIMER3_IMR_R |= 0x01; // 7) arm timeout interrupt
    // offset 0x018
    NVIC_PRI8_R = (NVIC_PRI8_R & ~(0x07 << 29)) | (PRIORITY << 29); // 8) priority
    // Vector Number 51, Interrupt Number 35
    // offset 0x410
    // bits 15:13
    NVIC_EN1_R |= (1<<3); // 9) Enable the correct irq IN nvic
    // offset 0x100
    //TIMER3_CTL_R |= 0X01; // 10) Enable TimerA

}

void Timer3_Enable(void){
    TIMER3_TAILR_R = Period - 1; // 4) reload value
    TIMER3_CTL_R |= 0X01; // 10) Enable TimerA
}

void Timer3_Disable(void){
    TIMER3_CTL_R &= ~0X01; // disable TimerA
}

void Timer3A_Handler(void){
    TIMER3_ICR_R |= TIMER_ICR_TATOCINT; // clear TimerA timeout flag

    (*PeriodicTask)();
}