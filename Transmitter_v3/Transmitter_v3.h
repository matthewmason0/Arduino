#ifndef TRANSMITTER_V3_H
#define TRANSMITTER_V3_H

#include <LoRa.h>
#include <custom_print.h>
#include "display_functions.h"
#include "EngineState.h"

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
static constexpr uint32_t SYNC_PERIOD = 1500;
uint32_t _syncTimer = 0;

static constexpr uint8_t MAX_RETRIES = 10;
uint8_t _retries = 0;

static constexpr uint32_t ELLIPSES_PERIOD = 300;
uint32_t _ellipsesTimer = 0;
uint8_t _ellipses = 0;

bool _autoRestart = false;

uint16_t _engTime = 0;
uint32_t _lastEngTimeUpdate = 0;
EngineState _engState = EngineState::OFF;

void updateEngineTime(const uint32_t now)
{
    if (_engState == EngineState::RUNNING)
    {
        if (now - _lastEngTimeUpdate >= 1000)
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
    _syncTimer = syncTime;
    println(millis(), ", ", syncTime);
    _syncState = SyncState::DISCOVERY_TX;
    println(F("_syncState DISCOVERY_TX"));
    LoRa.beginPacket();
    LoRa.write(SOH);
    LoRa.endPacket();
    drawTxIcon();
}
void _syncState_DISCOVERY_RX()
{
    println(millis());
    _syncState = SyncState::DISCOVERY_RX;
    println("_syncState DISCOVERY_RX");
    LoRa.singleRx();
}
void _syncState_SYNCING(const uint32_t syncTime)
{
    _syncTimer = syncTime;
    println(millis());
    _syncState = SyncState::SYNCING;
    println("_syncState SYNCING");
}
void _syncState_SYNCED_TX(const uint32_t syncTime)
{
    _syncTimer = syncTime;
    println(millis(), ", ", syncTime);
    _syncState = SyncState::SYNCED_TX;
    println(F("_syncState SYNCED_TX"));
    tx();
}
void _syncState_SYNCED_RX()
{
    println(millis());
    _syncState = SyncState::SYNCED_RX;
    println("_syncState SYNCED_RX");
    LoRa.singleRx();
}

static constexpr uint32_t MAX_TIMER_DURATION = 0xFFFF; // ms

uint32_t checkTimer(const uint32_t now, const uint32_t timer)
{
    const uint32_t elapsed = now - timer;
    if (elapsed > MAX_TIMER_DURATION)
        return 0; // timer is in the future
    return elapsed;
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

bool isInDiscovery()
{
    return _syncState != SyncState::SYNCED_TX && _syncState != SyncState::SYNCED_RX;
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
    if (state && (now - timer >= duration))
    {
        state = false;
        var = !var;
    }
}

// status strings  10 chars max "**********"
#define STR_SEARCHING         F("Searching")
#define STR_SYNCING           F("Syncing")
#define STR_IDLE              F("Connected")
#define STR_IDLE_AUTO_RESTART F("Connected")
#define STR_REQUESTING_START  F("Req Start")
#define STR_REQUESTING_STOP   F("Req Stop")
#define STR_REQUEST_ERROR     F("Req Error!")
#define STR_STARTING          F("Starting")
#define STR_STOPPING          F("Stopping")
#define STR_START_SUCCEEDED   F("Success!")
#define STR_START_FAILED      F("Failed!")
#define STR_STOP_SUCCEEDED    F("Success!")
#define STR_STOP_FAILED       F("Failed!")

enum class TransmitterState
{
    SEARCHING,         // discovery
    SYNCING,           // waiting for first ENQ reply
    IDLE,              // connected, _autoRestart FALSE
    IDLE_AUTO_RESTART, // connected, _autoRestart TRUE
    REQUESTING_START,
    REQUESTING_STOP,
    REQUEST_ERROR,
    STARTING,
    STOPPING,
    START_SUCCEEDED,
    START_FAILED,
    STOP_SUCCEEDED,
    STOP_FAILED
};
TransmitterState _state = TransmitterState::SEARCHING;
void _state_SEARCHING(const uint32_t now)
{
    println(F("_state SEARCHING"));
    _state = TransmitterState::SEARCHING;
    _ellipsesTimer = now;
    _ellipses = 0;
    drawStatusText(STR_SEARCHING, _ellipses);
}
void _state_SYNCING(const uint32_t now)
{
    println(F("_state SYNCING"));
    _state = TransmitterState::SYNCING;
    _ellipsesTimer = now;
    _ellipses = 0;
    drawStatusText(STR_SYNCING, _ellipses);
}
void _state_IDLE()
{
    println(F("_state IDLE"));
    _state = TransmitterState::IDLE;
    drawStatusText(STR_IDLE, 1);
}
void _state_IDLE_AUTO_RESTART()
{
    println(F("_state IDLE_AUTO_RESTART"));
    _state = TransmitterState::IDLE_AUTO_RESTART;
    drawStatusText(STR_IDLE_AUTO_RESTART, 1);
}
void _state_REQUESTING_START()
{
    println(F("_state REQUESTING_START"));
    _state = TransmitterState::REQUESTING_START;
}
void _state_REQUESTING_STOP()
{
    println(F("_state REQUESTING_STOP"));
    _state = TransmitterState::REQUESTING_STOP;
}
void _state_REQUEST_ERROR()
{
    println(F("_state REQUEST_ERROR"));
    _state = TransmitterState::REQUEST_ERROR;
}
void _state_STARTING()
{
    println(F("_state STARTING"));
    _state = TransmitterState::STARTING;
}
void _state_STOPPING()
{
    println(F("_state STOPPING"));
    _state = TransmitterState::STOPPING;
}
void _state_START_SUCCEEDED()
{
    println(F("_state START_SUCCEEDED"));
    _state = TransmitterState::START_SUCCEEDED;
}
void _state_START_FAILED()
{
    println(F("_state START_FAILED"));
    _state = TransmitterState::START_FAILED;
}
void _state_STOP_SUCCEEDED()
{
    println(F("_state STOP_SUCCEEDED"));
    _state = TransmitterState::STOP_SUCCEEDED;
}
void _state_STOP_FAILED()
{
    println(F("_state STOP_FAILED"));
    _state = TransmitterState::STOP_FAILED;
}

void updateStatusText(const __FlashStringHelper* text, const uint32_t now)
{
    if (now - _ellipsesTimer >= ELLIPSES_PERIOD)
    {
        drawStatusText(text, _ellipses);
        _ellipses = (_ellipses + 1) % 4;
        _ellipsesTimer = now;
    }
}

#endif // TRANSMITTER_V3_H
