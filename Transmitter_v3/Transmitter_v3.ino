#include "Transmitter_v3.h"
#include "display_functions.h"
#include "EngineState.h"
#include <check_mem.h>

void setup()
{
    pinMode(BATT_BUTTON_A, INPUT_PULLUP);
    pinMode(BUTTON_B, INPUT_PULLUP);
    pinMode(BUTTON_C, INPUT_PULLUP);
    Serial.begin(9600);
    LoRa.setPins(RF95_CS, RF95_RST, RF95_INT);
    LoRa.begin(915e6, 20);
    LoRa.setSpreadingFactor(11);
    LoRa.setSignalBandwidth(125e3);
    LoRa.setCodingRate4(5);
    LoRa.enableCrc();
    check_mem();

    delay(250); // wait for the OLED to power up
    initializeDisplay();

    println(F("connecting..."));
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
    if (_syncState == SyncState::SYNCING && msg == ACK)
    {
        // SYNCING state uses additional discovery period, response corresponds to the PREVIOUS discovery period
        _syncState_SYNCED_TX(_syncTimer - DISCOVERY_PERIOD + SYNC_PERIOD);
        // updateSync(now);
        _requestState_IDLE();
        _activeRequest_ENQ();
    }
    else if (msg && _requestState == RequestState::WAITING_FOR_REPLY)
        processMessage(now, (msg == ZERO) ? 0 : msg);
    else if (msg)
        println(F("message ignored"));

    processButtons(now);
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
    debounce(startBtnState, !digitalRead(BATT_BUTTON_A),
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
    if ((now - _battReadTimer) >= BATT_READ_PERIOD)
    {
        drawTransmitterBattery(measureBattery());
        _battReadTimer += BATT_READ_PERIOD;
    }
}
