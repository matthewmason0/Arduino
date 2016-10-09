#ifndef _M_BUS_H
#define _M_BUS_H

// a place for platform dependent indirect definitions
#if defined(__AVR_AT90S2313__) // the only one used so far
#define PIN_MBUS_OUT PB2 // MBus output pin (active = pull low)
#define PIN_TX_OF    PB6 // TX overflow error, send buffer full
#define PIN_RX_OF    PB5 // RX overflow error, receive buffer full
#define PIN_RX_UF    PB4 // RX underflow error, no char available
#define PIN_DEBUG    PB7 // debug condition
#elif defined(__AVR_ATmega8__) // trying the Mega8 now
#define PIN_MBUS_OUT PB3 // MBus output pin (active = pull low)
#define PIN_TX_OF    PB1 // Really strange: PIN_MBUS_OUT becomes weak
#define PIN_RX_OF    PB1 //  if any other b-port pin than 3 used for it,
#define PIN_RX_UF    PB1 //  or any other than 1 is used additionally.
#define PIN_DEBUG    PB1 //  So I map all error pins to 1.
#define CTC1 WGM12
#define OCR1H OCR1AH
#define OCR1L OCR1AL
#define TICIE TICIE1
#define UBRR UBRRL
#define UCR UCSRB
#else
#error "Code is not prepared for that MCU!"
#endif

#define SYSCLK 7372800L	// system clock in Hertz
#define BAUDRATE 115200UL // default UART baudrate

#define PRESCALE_256 _BV(CS02) // prescaler for normal operation
#define PRESCALE_1   _BV(CS00) // minimum prescaler to force interrupt

// in timer ticks (SYSCLK / prescaler = 34.722 us)
#define DEFAULT_ZERO_TIME 20 // low pulse 0.6 ms: "0" bit
#define DEFAULT_ONE_TIME  54 // low pulse 1.9 ms: "1" bit
#define DEFAULT_BIT_TIME  89 // time for a full bit cycle
#define DEFAULT_TOLERANCE 15 // the allowed "jitter" on reception
#define DEFAULT_MIN_PAUSE 10 // timeout for packet completion
#define DEFAULT_SPACE     50 // pause before sending new packet

// eeprom locations of constants, adapt init_eeprom() if changing these!
#define EE_BAUDRATE       ((uint8_t*)0)
#define EE_MIN_ZERO_TIME  ((uint8_t*)1)
#define EE_MAX_ZERO_TIME  ((uint8_t*)2)
#define EE_MIN_ONE_TIME   ((uint8_t*)3)
#define EE_MAX_ONE_TIME   ((uint8_t*)4)
#define EE_BIT_TIMEOUT    ((uint8_t*)5)
#define EE_SEND_ZERO_TIME ((uint8_t*)6)
#define EE_SEND_ONE_TIME  ((uint8_t*)7)
#define EE_SEND_BIT_TIME  ((uint8_t*)8)
#define EE_SEND_SPACE     ((uint8_t*)9)

/*
#define MIN_ZERO_TIME  (DEFAULT_ZERO_TIME - DEFAULT_TOLERANCE)
#define MAX_ZERO_TIME  (DEFAULT_ZERO_TIME + DEFAULT_TOLERANCE)
#define MIN_ONE_TIME   (DEFAULT_ONE_TIME - DEFAULT_TOLERANCE)
#define MAX_ONE_TIME   (DEFAULT_ONE_TIME + DEFAULT_TOLERANCE)
#define BIT_TIMEOUT    (DEFAULT_BIT_TIME + DEFAULT_MIN_PAUSE)
#define SEND_ZERO_TIME (DEFAULT_ZERO_TIME)
#define SEND_ONE_TIME  (DEFAULT_ONE_TIME)
#define SEND_BIT_TIME  (DEFAULT_BIT_TIME)
#define SEND_SPACE     (DEFAULT_SPACE)
*/
#define MIN_ZERO_TIME  eeprom_read_byte(EE_MIN_ZERO_TIME)
#define MAX_ZERO_TIME  eeprom_read_byte(EE_MAX_ZERO_TIME)
#define MIN_ONE_TIME   eeprom_read_byte(EE_MIN_ONE_TIME)
#define MAX_ONE_TIME   eeprom_read_byte(EE_MAX_ONE_TIME)
#define BIT_TIMEOUT    eeprom_read_byte(EE_BIT_TIMEOUT)
#define SEND_ZERO_TIME eeprom_read_byte(EE_SEND_ZERO_TIME)
#define SEND_ONE_TIME  eeprom_read_byte(EE_SEND_ONE_TIME)
#define SEND_BIT_TIME  eeprom_read_byte(EE_SEND_BIT_TIME)
#define SEND_SPACE     eeprom_read_byte(EE_SEND_SPACE)


// prototypes
int main (void); // here we go...
void init_io (void); // helper function for main(): setup timers and pins
void init_eeprom (void); // a convenience feature to populate the timings in eeprom with reasonable defaults
char int2hex(uint8_t n); // utility function: convert a number to a hex char
uint8_t hex2int(char c); // utility function: convert a hex char to a number

// interrupt handlers
SIGNAL(SIG_OVERFLOW0); // timer 0 overflow, used for sending
SIGNAL(SIG_INPUT_CAPTURE1); // edge detection interrupt, the heart of receiving
SIGNAL(SIG_OUTPUT_COMPARE1A); // timeout interrupt, this terminates a received packet

#endif
