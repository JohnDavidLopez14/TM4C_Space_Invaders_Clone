// Files
#include "UART.h"

// Constants
#define PA0 0X01
#define PA1 0x02

  // UART0 initialization for 80Mhz PLL, PA0, PA1a 
  // Input: none
  // Output: none
void UART_Init(void){
    // System Clocks
    SYSCTL_RCGC1_R |= SYSCTL_RCGC1_UART0;  // Activate UART 0
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOA;  // Actiavte GPIO A port, this is connceted to the USB icdi

    // UART Configuration
    UART0_CTL_R  &= ~UART_CTL_UARTEN;                   // Turn of UART for initialization
    UART0_IBRD_R = 43;                                  // integer baud rate divisior (assuming 80 Mhz)
    UART0_FBRD_R = 26;                                  // Fractional baud rate divisor
    UART0_LCRH_R = (UART_LCRH_WLEN_8 | UART_LCRH_FEN);  // 8 bits a frame, enable FIFO
    UART0_CTL_R  |= UART_CTL_UARTEN;                    // ENABLE uart

    // GPIOA, PA0 and PA1 are connected to UART 0
    GPIO_PORTA_AFSEL_R |= (PA0 | PA1);                         // Alternate function on PA0, PA1
    GPIO_PORTA_DEN_R   |= (PA0 | PA1);                         // Enable digital on PA0, PA1
    GPIO_PORTA_PCTL_R  |= (GPIO_PORTA_PCTL_R & ~0xFF) | 0x11;  //See table 14-1 for UART Pin Mux / Pin Assignment
    GPIO_PORTA_AMSEL_R &= (~(PA0 | PA1));                      // Disable digital on PA0, PA1
    GPIO_PORTA_DIR_R   &= ~PA0;                                // PA0 (UART0 RX) as an input pin
}

  // Block until a char is received on UART0 and return
  // Input: none
  // Output: ASCII code for key received
char UART_InChar(void){
    while((UART0_FR_R & UART_FR_RXFE) !=0);  // while the receive FIFO is empty, block
    return ((char)(UART0_DR_R&0xFF));
}

  // Get the oldest serial port input and return
  // Input: none
  // Output: ASCII code for key if received, 0 if no character
char UART_InCharNonBlocking(void){
    if((UART0_FR_R & UART_FR_RXFE) != 0){           // if receive FIFO is empty
        return((char)(UART0_DR_R&0XFF));   // return 8 bits in the FIFO
    } else {
        return 0;
    }
}

   // Send char data to transmit FIFO, will block until the transmit FIFO is not full
   // Input: char
   // Output: none
void UART_OutChar(char data){
  while     (UART0_FR_R & UART_FR_TXFF);  // while transmit FIFO is full, block
  UART0_DR_R = data;                      // send 8 bits to transmit FIFO
}

  // accepts ASCII input, will convert into an unsigned decimal format
  // Input: none
  // Output: 32-bit unsigned number
unsigned long UART_InUDec(void){
  unsigned long number = 0, length = 0;
  char character;
  character = UART_InChar();
  while(character != CR){ // while carriage return is not entered, ie <enter>
    if((character >= '0') && (character <= '9')){
      number = 10 * number + (character - '0');
      length++;
      UART_OutChar(character);
    }
    else if ((character == BS) && length){
      number /= 10;
      length--;
      UART_OutChar(character);
    }
    character = UART_InChar();
  }
  return number;
}

  // sends a string over UART
  // Input: pointer to a null terminated character array
  // Output: none
void UART_OutString(char *buffer){
  while (*buffer){ // this will stop once we dereference a pointer to a null character
    UART_OutChar(*buffer);
    buffer++;
  } 
}