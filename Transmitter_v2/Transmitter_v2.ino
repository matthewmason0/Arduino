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

    processButtons(now);

    switch (_state)
    {
        case TransmitterState::SEARCHING:
        {
            break;
        }
        case TransmitterState::IDLE:
        {
            break;
        }
        case TransmitterState::IDLE_AUTO_RESTART:
        {
            break;
        }
        case TransmitterState::REQUESTING_START:
        {
            break;
        }
        case TransmitterState::REQUESTING_STOP:
        {
            break;
        }
        case TransmitterState::REQUEST_ERROR:
        {
            break;
        }
        case TransmitterState::STARTING:
        {
            break;
        }
        case TransmitterState::STOPPING:
        {
            break;
        }
        case TransmitterState::START_SUCCEEDED:
        {
            break;
        }
        case TransmitterState::STOP_SUCCEEDED:
        {
            break;
        }
        case TransmitterState::START_FAILED:
        {
            break;
        }
        case TransmitterState::STOP_FAILED:
        {
            break;
        }
    }

    updateSync(now);
    updateBattery(now);
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

// non-blocking button processing
void processButtons(const uint32_t now)
{
    static bool startBtnState = false;
    static bool startBtnDbncState = false;
    static uint32_t startBtnDbncTimer = 0;
    bool startBtnPrevState = startBtnState;
    debounce(startBtnState, !digitalRead(BUTTON_A),
             startBtnDbncState, startBtnDbncTimer, now, BUTTON_DEBOUNCE_TIME);
    if (!startBtnPrevState && startBtnState) // on button press
    {
        ; // action
    }

    static bool stopBtnState = false;
    static bool stopBtnDbncState = false;
    static uint32_t stopBtnDbncTimer = 0;
    bool stopBtnPrevState = stopBtnState;
    debounce(stopBtnState, !digitalRead(BUTTON_C),
             stopBtnDbncState, stopBtnDbncTimer, now, BUTTON_DEBOUNCE_TIME);
    if (!stopBtnPrevState && stopBtnState) // on button press
    {
        ; // action
    }
}

uint8_t measureBattery()
{
    float reading = analogRead(BATT) / 1024.0f;
    float voltage = reading * 3.3f * 2.0f;
    // println(voltage);
    float rawPercent = (voltage - 3.2f) * 100.0f; // 3.2-4.2 V => 0-100 %
    uint8_t percent = (uint8_t)round(constrain(rawPercent, 0.0f, 100.0f));
    // println(percent, '%');
    return percent;
}

void updateBattery(const uint32_t now)
{
    if ((now - _battReadTimer) >= BATT_READ_PERIOD)
    {
        drawTransmitterBattery(measureBattery());
        _battReadTimer += BATT_READ_PERIOD;
    }
}
