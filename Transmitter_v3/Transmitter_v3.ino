#define DISABLE_PRINT
#include "Transmitter_v3.h"
#include "display_functions.h"
#include "EngineState.h"
#include <LoRa.h>
#include <custom_print.h>

void setup()
{
    pinMode(BATT_BUTTON_A, INPUT_PULLUP);
    pinMode(BUTTON_B, INPUT_PULLUP);
    pinMode(BUTTON_C, INPUT_PULLUP);
#ifndef DISABLE_PRINT
    Serial.begin(9600);
#endif
    LoRa.setPins(RF95_CS, RF95_RST, RF95_INT);
    LoRa.begin(915e6, 20);
    LoRa.setSpreadingFactor(11);
    LoRa.setSignalBandwidth(125e3);
    LoRa.setCodingRate4(5);
    LoRa.enableCrc();
    initializeDisplay();
    println(F("connecting..."));
    _syncState_DISCOVERY_TX(millis());
}

void loop()
{
    const uint32_t now = millis();

    uint8_t c = 0;
    if (LoRa.available())
    {
        c = (uint8_t)LoRa.read();
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
    // message processing
    if (_syncState == SyncState::SYNCING && msg == ACK)
    {
        // SYNCING state uses additional discovery period, response corresponds to the PREVIOUS discovery period
        _syncState_SYNCED_TX(_syncTimer - DISCOVERY_PERIOD);
        _requestState_IDLE();
        _activeRequest_ENQ();
    }
    else if (msg && _requestState == RequestState::WAITING_FOR_REPLY)
        processMessage(now, (msg == ZERO) ? 0 : msg);
    else if (msg)
        println(F("message ignored"));

    // button processing
    bool btnAPressed, btnCPressed;
    processButtons(now, btnAPressed, btnCPressed);

    // update current state
    switch (_state)
    {
        case TransmitterState::SEARCHING:
        {
            if (_syncState != SyncState::DISCOVERY_TX && _syncState != SyncState::DISCOVERY_RX)
                _state_SYNCING(now);
            else
                updateStatusText(STR_SEARCHING, now);
            break;
        }
        case TransmitterState::SYNCING:
        {
            if (_syncState == SyncState::DISCOVERY_TX || _syncState == SyncState::DISCOVERY_RX)
                _state_SEARCHING(now);
            else
                updateStatusText(STR_SYNCING, now);
            break;
        }
        case TransmitterState::IDLE:
        {
            if (btnAPressed && _engState == EngineState::RUNNING) // toggle auto restart
                _state_IDLE_AUTO_RESTART();
            else if (btnAPressed)
                _state_REQUESTING_START(now);
            else if (btnCPressed)
                _state_REQUESTING_STOP(now);
            break;
        }
        case TransmitterState::IDLE_AUTO_RESTART:
        {
            if (btnAPressed) // toggle auto restart
                _state_IDLE();
            else if (btnCPressed)
                _state_REQUESTING_STOP(now);
            else if (_engState == EngineState::OFF) // auto restart
                _state_REQUESTING_START(now);
            break;
        }
        case TransmitterState::REQUESTING_START:
        {
            updateStatusText(STR_REQUESTING_START, now);
            break;
        }
        case TransmitterState::REQUESTING_STOP:
        {
            updateStatusText(STR_REQUESTING_STOP, now);
            break;
        }
        case TransmitterState::REQUEST_ERROR:
        {
            if (now - _stateTimer >= MESSAGE_STATE_TIME)
            {
                if (_autoRestart)
                    _state_IDLE_AUTO_RESTART();
                else
                    _state_IDLE();
            }
            break;
        }
        case TransmitterState::STARTING:
        {
            if (_engState == EngineState::RUNNING)
                _state_START_SUCCEEDED(now);
            else if (now - _stateTimer >= STARTING_TIMEOUT)
                _state_START_FAILED(now);
            else
                updateStatusText(STR_STARTING, now);
            break;
        }
        case TransmitterState::STOPPING:
        {
            if (_engState == EngineState::OFF)
                _state_STOP_SUCCEEDED(now);
            else if (now - _stateTimer >= STOPPING_TIMEOUT)
                _state_STOP_FAILED(now);
            else
                updateStatusText(STR_STOPPING, now);
            break;
        }
        case TransmitterState::START_SUCCEEDED:
        {
            if (now - _stateTimer >= MESSAGE_STATE_TIME)
                _state_IDLE_AUTO_RESTART();
            break;
        }
        case TransmitterState::START_FAILED:
        case TransmitterState::STOP_SUCCEEDED:
        case TransmitterState::STOP_FAILED:
        {
            if (now - _stateTimer >= MESSAGE_STATE_TIME)
                _state_IDLE();
            break;
        }
    }

    // tx, update timers, refresh display, etc.
    updateSync(now);
    updateBattery(now);
    updateIcons();
    updateEngineTime(now);
    updateDisplay();
}

void decideNextRequest()
{
    switch (_state)
    {
        case TransmitterState::SEARCHING:
            break;
        case TransmitterState::SYNCING: // first ENQ reply has sucessfully been received
            if (_autoRestart)
                _state_IDLE_AUTO_RESTART();
            else
                _state_IDLE();
        case TransmitterState::IDLE:
        case TransmitterState::IDLE_AUTO_RESTART:
        case TransmitterState::REQUEST_ERROR:
        case TransmitterState::STARTING:
        case TransmitterState::STOPPING:
        case TransmitterState::START_SUCCEEDED:
        case TransmitterState::START_FAILED:
        case TransmitterState::STOP_SUCCEEDED:
        case TransmitterState::STOP_FAILED:
            _activeRequest_ENQ();
            break;
        case TransmitterState::REQUESTING_START:
            _activeRequest_STX();
            break;
        case TransmitterState::REQUESTING_STOP:
            _activeRequest_ETX();
            break;
    }
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
            decideNextRequest();
            break;
        }
        case ActiveRequest::STX:
        {
            if (msg == ACK)
                _state_STARTING(now);
            else
                _state_REQUEST_ERROR(now);
            _requestState_IDLE();
            _activeRequest_ENQ();
            break;
        }
        case ActiveRequest::ETX:
        {
            if (msg == ACK)
                _state_STOPPING(now);
            else
                _state_REQUEST_ERROR(now);
            _requestState_IDLE();
            _activeRequest_ENQ();
            break;
        }
    }
}

// non-blocking button processing
void processButtons(const uint32_t now, bool& btnA, bool& btnC)
{
    static bool btnAState = false;
    static bool btnADbncState = false;
    static uint32_t btnADbncTimer = 0;
    bool btnAPrevState = btnAState;
    debounce(btnAState, !digitalRead(BATT_BUTTON_A),
             btnADbncState, btnADbncTimer, now, BUTTON_DEBOUNCE_TIME);
    if (!btnAPrevState && btnAState) // only on button down
        btnA = true;
    else
        btnA = false;

    static bool btnCState = false;
    static bool btnCDbncState = false;
    static uint32_t btnCDbncTimer = 0;
    bool btnCPrevState = btnCState;
    debounce(btnCState, !digitalRead(BUTTON_C),
             btnCDbncState, btnCDbncTimer, now, BUTTON_DEBOUNCE_TIME);
    if (!btnCPrevState && btnCState) // only on button down
        btnC = true;
    else
        btnC = false;

    updateButtonLabels(btnAState, btnCState, _engState == EngineState::RUNNING);
}

uint8_t measureBattery()
{
    pinMode(BATT_BUTTON_A, INPUT);
    float reading = analogRead(BATT_BUTTON_A) / 1024.0f;
    pinMode(BATT_BUTTON_A, INPUT_PULLUP);
    float voltage = reading * 3.3f * 2.0f;
    // println(voltage);
    float rawPercent = (voltage - 3.2f) * 100.0f; // 3.2-4.2 V => 0-100 %
    uint8_t percent = (uint8_t)round(constrain(rawPercent, 0.0f, 100.0f));
    // println(percent, '%');
    return percent;
}

void updateBattery(const uint32_t now)
{
    if (now - _battReadTimer >= BATT_READ_PERIOD)
    {
        drawTransmitterBattery(measureBattery());
        _battReadTimer += BATT_READ_PERIOD;
    }
}
