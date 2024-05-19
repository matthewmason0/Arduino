#ifndef TRANSMITTER_V2_H
#define TRANSMITTER_V2_H

#include <SoftwareSerial.h>
#include <custom_print.h>
#include "display_functions.h"
#include "EngineState.h"
#include <check_mem.h>

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

static constexpr uint32_t DISCOVERY_PERIOD = 2000; // ms
static constexpr uint32_t SYNC_PERIOD = 3000;
uint32_t _syncTimer = 0;

static constexpr uint8_t MAX_RETRIES = 10;
uint8_t _retries = 0;

uint16_t _engTime = 0;
uint32_t _lastEngTimeUpdate = 0;
EngineState _engState = EngineState::OFF;

void updateEngineTime(const uint32_t now)
{
    if (_engState == EngineState::RUNNING)
    {
        if ((now - _lastEngTimeUpdate) >= 1000)
        {
            _engTime += 1;
            _lastEngTimeUpdate = now;
            drawEngineTime(_engTime);
        }
    }
}

void tx();

enum class SyncState
{
    DISCOVERY,
    SYNCED
};
SyncState _syncState = SyncState::DISCOVERY;
void _syncState_DISCOVERY(const uint32_t syncTime)
{
    hc12.write(SOH);
    check_mem();
    drawTxIcon();
    if (_syncState != SyncState::DISCOVERY)
    {
        println(F("_syncState DISCOVERY"));
        _syncState = SyncState::DISCOVERY;
    }
    _syncTimer = syncTime;
}
void _syncState_SYNCED(const uint32_t syncTime)
{
    if (_syncState != SyncState::SYNCED)
    {
        println(F("_syncState SYNCED"));
        _syncState = SyncState::SYNCED;
    }
    tx();
    println(F("sync"));
    _syncTimer = syncTime;
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
    drawRetries(_retries, MAX_RETRIES, _syncState == SyncState::DISCOVERY);
    _requestState = RequestState::IDLE;
}
void _requestState_WAITING_FOR_REPLY()
{
    println(F("_requestState WAITING_FOR_REPLY"));
    _requestState = RequestState::WAITING_FOR_REPLY;
}

// runs once per tx window, when synced
void tx()
{
    if (_requestState == RequestState::WAITING_FOR_REPLY)
    {
        _retries++;
        if (_retries > MAX_RETRIES)
        {
            // _state_CONNECTING();
            _syncState_DISCOVERY(millis());
            _requestState_IDLE();
            _activeRequest_NONE();
            return;
        }
        drawRetries(_retries, MAX_RETRIES, _syncState == SyncState::DISCOVERY);
    }
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
    if (_activeRequest != ActiveRequest::NONE)
        drawTxIcon();
}

/* messages:
10 chars
..........
Connecting
CONNECTING
CONNECTED
REQUESTING
REQ RECVD
REQ ERROR
SUCCESS!
START FAIL
STOP FAIL
*/

#endif // TRANSMITTER_V2_H
