#ifndef RECEIVER_V3_H
#define RECEIVER_V3_H

#include <LoRa.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <custom_print.h>
#include "EngineState.h"

static constexpr int RF95_RST = 4;
static constexpr int RF95_INT = 7;
static constexpr int RF95_CS  = 8;
static constexpr int START     = 5;
static constexpr int IGNITION  = 6;
static constexpr int BATT      = A9;

// ASCII Control Characters
static constexpr uint8_t ACK = 6  | 0x80; // request completed
static constexpr uint8_t NAK = 21 | 0x80; // request not completed
static constexpr uint8_t SOH = 1  | 0x80; // start of connection, initiates TX/RX sync
static constexpr uint8_t ENQ = 5  | 0x80; // enquiry of current values
static constexpr uint8_t STX = 2  | 0x80; // start engine request, followed by password
static constexpr uint8_t ETX = 3  | 0x80; // stop engine request
static constexpr uint8_t ZERO = 0x80; // substitute for 0 value

static constexpr uint8_t PASSWORD = 0xDB; // 11011011

static constexpr uint32_t IGN_DEBOUNCE_TIME = 500; // ms

static constexpr uint32_t CONNECTION_TIMEOUT = 10000; // ms
uint32_t _connectionTimer = 0;
uint32_t _sessionTimer = 0;

static constexpr uint32_t SYNC_PERIOD = 3000; // ms
static constexpr uint32_t SYNC_OFFSET = SYNC_PERIOD / 2;
static constexpr uint32_t SYNC_DELAY = SYNC_OFFSET + 420; // 1284 ms TX delay
uint32_t _syncTimer = 0;

static constexpr size_t TX_BUFFER_LEN = 16;
char _txBuffer[TX_BUFFER_LEN] = "";

// transmitter sends SOH, starts syncTimer
// receiver receivees SOH, starts syncTimer, starts connectionTimer
// receiver waits for transmit window, sends ACK
// transmitter receives ACK
// connection is made.

static constexpr uint32_t AWAKE_TIME = 2500; // ms
uint32_t _awakeTimer = 0;

static constexpr uint32_t START_TIMEOUT = 15000; // ms
uint32_t _startTimer = 0;

uint32_t _engTimer = 0;
uint32_t _engTotTime = 0;

EngineState _engState = EngineState::OFF;
void _engState_OFF(const uint32_t now)
{
    println("_engState OFF");
    if (_engState == EngineState::RUNNING)
    {
        _engTotTime += (now - _engTimer);
        println("_engTotTime: ", _engTotTime);
    }
    _engState = EngineState::OFF;
}
void _engState_STARTING(const uint32_t now)
{
    println("_engState STARTING");
    _startTimer = now;
    _engState = EngineState::STARTING;
}
void _engState_RUNNING(const uint32_t now)
{
    println("_engState RUNNING");
    _engTimer = now;
    _engState = EngineState::RUNNING;
}

// void printTxBuffer()
// {
//     for (size_t i = 0; _txBuffer[i]; i++)
//         print((uint8_t)((_txBuffer[i] == (char)ZERO) ? 0 : _txBuffer[i]), ' ');
//     println();
// }

enum class ReceiverState
{
    NOT_CONNECTED,
    CONNECTED,
    SLEEP
};
ReceiverState _state = ReceiverState::NOT_CONNECTED;
void _state_NOT_CONNECTED()
{
    println("_state NOT_CONNECTED");
    _state = ReceiverState::NOT_CONNECTED;
}
void _state_CONNECTED(const uint32_t now)
{
    if (_state != ReceiverState::CONNECTED)
        println("_state CONNECTED");
    _connectionTimer = now;
    _state = ReceiverState::CONNECTED;
}
void _state_SLEEP()
{
    println("_state SLEEP");
    _engTotTime = 0;
    _state = ReceiverState::SLEEP;
}

enum class SyncState
{
    IDLE,
    SYNCED
};
SyncState _syncState = SyncState::IDLE;
void _syncState_IDLE()
{
    if (_syncState != SyncState::IDLE)
        println("_syncState IDLE");
    _txBuffer[0] = 0;
    _syncState = SyncState::IDLE;
}
void _syncState_SYNCED(const uint32_t syncTime)
{
    if (_txBuffer[0])
    {
        // size_t i;
        // for (i = 0; _txBuffer[i]; i++)
        //     if (_txBuffer[i] == (char)ZERO)
        //         _txBuffer[i] = 0;
        // rf95.send((uint8_t*)_txBuffer, i);
        // print("sent ");
        // printTxBuffer();
        _txBuffer[0] = 0;
    }
    println("sync");
    _syncTimer = syncTime;
    _syncState = SyncState::SYNCED;
}

enum class RequestState
{
    NONE, // no request ongoing
    ENQ,  // enquiry - send current values
    STX,  // start engine
    ETX   // stop engine
};
RequestState _requestState = RequestState::NONE;
void _requestState_NONE()
{
    println("_requestState NONE");
    _requestState = RequestState::NONE;
}
void _requestState_ENQ()
{
    println("_requestState ENQ");
    _requestState = RequestState::ENQ;
}
void _requestState_STX()
{
    println("_requestState STX");
    _requestState = RequestState::STX;
}
void _requestState_ETX()
{
    println("_requestState ETX");
    _requestState = RequestState::ETX;
}

void adc_off()
{
    ADCSRA &= ~(1<<ADEN);
    PRR0 |= (1<<PRADC);
}

void adc_on()
{
    PRR0 &= ~(1<<PRADC);
    ADCSRA |= (1<<ADEN);
}

void setup_sleep()
{
    // shut down TWI
    PRR0 = (1<<PRTWI);
    // shut down analog comparator
    ACSR &= ~(1<<ACBG) & ~(1<<ACIE); // disable voltage ref & interrupt
    ACSR |= (1<<ACD);
    // shut down ADC
    adc_off();
    // setup watchdog timer
    cli();
    wdt_reset();
    WDTCSR |= (1<<WDCE) | (1<<WDE); // enable changes
    WDTCSR = (1<<WDIE) | (0<<WDE) | (1<<WDP3); // ~4s interrupt
    sei();
}

ISR(WDT_vect) {}

void sleep()
{
    // hc12_sleep();
    uint8_t prr0_orig = PRR0;
    uint8_t prr1_orig = PRR1;
    cli();
    wdt_reset();
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    PRR0 = 0xFF;
    PRR1 = 0xFF;
    sei();
    sleep_cpu();
    sleep_disable();
    PRR0 = prr0_orig;
    PRR1 = prr1_orig;
    // hc12_wake();
}

void debounce(bool& var, const bool in,
              bool& state, uint32_t& timer,
              const uint32_t now, const uint32_t duration)
{
    if (in == var)
        state = false;
    else if (!state) // && (in != var), on transition
    {
        state = true;
        timer = now;
    }
    if (state && ((now - timer) >= duration))
    {
        state = false;
        var = !var;
    }
}

// append c to _txBuffer
void tx(const char c)
{
    size_t i;
    for (i = 0; _txBuffer[i]; i++);
    if ((i + 1) < TX_BUFFER_LEN)
    {
        _txBuffer[i] = c ? c : ZERO;
        _txBuffer[i + 1] = 0;
    }
    // print("_txBuffer: ");
    // printTxBuffer();
}

void updateSync(const uint32_t now)
{
    if (_syncState == SyncState::SYNCED)
    {
        // update timer and TX at start of new period
        if ((now - _syncTimer) >= SYNC_PERIOD)
            _syncState_SYNCED(_syncTimer + SYNC_PERIOD);
    }
}

uint16_t getEngineTime(const uint32_t now)
{
    uint32_t engTimeMs = _engTotTime;
    if (_engState == EngineState::RUNNING)
        engTimeMs += (now - _engTimer);
    uint16_t engTimeS = (uint16_t)(engTimeMs / 1000);
    // saturate at 12 bits (4095 seconds)
    if (engTimeS > 0xFFF)
        engTimeS = 0xFFF;
    return engTimeS;
}

bool step(uint32_t now, uint8_t msg);
void processMessage(uint32_t now, uint8_t msg);
void updateEngine(uint32_t now);
uint8_t measureBattery();

#endif // RECEIVER_V3_H