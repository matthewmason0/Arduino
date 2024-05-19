#include "Transmitter_v2.h"
#include "display_functions.h"
#include "EngineState.h"
#include <check_mem.h>

void setup()
{
    pinMode(SET, OUTPUT);
    pinMode(BUTTON_A, INPUT_PULLUP);
    pinMode(BUTTON_B, INPUT_PULLUP);
    pinMode(BUTTON_C, INPUT_PULLUP);
    digitalWrite(SET, 1);
    hc12.begin(1200);
    Serial.begin(9600);
    check_mem();

    delay(250); // wait for the OLED to power up
    initializeDisplay();

    println(F("connecting..."));
}

void loop()
{
    const uint32_t now = millis();

    uint8_t c = 0;
    if (hc12.available())
    {
        c = (uint8_t)hc12.read();
        if (!c)
            c = ZERO;
        drawRxIcon();
        println(F("Received: "), c);
    }

    step(now, c);

    const uint32_t dt = millis() - now;
    if (dt > 10)
        println(F("slow loop: "), dt, F("ms"));
}

// runs once per loop (non-blocking)
void step(const uint32_t now, const uint8_t msg)
{
    if (_syncState == SyncState::DISCOVERY && msg == ACK)
    {
        _syncState_SYNCED(_syncTimer - DISCOVERY_PERIOD); // response corresponds to the PREVIOUS discovery TX
        updateSync(now);
        _requestState_IDLE();
        _activeRequest_ENQ();
    }
    else if (msg && _requestState == RequestState::WAITING_FOR_REPLY)
        processMessage(now, (msg == ZERO) ? 0 : msg);
    else if (msg)
        println(F("message ignored"));
    // switch (_state)
    // {
    //     case TransmitterState::CONNECTING:
    //     {
    //         updateSync(now);
    //         break;
    //     }
    //     case TransmitterState::CONNECTED:
    //     {
    //         updateSync(now);
    //         break;
    //     }
    // }
    updateSync(now);
    updateIcons();
    updateEngineTime(now);
    updateDisplay();
}

// non-blocking message processing
void processMessage(const uint32_t now, const uint8_t msg)
{
    static uint8_t rxBatt = 0;
    static uint16_t engTime = 0;
    switch (_activeRequest)
    {
        case ActiveRequest::NONE:
            break;
        case ActiveRequest::ENQ:
        {
            rxBatt = msg;
            _activeRequest_ENQ1();
            break;
        }
        case ActiveRequest::ENQ1:
        {
            // msg -> lower 7 bits of engTime
            engTime = msg;
            _activeRequest_ENQ2();
            break;
        }
        case ActiveRequest::ENQ2:
        {
            // lower 5 bits of msg -> upper 5 bits of engTime (12 bits total)
            engTime |= (uint16_t)(msg & 0x1F) << 7;
            _engTime = engTime;
            _lastEngTimeUpdate = now;
            // upper 2 bits of msg -> engState
            _engState = (EngineState)(msg >> 5);
            drawReceiverValues(rxBatt, _engTime, _engState);
            _requestState_IDLE();
            _activeRequest_ENQ();
            break;
        }
        case ActiveRequest::STX:
        {
            if (msg == ACK)
            {
                ; // success
            }
            else
            {
                ; // failure
            }
            _requestState_IDLE();
            _activeRequest_ENQ();
            break;
        }
        case ActiveRequest::ETX:
        {
            if (msg == ACK)
            {
                ; // success
            }
            else
            {
                ; // failure
            }
            _requestState_IDLE();
            _activeRequest_ENQ();
            break;
        }
    }
}

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
