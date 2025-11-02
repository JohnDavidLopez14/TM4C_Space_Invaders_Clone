#include "hardware/Timer5.h"

#define TIMER_NUM 5
#define PRIORITY 7

static void (*PeriodicTask)(void);

void Timer5_Init(void(*task)(void), unsigned long period){
    PeriodicTask = task;
    SYSCTL_RCGCTIMER_R |= (1 << TIMER_NUM); // 0) Activate Timer1
    // offset 0x604
    TIMER5_CTL_R &= ~0X01; // 1) disable Timer1 during setup
    // offset 0x00C
    TIMER5_CFG_R &= ~0x07; // 2) configure for 32 bit mode
    // offset 0x000
    TIMER5_TAMR_R = (TIMER5_TAMR_R & ~0x03) | 0x02; // 3) configure for periodic mode, defalut down-count settings
    // offset 0x004
    TIMER5_TAILR_R = period - 1; // 4) reload value
    // offset 0x028
    TIMER5_TAPR_R &= ~0xFFFF; // 5) bus clock resolution
    // offset 0x038
    TIMER5_ICR_R |= TIMER_ICR_TATOCINT; // 6) Clear TimerA timeout flag    
    // offset 0x024
    TIMER5_IMR_R |= 0x01; // 7) arm timeout interrupt
    // offset 0x018
    NVIC_PRI26_R = (NVIC_PRI26_R & ~(0x07 << 5)) | (PRIORITY << 5); // 8) priority
    // Vector Number 120, Interrupt Number 104
    // offset 0x410
    // bits 15:13
    NVIC_EN3_R |= (1<<8); // 9) Enable the correct irq IN nvic
    // offset 0x100
    TIMER5_CTL_R |= 0X01; // 10) Enable Timer1A

}

void TIMER5A_Handler(void){
    TIMER5_ICR_R |= TIMER_ICR_TATOCINT; // clear TimerA timeout flag
    (*PeriodicTask)();
}