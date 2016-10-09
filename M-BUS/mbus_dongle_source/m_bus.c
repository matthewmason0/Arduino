///////////////////////////////////////////////////////////////////
// Alpine M-Bus interface to RS232
// (c) 2003 Joerg Hohensohn, allowed for non-commercial use only
//                           (feedback appreciated)
//
// This translates the M-Bus to RS232 and back (listen and send)
// For more info see http://joerg.hohensohn.bei.t-online.de/mbus
///////////////////////////////////////////////////////////////////

#include <inttypes.h>      // common scalar types
#include <avr/io.h>        // for device register definitions
#include <avr/interrupt.h> // for interrupt enable
#include <avr/signal.h>    // for interrupt handlers
#include <avr/sleep.h>     // for power-save idle sleep
#include <avr/wdt.h>       // for watchdog, used to prevent deadlocks in interrupts
#include <avr/eeprom.h>    // EEPROM access
#include "uart.h"          // my UART "driver"
#include "m_bus.h"         // look for definitions here


static struct
{	// all the information for the receive state
	volatile enum
	{
		eWait, // waiting for packet start
		eHigh, // rising edge has been seen
		eLow,  // falling edge has been seen
	} state;
	char a4bits[4]; // received bits as chars
	uint8_t uBits; // # of received bits
} gRcv; 

static struct
{	// all the information for the transmit state
	uint8_t uBits; // # of sent bits
	uint8_t uCurrent; // processed hex nibble
	enum
	{
		eStart, // before sending a bit
		eLow0,  // short pulse of a '0' is sent
		eLow1,  // long pulse of a '1' is sent
		eEnd,   // end of sequence
	} state;
} gTrans; 


// here we go...
int main (void)
{
	wdt_reset(); // reset watchdog before enabling it
	wdt_enable(WDTO_15MS); // enable with safe interval, larger than timer1 timeout

	// init virgin EEPROM with defaults, in timer ticks (34.722 us)
	init_eeprom();

	// setup the states, only the necessary
	gRcv.state = eWait;
	gTrans.uBits = 0;
	
    init_io(); // setup timers and ports
	UartInit(eeprom_read_byte(EE_BAUDRATE));
    sei(); // enable interrupts

    for (;;)
    {
    	sleep_mode(); // idle mode by default
    	wdt_reset(); // if nothing happens, timer1 keeps waking us up
    	
    	// check if there is a command to be sent
    	if (!(TIMSK & _BV(TOIE0)) // not already sending
    		&& gRcv.state == eWait // not receiving
    		&& UartSearchReceiveBuffer('\r')) // newline in buffer
    	{	// start the transmission
			gTrans.state = eStart;
			TCCR0  = PRESCALE_256; // slow prescaling while sending
			TCNT0 = 0; // reset timer because ISR only offsets to it
			TIMSK |= _BV(TOIE0); // start the output handler with timer0
    	}
    }

    return (0);
}

// helper function for main(): setup timers and pins
void init_io (void)
{
	// timer settings: prescale 256 = 28,8kHz
	TCCR0  = PRESCALE_1; // fast prescale that would immediately generate interrups
	TCNT0 = -1; // next interrupt will be pending immediately, but is masked
	TCCR1B = _BV(ICNC1) | _BV(CTC1) | PRESCALE_256; // noise filter, reset on match, prescale
	OCR1H = 0; // we use only the lower part, but have to write this first
	OCR1L = BIT_TIMEOUT; // have to complete a bit within this time

    // enable capture and compare match interrupt for timer 1
	TIMSK = _BV(TICIE) | _BV(OCIE1A);
	
	// set my outputs, all but PB2 are for debugging / error signaling
	DDRB =  _BV(PIN_MBUS_OUT | _BV(PIN_TX_OF) | _BV(PIN_RX_OF) | _BV(PIN_RX_UF) | _BV(PIN_DEBUG)); // output and debug pins
	//TCCR1A = _BV(COM1A0); // test: toggle OC1 at compare match
}

// a convenience feature to populate the timings in eeprom with reasonable defaults
void init_eeprom (void)
{
	uint8_t i;
	static const uint8_t ee_table[] =
	{	// default initialization values, positions must match the EE_xx_TIME order
		(uint8_t)((SYSCLK / (16 * BAUDRATE)) - 1), // default baudrate as register value
		DEFAULT_ZERO_TIME - DEFAULT_TOLERANCE, // EE_MIN_ZERO_TIME
		DEFAULT_ZERO_TIME + DEFAULT_TOLERANCE, // EE_MAX_ZERO_TIME
		DEFAULT_ONE_TIME - DEFAULT_TOLERANCE, // EE_MIN_ONE_TIME
		DEFAULT_ONE_TIME + DEFAULT_TOLERANCE, // EE_MAX_ONE_TIME
		DEFAULT_BIT_TIME + DEFAULT_MIN_PAUSE, // EE_BIT_TIMEOUT
		DEFAULT_ZERO_TIME, // EE_SEND_ZERO_TIME
		DEFAULT_ONE_TIME, // EE_SEND_ONE_TIME
		DEFAULT_BIT_TIME, // EE_SEND_BIT_TIME
		DEFAULT_SPACE, // EE_SEND_SPACE
	};
	
	for (i=0; i<sizeof(ee_table) / sizeof(*ee_table); i++)
	if (eeprom_read_byte((uint8_t*)i) == 0xFF) // virgin?
	{
		eeprom_write_byte((uint8_t*)i, ee_table[i]);
		wdt_reset(); // bear in mind that writing can take up to 5ms, beware of the watchdog
	}
}

// utility function: convert a number to a hex char
char int2hex(uint8_t n)
{
	if (n<10)
		return ('0' + n);
	else if (n<16)
		return ('A' + n - 10);

	return 'X'; // out of range
}

// utility function: convert a hex char to a number
uint8_t hex2int(char c)
{
	if (c < '0')
		return 0xFF; // illegal hex digit
	else if (c <= '9')
		return (c - '0');
	else if (c < 'A')
		return 0xFF; // illegal hex digit
	else if (c <= 'F')
		return (c - 'A' + 10);
	else if (c < 'a')
		return 0xFF; // illegal hex digit
	else if (c <= 'f')
		return (c - 'a' + 10);

	return 0xFF; // default
}

// timer 0 overflow, used for sending
SIGNAL(SIG_OVERFLOW0)
{
	switch(gTrans.state)
	{
	case eStart: // start of a bit, or maybe the packet
		if (gTrans.uBits % 4 == 0)
		{	// need a new hex nibble
			uint8_t fetched;
			do
			{	// end with '\r', skip all other control codes (e.g. '\n') and non-hex chars
				PORTB |= _BV(PIN_DEBUG); // debug, indicate loop
				fetched = UartReceiveByte();
				gTrans.uCurrent = hex2int(fetched);
			} while (fetched != '\r' && gTrans.uCurrent == 0xFF);
			PORTB &= ~_BV(PIN_DEBUG); // debug end
			
			if (fetched == '\r')
			{	// done with this line (packet)
				TCNT0 -= SEND_SPACE; // space til the next transmision can start
				gTrans.state = eEnd;
				break; // exit
			}
		}
		PORTB |= _BV(PIN_MBUS_OUT); // pull the line low

		if (gTrans.uCurrent & (1 << (3 - (gTrans.uBits % 4))))
		{	// 1
			gTrans.state = eLow1;
			TCNT0 -= SEND_ONE_TIME; // next edge for the long pulse
		}
		else
		{	// 0
			gTrans.state = eLow0;
			TCNT0 -= SEND_ZERO_TIME; // next edge for the short pulse
		}
		gTrans.uBits++; // next bit
		break;

	case eLow0:
		PORTB &= ~_BV(PIN_MBUS_OUT); // release the line
		gTrans.state = eStart;
		TCNT0 -= (SEND_BIT_TIME - SEND_ZERO_TIME); // next edge
		break;
	
	case eLow1:
		PORTB &= ~_BV(PIN_MBUS_OUT); // release the line
		gTrans.state = eStart;
		TCNT0 -= (SEND_BIT_TIME - SEND_ONE_TIME); // next edge
		break;
		
	case eEnd:
		TIMSK &= ~_BV(TOIE0); // stop timer0 interrupts
		TCCR0  = PRESCALE_1; // set to "immediate interrupt" mode again
		TCNT0 = -1; // next interrupt will be pending immediately, but is masked
		gTrans.state = eStart;
		gTrans.uBits = 0; // reset the bit counter again
		break;
	}
}

// edge detection interrupt, the heart of receiving
SIGNAL(SIG_INPUT_CAPTURE1)
{	
	char outchar;
	
	switch(gRcv.state)
	{
	case eWait: // a packet is starting
		gRcv.uBits = 0;
		// no break, fall through
	case eHigh: // high phase between bits has ended, start of low pulse
		// could check the remain high time to verify bit, but won't work for the last (timed out)
		TCNT1H = 0; // reset the timer, high byte first
		TCNT1L = 0;
		TIFR |= _BV(OCF1A); // clear timeout pending, to be shure
	
		gRcv.state = eLow;
		TCCR1B |= _BV(ICES1); // capture on rising edge
		break;

	case eLow: // end of the pulldown phase of a bit
		gRcv.state = eHigh;
		TCCR1B &= ~_BV(ICES1); // capture on falling edge

		// check the low time to determine bit value
		if (ICR1L < MIN_ZERO_TIME)
			outchar = '<';
		else if (ICR1L <= MAX_ZERO_TIME)
			outchar = '0';
		else if (ICR1L < MIN_ONE_TIME)
			outchar = '=';
		else if (ICR1L <= MAX_ONE_TIME)
			outchar = '1';
		else
			outchar = '>';
/*
		// test, write length for bad bits
		if (outchar != '0' && outchar != '1')
		{
			UartTransmitByte('(');
			UartTransmitByte(int2hex(ICR1L >> 4));
			UartTransmitByte(int2hex(ICR1L & 0x0F));
			UartTransmitByte(')');
		}
*/
		//UartTransmitByte(outchar); // sending here gives binary

		// hex conversion & output, for convenience
		gRcv.a4bits[gRcv.uBits % 4] = outchar;
		gRcv.uBits++;
		if ((gRcv.uBits % 4) == 0) // 4 bits completed?
		{
			uint8_t i;
			uint8_t uHexDigit = 0;
			
			for (i=0; i<4; i++) // convert to binary
			{
				if (gRcv.a4bits[i] == '1')
					uHexDigit |= (1 << (3-i));
				else if (gRcv.a4bits[i] != '0')
					uHexDigit = 0xFF; // mark error
			}
			UartTransmitByte(int2hex(uHexDigit)); // sending here gives hex
		}

		break;
	}
}

// timeout interrupt, this terminates a received packet
SIGNAL(SIG_OUTPUT_COMPARE1A)
{
	if (gRcv.state == eWait)
		return; // timeouts don't matter

	gRcv.state = eWait; // start looking for a new packet
	TCCR1B &= ~_BV(ICES1); // capture on falling edge

	// else the packet is completed
	if ((gRcv.uBits % 4) != 0) // there should be no data waiting for output
		UartTransmitByte('X'); // but if, then mark it

	UartTransmitByte('\r'); // 0x0D
	UartTransmitByte('\n'); // 0x0A
}


