#include "hardware/Timer1.h"

#define TIMER_NUM 1
#define PRIORITY 5

static void (*PeriodicTask)(void);

void Timer1_Init(void(*task)(void), unsigned long period){
    PeriodicTask = task;
    SYSCTL_RCGCTIMER_R |= (1 << TIMER_NUM); // 0) Activate Timer1
    // offset 0x604
    TIMER1_CTL_R &= ~0X01; // 1) disable Timer1 during setup
    // offset 0x00C
    TIMER1_CFG_R &= ~0x07; // 2) configure for 32 bit mode
    // offset 0x000
    TIMER1_TAMR_R = (TIMER1_TAMR_R & ~0x03) | 0x02; // 3) configure for periodic mode, defalut down-count settings
    // offset 0x004
    TIMER1_TAILR_R = period - 1; // 4) reload value
    // offset 0x028
    TIMER1_TAPR_R &= ~0xFFFF; // 5) bus clock resolution
    // offset 0x038
    TIMER1_ICR_R |= TIMER_ICR_TATOCINT; // 6) Clear TimerA timeout flag    
    // offset 0x024
    TIMER1_IMR_R |= 0x01; // 7) arm timeout interrupt
    // offset 0x018
    NVIC_PRI5_R = (NVIC_PRI5_R & ~(0x07 << 13)) | (PRIORITY << 13); // 8) priority
    // Vector Number 37, Interrupt Number 21
    // offset 0x410
    // bits 15:13
    NVIC_EN0_R |= (1<<21); // 9) Enable the correct irq IN nvic
    // offset 0x100
    TIMER1_CTL_R |= 0X01; // 10) Enable Timer1A

}

void TIMER1A_Handler(void){
    TIMER1_ICR_R |= TIMER_ICR_TATOCINT; // clear TimerA timeout flag
    (*PeriodicTask)();
}