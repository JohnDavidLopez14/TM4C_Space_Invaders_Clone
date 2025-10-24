#include "hardware/Timer2.h"

#define TIMER_NUM 2
#define PRIORITY 6

static void (*PeriodicTask)(void);

void Timer2_Init(void(*task)(void)){
    PeriodicTask = task;
    SYSCTL_RCGCTIMER_R |= (1 << TIMER_NUM); // 0) Activate Timer2
    // offset 0x604
    TIMER2_CTL_R &= ~0X01; // 1) disable Timer2 during setup
    // offset 0x00C
    TIMER2_CFG_R &= ~0x07; // 2) configure for 32 bit mode
    // offset 0x000
    TIMER2_TAMR_R = (TIMER2_TAMR_R & ~0x03) | 0x01; // 3) configure for one shot mode, default down-count settings
    // offset 0x004
    // TIMER2_TAILR_R = period - 1; // 4) reload value
    // offset 0x028
    TIMER2_TAPR_R &= ~0xFFFF; // 5) bus clock resolution
    // offset 0x038
    TIMER2_ICR_R |= TIMER_ICR_TATOCINT; // 6) Clear TimerA timeout flag    
    // offset 0x024
    TIMER2_IMR_R |= 0x01; // 7) arm timeout interrupt
    // offset 0x018
    NVIC_PRI5_R = (NVIC_PRI5_R & ~(0x07 << 29)) | (PRIORITY << 29); // 8) priority
    // Vector Number 39, Interrupt Number 23
    // offset 0x414
    // bits 31:29
    NVIC_EN0_R |= (1<<23); // 9) Enable the correct irq IN nvic
    // offset 0x100
    //TIMER2_CTL_R |= 0X01; // 10) Enable TimerA

}

void Timer2_Oneshot(unsigned long period){
    TIMER2_CTL_R &= ~0X01; // disable timer 2a
    TIMER2_ICR_R |= TIMER_ICR_TATOCINT; // clear any pending flags
    TIMER2_TAILR_R = period - 1; // Set reload value
    TIMER2_CTL_R |= 0X01; // enable timer 2a
}

void TIMER2A_Handler(void){
    (*PeriodicTask)();
    TIMER2_ICR_R |= TIMER_ICR_TATOCINT; // clear TimerA timeout flag
}