// AVR306: Using the AVR UART in C
// Routines for interrupt controlled UART
// Last modified: 03-02-19
// Modified by: AR, Hoh

// includes
#include <inttypes.h>   // common scalar types
#include <avr/io.h>     // for device register definitions
#include <avr/signal.h> // for interrupt handlers
#include "m_bus.h"      // for error pin definitions
#include "uart.h"

// UART buffer size definitions
#define UART_RX_BUFFER_SIZE 64     // 2,4,8,16,32,64,128 or 256 bytes
#define UART_TX_BUFFER_SIZE 16


#define UART_RX_BUFFER_MASK (UART_RX_BUFFER_SIZE - 1)
#if (UART_RX_BUFFER_SIZE & UART_RX_BUFFER_MASK)
	#error RX buffer size is not a power of 2
#endif

#define UART_TX_BUFFER_MASK (UART_TX_BUFFER_SIZE - 1)
#if (UART_TX_BUFFER_SIZE & UART_TX_BUFFER_MASK)
	#error TX buffer size is not a power of 2
#endif


// static variables
static uint8_t UART_RxBuf[UART_RX_BUFFER_SIZE];
static volatile uint8_t UART_RxHead;
static volatile uint8_t UART_RxTail;
static uint8_t UART_TxBuf[UART_TX_BUFFER_SIZE];
static volatile uint8_t UART_TxHead;
static volatile uint8_t UART_TxTail;


// initialize UART
void UartInit(uint8_t baudrate)
{
	uint8_t null;

	UBRR = baudrate; // set the baud rate
	// enable UART receiver and transmitter, and receive interrupt
	UCR = _BV(RXCIE) | _BV(RXEN) | _BV(TXEN);

	null = 0; // flush receive buffer

	UART_RxTail = null;
	UART_RxHead = null;
	UART_TxTail = null;
	UART_TxHead = null;
}

// receive interrupt handler
SIGNAL(SIG_UART_RECV)
{
	uint8_t data;
	uint8_t tmphead;

	data = UDR; // read the received data

	// calculate buffer index
	tmphead = (UART_RxHead + 1) & UART_RX_BUFFER_MASK;
	UART_RxHead = tmphead; // store new index

	if ( tmphead == UART_RxTail )
	{	// ERROR! Receive buffer overflow
		PORTB |= _BV(PIN_RX_OF); // set debug bit for indication
	}
	
	UART_RxBuf[tmphead] = data; // store received data in buffer
}

// transmit interrupt handler
SIGNAL(SIG_UART_DATA)
{
	uint8_t tmptail;

	if (UART_TxHead != UART_TxTail) // check if all data is transmitted
	{
		// calculate buffer index
		tmptail = (UART_TxTail + 1) & UART_TX_BUFFER_MASK;
		UART_TxTail = tmptail; // store new index
	
		UDR = UART_TxBuf[tmptail]; // start transmition
	}
	else
	{
		UCR &= ~_BV(UDRIE); // disable UDRE interrupt
	}
}

// Read and write functions
uint8_t UartReceiveByte(void)
{
	uint8_t tmptail;
	
	while (UART_RxHead == UART_RxTail) // wait for incoming data
	{	// underrun condition: we have to wait, fatal if called within interrupt
		PORTB |= _BV(PIN_RX_UF); // set debug bit for indication
	}

	tmptail = (UART_RxTail + 1) & UART_RX_BUFFER_MASK; // calculate buffer index
	
	UART_RxTail = tmptail; // store new index
	return UART_RxBuf[tmptail]; // return data
}

void UartTransmitByte(uint8_t data)
{
	uint8_t tmphead;
	// calculate buffer index
	tmphead = (UART_TxHead + 1) & UART_TX_BUFFER_MASK; // wait for free space in buffer
	while (tmphead == UART_TxTail)
	{	// overflow condition: we have to wait, fatal if called within interrupt
		PORTB |= _BV(PIN_TX_OF); // set debug bit for indication
	}

	UART_TxBuf[tmphead] = data; // store data in buffer
	UART_TxHead = tmphead; // store new index

	UCR |= _BV(UDRIE); // enable UDRE interrupt
}

// return 0 (FALSE) if the receive buffer is empty
uint8_t UartDataInReceiveBuffer(void)
{
	return (UART_RxHead - UART_RxTail) & UART_RX_BUFFER_MASK;
}

// search for a specific char in buffer, return position (1-based, 0 if not found)
uint8_t UartSearchReceiveBuffer(uint8_t key)
{
	uint8_t i;
	uint8_t level = UartDataInReceiveBuffer();

	for (i=1; i<=level; i++)
	{
		if (UART_RxBuf[(UART_RxTail + i) & UART_RX_BUFFER_MASK] == key)
			return i; // found
	}
	return 0; // not found
}
