// Timer0.c
// Runs on LM4F120/TM4C123
// Use TIMER0 in 32-bit periodic mode to request interrupts at a periodic rate
// Daniel Valvano
// March 20, 2014

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2013
  Program 7.5, example 7.6

 Copyright 2013 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remaains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

#include "tm4c123gh6pm.h"
#include <stdint.h>

void (*PeriodicTask)(void);   // function pointer, void return type, void arguments

// ***************** Timer0_Init ****************
// Activate TIMER0 interrupts to run user task periodically
// Inputs:  task is a pointer to a user function
//          period in units (1/clockfreq)
// Outputs: none
void Timer0_Init(void(*task)(void), unsigned long period){
  SYSCTL_RCGCTIMER_R |= 0x01;   // 0) activate TIMER0
  // 16/23-Bit General-Purpose Timer Run Mode Clock Gating Control (RCGCTIMER), offset 0x604
  // setting bit 0 to 1 -> "Enable and provide a clock to 16/32 bit general-purpose timer module 0 in Run mode"

  PeriodicTask = task;          // user function
  TIMER0_CTL_R = 0x00000000;    // 1) disable TIMER0A during setup
  // GPTM Control (GPTMCTL), offset 0x00C
  // This register is used alongside the GPTMCFG and GMTMnMR registerst o fine-tune the timer configuration, and to enable other features such as timer stall and the output trigger
  // The output trigger can be used to initiaite transfers on the DAC module
  // 0 - Timer A is disabled

  TIMER0_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  // GPTM Configuration (GPTMCFG), offset 0x000
  // This register configures the global operation of the GPTM module
  // The value written to this register determines whether the GPTM is in 32- or 64-bit mode (concatenated timers) or in 16- or 32-bit mode (individual, split timers)
  // 0x0 - For a 16/32-bit timer, this value selects the 32-bit timer configuration

  TIMER0_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  // GPTM Timer A Mode (GPTMTAMR), offset 0x004
  // bits 1:0 - TAMR
  // 0x2 -> Periodic mode

  TIMER0_TAILR_R = period-1;    // 4) reload value
  // GPTM Timer A Interval Load (GPTMTAILR), offset 0x028
  // 31:0 - GPTM Timer A Interval Load Register

  TIMER0_TAPR_R &= 0xFFFF0000;            // 5) bus clock resolution
  // GPTM Timer A Prescale (GPTMTAPR), offset 0x038
  // set this to 0 to disable prescaling

  TIMER0_ICR_R = 0x00000001;    // 6) clear TIMER0A timeout flag
  // GPTM Interrupt Clear, offset 0x024
  // 0 - TATOCINT
  // Writing a 1 to this bit clears the TATORIS bit in the GPTMRIS register and the TATOMIS bit in the GPTMMIS register

  TIMER0_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  // GPTM Interrupt Mask (GPTMIMR), offset 0x018
  // This register allows softwar eto enable/disable GPTM controller-level interrupts
  // Setting a bit enable the corresponding interrupt, while clearing a bit disables it
  // bit 0 - GPTM Timer A Capture Mode Match Interrupt Mask
  // 1 - Interrupt is enabled

  NVIC_PRI4_R = (NVIC_PRI4_R&0x00FFFFFF)|0x80000000; // 8) priority 4
  // Interrupt 16-19 Priority (PRI4), offset 0x410
  // interrupts enabled in the main program after all devices initialized
  // vector number 35, interrupt number 19
  // table 2-9
  // Vector Number 34, Interrupt Number 19, 16-32-Bit Timer 0A
  // bits 31:29 -> 0b100 -> 4, priority 4

  NVIC_EN0_R = 1<<19;           // 9) enable IRQ 19 in NVIC
  // Interrupt 0-31 Set Enable (EN0), offset 0x100
  // nebales interrupt number 19
  // Table 2-9 -> interrupt 19 - Timer 0A

  // TIMER0_CTL_R = 0x00000001;    // 10) enable TIMER0A
  // GPTM Control (GPTMCTL), offset 0x00C
  // This register is used alongside the GPTMCFG and GMTMTnMR registers to fine-tune the timer configuration, and to enable other features such as timer stall and the output trigger
  // The output trigger can be used to initiate transfers on the ADC module
  // 1 - Timer A is enabled and begins counting or the capture logic is enabled based on the GPTMCFG register
}

void Timer0A_Handler(void){
  TIMER0_ICR_R = TIMER_ICR_TATOCINT;// acknowledge TIMER0A timeout
  // TIMER_ICR_TATOCINT - 0x00000001
  // Writing a 1 to this bit clears the TATORIS bit in the GPTMRIS register and the TATOMIS bit in the GPTMMIS register
  // TIMER_ICR_TATOCINT is just a macro for that register clear

  (*PeriodicTask)();                // execute user task
}
