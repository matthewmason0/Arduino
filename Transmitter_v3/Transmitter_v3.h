#ifndef TRANSMITTER_V3_H
#define TRANSMITTER_V3_H

#include <LoRa.h>
#include <custom_print.h>
#include "display_functions.h"
#include "EngineState.h"
#include <check_mem.h>

static constexpr int RF95_RST      = 4;
static constexpr int RF95_INT      = 7;
static constexpr int RF95_CS       = 8;
static constexpr int BATT_BUTTON_A = 9; // A9
static constexpr int BUTTON_B      = 6;
static constexpr int BUTTON_C      = 5;

// ASCII Control Characters
static constexpr uint8_t ACK = 6  | 0x80; // request completed
static constexpr uint8_t NAK = 21 | 0x80; // request not completed
static constexpr uint8_t SOH = 1  | 0x80; // start of connection, initiates TX/RX sync
static constexpr uint8_t ENQ = 5  | 0x80; // enquiry of current values
static constexpr uint8_t STX = 2  | 0x80; // start engine request, followed by password
static constexpr uint8_t ETX = 3  | 0x80; // stop engine request
static constexpr uint8_t ZERO = 0x80; // substitute for 0 value

static constexpr uint8_t PASSWORD = 0xDB; // 11011011

static constexpr uint32_t BUTTON_DEBOUNCE_TIME = 50; // ms

static constexpr uint32_t BATT_READ_PERIOD = 60000; // ms
uint32_t _battReadTimer = 0 - BATT_READ_PERIOD;

static constexpr uint32_t DISCOVERY_PERIOD = 500; // ms
static constexpr uint32_t SYNC_PERIOD = 1000;
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
    DISCOVERY_TX,
    DISCOVERY_RX,
    SYNCING,
    SYNCED_TX,
    SYNCED_RX
};
SyncState _syncState = SyncState::DISCOVERY_TX;
void _syncState_DISCOVERY_TX(const uint32_t syncTime)
{
    LoRa.beginPacket();
    LoRa.write(SOH);
    LoRa.endPacket();
    check_mem();
    drawTxIcon();
    println(F("_syncState DISCOVERY_TX"));
    _syncTimer = syncTime;
    _syncState = SyncState::DISCOVERY_TX;
}
void _syncState_DISCOVERY_RX()
{
    LoRa.singleRx();
    println("_syncState DISCOVERY_RX");
    _syncState = SyncState::DISCOVERY_RX;
}
void _syncState_SYNCING(const uint32_t syncTime)
{
    println("_syncState SYNCING");
    _syncTimer = syncTime;
    _syncState = SyncState::SYNCING;
}
void _syncState_SYNCED_TX(const uint32_t syncTime)
{
    println(F("_syncState SYNCED_TX"));
    tx();
    println(F("sync"));
    _syncTimer = syncTime;
    _syncState = SyncState::SYNCED_TX;
}
void _syncState_SYNCED_RX()
{
    LoRa.singleRx();
    println("_syncState SYNCED_RX");
    _syncState = SyncState::SYNCED_RX;
}

bool isInDiscovery()
{
    return _syncState != SyncState::SYNCED_TX && _syncState != SyncState::SYNCED_RX;
}

uint32_t checkTimer(const uint32_t now, const uint32_t timer)
{
    if (now > timer)
        return now - timer;
    return 0;
}

void updateSync(const uint32_t now)
{
    switch (_syncState)
    {
        case SyncState::DISCOVERY_TX:
        {
            if (checkTimer(now, _syncTimer) >= DISCOVERY_PERIOD)
                _syncState_DISCOVERY_TX(_syncTimer + DISCOVERY_PERIOD);
            if (!LoRa.isTransmitting())
                _syncState_DISCOVERY_RX();
            break;
        }
        case SyncState::DISCOVERY_RX:
        {
            if (checkTimer(now, _syncTimer) >= DISCOVERY_PERIOD)
                _syncState_DISCOVERY_TX(_syncTimer + DISCOVERY_PERIOD);
            if (LoRa.validSignalDetected())
                _syncState_SYNCING(_syncTimer + DISCOVERY_PERIOD);
            break;
        }
        case SyncState::SYNCING:
        {
            if (checkTimer(now, _syncTimer) >= DISCOVERY_PERIOD)
                _syncState_DISCOVERY_TX(_syncTimer + DISCOVERY_PERIOD);
            break;
        }
        case SyncState::SYNCED_TX:
        {
            // update timer and TX at start of new period
            if (checkTimer(now, _syncTimer) >= SYNC_PERIOD)
                _syncState_SYNCED_TX(_syncTimer + SYNC_PERIOD);
            if (!LoRa.isTransmitting())
                _syncState_SYNCED_RX();
            break;
        }
        case SyncState::SYNCED_RX:
        {
            if (checkTimer(now, _syncTimer) >= SYNC_PERIOD)
                _syncState_SYNCED_TX(_syncTimer + SYNC_PERIOD);
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
    drawRetries(_retries, MAX_RETRIES, isInDiscovery());
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
            _syncState_DISCOVERY_TX(millis());
            _requestState_IDLE();
            _activeRequest_NONE();
            return;
        }
        drawRetries(_retries, MAX_RETRIES, isInDiscovery());
    }
    if (_activeRequest == ActiveRequest::NONE)
    {
        LoRa.idle();
        return;
    }
    LoRa.beginPacket();
    switch (_activeRequest)
    {
        case ActiveRequest::ENQ:
        case ActiveRequest::ENQ1:
        case ActiveRequest::ENQ2:
        {
            _activeRequest_ENQ();
            LoRa.write(ENQ);
            _requestState_WAITING_FOR_REPLY();
            break;
        }
        case ActiveRequest::STX:
        {
            LoRa.write(STX);
            LoRa.write(PASSWORD);
            _requestState_WAITING_FOR_REPLY();
            break;
        }
        case ActiveRequest::ETX:
        {
            LoRa.write(ETX);
            _requestState_WAITING_FOR_REPLY();
            break;
        }
    }
    LoRa.endPacket();
    drawTxIcon();
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

#endif // TRANSMITTER_V3_H
