#ifndef _UART_H
#define _UART_H

// prototypes

void UartInit(uint8_t baudrate); // init the driver
uint8_t UartReceiveByte(void); // receive a byte
void UartTransmitByte(uint8_t data); // transmit a byte
uint8_t UartDataInReceiveBuffer(void); // return buffer fullness
uint8_t UartSearchReceiveBuffer(uint8_t key); // search for a specific char in buffer

#endif
