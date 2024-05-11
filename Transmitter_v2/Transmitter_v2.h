#ifndef TRANSMITTER_V2_H
#define TRANSMITTER_V2_H

#include <SoftwareSerial.h>
#include <custom_print.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <check_mem.h>

// uint8_t* heapptr;
// uint8_t* stackptr;

Adafruit_SH1107 display(64, 128, &Wire);

SoftwareSerial hc12(2, 3); // RX, TX

static constexpr int SET = 4;
static constexpr int BUTTON_A = 9;
static constexpr int BUTTON_B = 6;
static constexpr int BUTTON_C = 5;
static constexpr int BATT = A6;

// ASCII Control Characters
static constexpr uint8_t ACK = 6  | 0x80; // request completed
static constexpr uint8_t NAK = 21 | 0x80; // request not completed
static constexpr uint8_t SOH = 1  | 0x80; // start of connection, initiates TX/RX sync
static constexpr uint8_t ENQ = 5  | 0x80; // enquiry of current values
static constexpr uint8_t STX = 2  | 0x80; // start engine request, followed by password
static constexpr uint8_t ETX = 3  | 0x80; // stop engine request
static constexpr uint8_t ZERO = 0x80; // substitute for 0 value

static constexpr uint8_t PASSWORD = 0xDB; // 11011011

// Text Symbols
static constexpr uint8_t COLON   = ':';
static constexpr uint8_t DASH    = '-';
static constexpr uint8_t PERCENT = '%';
static constexpr uint8_t SPACE   = ' ';

static constexpr uint32_t DISCOVERY_PERIOD = 2000; // ms
static constexpr uint32_t SYNC_PERIOD = 3000;
uint32_t _syncTimer = 0;

// static constexpr size_t TX_BUFFER_LEN = 16;
// char _txBuffer[TX_BUFFER_LEN] = "";

static constexpr uint8_t MAX_RETRIES = 10;
uint8_t _retries = 0;

uint16_t _engTime = 0;
uint32_t _lastEngTimeUpdate = 0;

enum class EngineState
{
    OFF = 0,
    STARTING = 1,
    RUNNING = 2
};
EngineState _engState = EngineState::OFF;

static constexpr uint32_t ICON_FLASH_TIME = 500; // ms
uint32_t _txIconTimer = 0;
uint32_t _rxIconTimer = 0;

void displayRetries();

void drawBattery(uint8_t x, uint8_t y, int8_t batt);
void drawEngineTime();

void clearReceiverValues();
void displayReceiverValues(uint8_t batt, uint16_t engTime, EngineState engState);
void updateEngineTime(uint32_t now);

void displayTxIcon();
void displayRxIcon();
void updateIcons();

// need to prevent STX->other, ETX->other while request active

enum class ActiveRequest
{
    NONE, // no request ongoing
    ENQ,  // enquiry - send current values
    ENQ1,
    ENQ2,
    STX,  // start engine
    ETX   // stop engine
};
ActiveRequest _activeRequest = ActiveRequest::NONE;
void _activeRequest_NONE()
{
    println(F("_activeRequest NONE"));
    _activeRequest = ActiveRequest::NONE;
}
void _activeRequest_ENQ()
{
    println(F("_activeRequest ENQ"));
    _activeRequest = ActiveRequest::ENQ;
}
void _activeRequest_ENQ1()
{
    println(F("_activeRequest ENQ1"));
    _activeRequest = ActiveRequest::ENQ1;
}
void _activeRequest_ENQ2()
{
    println(F("_activeRequest ENQ2"));
    _activeRequest = ActiveRequest::ENQ2;
}
void _activeRequest_STX()
{
    println(F("_activeRequest STX"));
    _activeRequest = ActiveRequest::STX;
}
void _activeRequest_ETX()
{
    println(F("_activeRequest ETX"));
    _activeRequest = ActiveRequest::ETX;
}

enum class RequestState
{
    IDLE,
    WAITING_FOR_REPLY
};
RequestState _requestState = RequestState::IDLE;
void _requestState_IDLE()
{
    println(F("_requestState IDLE"));
    _retries = 0;
    displayRetries();
    _requestState = RequestState::IDLE;
}
void _requestState_WAITING_FOR_REPLY()
{
    println(F("_requestState WAITING_FOR_REPLY"));
    _requestState = RequestState::WAITING_FOR_REPLY;
}

// enum class TransmitterState
// {
//     CONNECTING,
//     CONNECTED
// };
// TransmitterState _state = TransmitterState::CONNECTING;
// void _state_CONNECTING()
// {
//     println(F("_state CONNECTING"));
//     _state = TransmitterState::CONNECTING;
// }
// void _state_CONNECTED()
// {
//     println(F("_state CONNECTED"));
//     _activeRequest_ENQ();
//     _state = TransmitterState::CONNECTED;
// }

void _syncState_DISCOVERY(const uint32_t syncTime);

void tx()
{
    if (_requestState == RequestState::WAITING_FOR_REPLY)
    {
        _retries++;
        if (_retries > MAX_RETRIES)
        {
            _requestState_IDLE();
            _activeRequest_NONE();
            // _state_CONNECTING();
            _syncState_DISCOVERY(millis());
            return;
        }
        displayRetries();
    }
    if (_activeRequest != ActiveRequest::NONE)
        displayTxIcon();
    switch (_activeRequest)
    {
        case ActiveRequest::NONE:
            break;
        case ActiveRequest::ENQ:
        case ActiveRequest::ENQ1:
        case ActiveRequest::ENQ2:
        {
            _activeRequest_ENQ();
            hc12.write(ENQ);
            _requestState_WAITING_FOR_REPLY();
            break;
        }
        case ActiveRequest::STX:
        {
            hc12.write(STX);
            hc12.write(PASSWORD);
            _requestState_WAITING_FOR_REPLY();
            break;
        }
        case ActiveRequest::ETX:
        {
            hc12.write(ETX);
            _requestState_WAITING_FOR_REPLY();
            break;
        }
    }
}

enum class SyncState
{
    DISCOVERY,
    SYNCED
};
SyncState _syncState = SyncState::DISCOVERY;
void _syncState_DISCOVERY(const uint32_t syncTime)
{
    check_mem();
    hc12.write(SOH);
    displayTxIcon();
    println(F("_syncState DISCOVERY"));
    // _txBuffer[0] = 0;
    _syncTimer = syncTime;
    _syncState = SyncState::DISCOVERY;
}
void _syncState_SYNCED(const uint32_t syncTime)
{
    if (_syncState != SyncState::SYNCED)
    {
        println(F("_syncState SYNCED"));
        _activeRequest_ENQ();
    }
    // if (_txBuffer[0])
    // {
    //     hc12.write(_txBuffer);
    //     print(F("sent "));
    //     printTxBuffer();
    //     _txBuffer[0] = 0;
    // }
    tx();
    println(F("sync"));
    _syncTimer = syncTime;
    _syncState = SyncState::SYNCED;
}

void updateSync(const uint32_t now)
{
    switch (_syncState)
    {
        case SyncState::DISCOVERY:
        {
            // update timer and TX at start of new discovery period
            if ((now - _syncTimer) >= DISCOVERY_PERIOD)
                _syncState_DISCOVERY(_syncTimer + DISCOVERY_PERIOD);
            break;
        }
        case SyncState::SYNCED:
        {
            // update timer and TX at start of new period
            if ((now - _syncTimer) >= SYNC_PERIOD)
                _syncState_SYNCED(_syncTimer + SYNC_PERIOD);
            break;
        }
    }
}

#endif // TRANSMITTER_V2_H
