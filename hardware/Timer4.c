#include "hardware/Timer4.h"

#define TIMER_NUM 4
#define PRIORITY 7

static void (*PeriodicTask)(void);
static unsigned long Period;

void Timer4_Init(void(*task)(void), unsigned long period){
    PeriodicTask = task;
    SYSCTL_RCGCTIMER_R |= (1 << TIMER_NUM); // 0) Activate Timer1
    // offset 0x604
    TIMER4_CTL_R &= ~0X01; // 1) disable Timer1 during setup
    // offset 0x00C
    TIMER4_CFG_R &= ~0x07; // 2) configure for 32 bit mode
    // offset 0x000
    TIMER4_TAMR_R = (TIMER4_TAMR_R & ~0x03) | 0x02; // 3) configure for periodic mode, defalut down-count settings
    // offset 0x004
    //TIMER4_TAILR_R = period - 1; // 4) reload value
    Period = period -1;
    // offset 0x028
    TIMER4_TAPR_R &= ~0xFFFF; // 5) bus clock resolution
    // offset 0x038
    TIMER4_ICR_R |= TIMER_ICR_TATOCINT; // 6) Clear TimerA timeout flag    
    // offset 0x024
    TIMER4_IMR_R |= 0x01; // 7) arm timeout interrupt
    // offset 0x018
    NVIC_PRI17_R = (NVIC_PRI17_R & ~(0x07 << 21)) | (PRIORITY << 21); // 8) priority
    // Vector Number 86, Interrupt Number 70
    // offset 0x410
    // bits 15:13
    NVIC_EN2_R |= (1<<6); // 9) Enable the correct irq IN nvic
    // offset 0x100
    // TIMER4_CTL_R |= 0X01; // 10) Enable Timer1A

}

void TIMER4A_Handler(void){
    TIMER4_ICR_R |= TIMER_ICR_TATOCINT; // clear TimerA timeout flag
    (*PeriodicTask)();
}

void Timer4_Enable(void){
    TIMER4_TAILR_R = Period; // reset reload value
    TIMER4_CTL_R |= 0X01; // 10) Enable Timer1A
}

void Timer4_Disable(void){
    TIMER4_CTL_R &= ~0X01; // 10) Enable Timer1A
}