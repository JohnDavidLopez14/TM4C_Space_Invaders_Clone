#include "ADC.h"
#include "tm4c123gh6pm.h"
#include <stdint.h>

#define PE2 (1 << 2)

// Initialize PE2/AIN1 using ADC0
// This initialization function sets up the ADC 
// Max sample rate: <=125,000 samples/second
// SS3 triggering event: software trigger
// SS3 1st sample source:  channel 1
// SS3 interrupts: enabled but not promoted to controller
void ADC0_Init(void){
  // Port E
  SYSCTL_RCGCGPIO_R |= 0x10;                           // activate Port E clock
  while            ((SYSCTL_PRGPIO_R & 0x10) == 0){};  // wait for PE

  // Port E pin selection
  GPIO_PORTE_DIR_R   &= ~PE2;  // PE2 input
  GPIO_PORTE_AFSEL_R |= PE2;   // allow alternate function select
  GPIO_PORTE_DEN_R   &= ~PE2;  // disable digital
  GPIO_PORTE_AMSEL_R |= PE2;   // turn on analog mode

// ADC clock
  SYSCTL_RCGCADC_R |= 0x01;                          // activate ADC0
  while           ((SYSCTL_PRADC_R & 0x01) == 0){};  // wait for ADC0 ready

// ADC configuration
  ADC0_PC_R      = 0x01;                               // 125K samples/sec
  ADC0_SSPRI_R   = 0x0123;                             // SS3 highest
  ADC0_ACTSS_R  &= ~0x0008;                            // disable SS3
  ADC0_EMUX_R   &= ~0xF000;                            // software trigger
  ADC0_SSMUX3_R  = (ADC0_SSMUX3_R & 0xFFFFFFF0 ) + 1;  // AIN1 (PE2)
  ADC0_SSCTL3_R  = 0x0006;                             // IE0 END0
  ADC0_ACTSS_R  |= 0x0008;                             // enable SS3
}


//------------ADC0_In------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
unsigned long ADC0_In(void){
  unsigned long result;
  ADC0_PSSI_R = 0x0008;                 // software trigger, bit 3 is SS3
  while((ADC0_RIS_R & 0x08) == 0) {};   // ADC0_RIS_R - ADC Raw Interrupt Status, this waits until previous adc conversion are complete
  // 0 - An interrupt has not occured
  // 1 - sample has completed conversion and ADCSSCTL3 bit is set, enabling the interrupt
  result = ADC0_SSFIFO3_R & 0xFFF;      // 12 bits
  ADC0_ISC_R = 0X0008;                  // Clear trigger
  return result;
}