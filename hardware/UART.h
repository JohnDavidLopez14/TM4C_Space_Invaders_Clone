// Header Guards
#ifndef __UART_H__
#define __UART_H__
#include "tm4c123gh6pm.h"
#include <stdint.h>

// standard ASCII symbols
#define CR   0x0D
#define LF   0x0A
#define BS   0x08
#define ESC  0x1B
#define SP   0x20
#define DEL  0x7F

void UART_Init(void);

char UART_InChar(void);

char UART_InCharNonBlocking(void);

void UART_OutChar(char data);

unsigned long UART_InUDec(void);

void UART_OutString(char buffer[]);

void UART_ConvertUDec(unsigned long n);

void UART_OutUDec(unsigned long n);

#endif