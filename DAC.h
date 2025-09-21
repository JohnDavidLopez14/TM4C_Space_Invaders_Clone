#ifndef __DAC__
#define __DAC__

// Initializes port B, PB0:3 as dac output pins
void DAC_Init();

// outputs the argument as PB0:3
void DAC_Out(unsigned long data);

#endif