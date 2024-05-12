#include "Transmitter_v2.h"
#include "logo.h"
#include "background.h"
#include "icons.h"
#include "NumericMono.h"
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
    uint8_t val = display.begin(0x3C, true);
    Serial.println(val);
    display.clearDisplay();
    display.setRotation(2);
    display.drawBitmap(10, 0, LOGO, 44, 128, 1);
    display.display();
    check_mem();
    delay(800);

    display.clearDisplay();
    display.drawBitmap(0, 0, BACKGROUND, 64, 128, 1);
    display.setTextSize(1);
    display.setTextColor(1, 0);
    display.setTextWrap(false);
    display.setCursor(0, 111);
    display.print(F("START"));
    display.setCursor(41, 111);
    display.print(F("STOP"));
    display.setCursor(0, 48);
    display.print(F("CONNECTING"));
    display.drawPixel(60, 54, 1);
    display.drawPixel(63, 54, 1);
    drawBattery(32, 5, 69);
    // display.setCursor(39, 4);
    // display.print(F("69%"));
    // display.setCursor(39, 33);
    // display.print(F("42%"));
    // display.setCursor(27, 66);
    // display.print(F("START"));
    display.display();

    clearReceiverValues();

    println(F("connecting..."));
    // _syncState_DISCOVERY(millis());
    // while (true)
    // {
    //     while (!hc12.available())
    //     {
    //         updateSync(millis());
    //     }
    //     uint8_t c = hc12.read();
    //     if (c == ACK)
    //         break;
    //     else
    //         println(F("failed to connect"));
    // }
    // // response corresponds to the PREVIOUS discovery TX
    // _syncState_SYNCED(_syncTimer - DISCOVERY_PERIOD);
    // _state_CONNECTED();
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
        displayRxIcon();
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
            displayReceiverValues(rxBatt, _engTime, _engState);
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

void displayRetries()
{
    // fill retries bar @ (31, 20) 20x3
    uint8_t fill = (MAX_RETRIES - _retries) * 2;
    if (_syncState == SyncState::DISCOVERY)
        fill = 0;
    uint8_t empty = 20 - fill;
    display.fillRect(31, 20, fill, 3, 1);
    display.fillRect(31+fill, 20, empty, 3, 0);
    display.display();
}

void drawBattery(const uint8_t x, const uint8_t y, const int8_t batt)
{
    if (batt < 0)
    {
        // clear battery icon @ (x, y) 3x5
        display.fillRect(x, y, 3, 5, 0);
        // clear text @ (x+7, y-1) 4 chars
        display.setCursor(x+7, y-1);
        display.write(DASH);
        display.write(DASH);
        display.write(DASH);
        display.write(PERCENT);
        return;
    }
    // fill battery icon @ (x, y) 3x5
    uint8_t fill = (batt + 10) / 20;
    uint8_t empty = 5 - fill;
    display.fillRect(x, y, 3, empty, 0);
    display.fillRect(x, y+empty, 3, fill, 1);
    // update text @ (x+7, y-1) 4 chars
    display.setCursor(x+7, y-1);
    display.print(batt);
    display.write(PERCENT);
    if (batt < 100)
        display.write(SPACE);
    if (batt < 10)
        display.write(SPACE);
}

void drawEngineTime()
{
    // update text @ (19, 85) 44x13
    uint8_t mins = _engTime / 60;
    uint8_t secs = _engTime % 60;
    display.fillRect(19, 85, 44, 13, 0);
    display.setFont(&NumericMono);
    display.setCursor(19, 85+12);
    if (mins < 10)
        display.write('0');
    display.print(mins);
    display.write(COLON);
    if (secs < 10)
        display.write('0');
    display.print(secs);
    display.setFont();
}

void clearReceiverValues()
{
    drawBattery(32, 34, -1);
    // clear text @ (27, 66) 5 chars
    display.setCursor(27, 66);
    display.write(SPACE);
    display.write(DASH);
    display.write(DASH);
    display.write(DASH);
    display.write(SPACE);
    // clear text @ (19, 85) 44x13
    display.fillRect(19, 85, 44, 13, 0);
    display.setFont(&NumericMono);
    display.setCursor(19, 85+12);
    display.write(DASH);
    display.write(DASH);
    display.write(COLON);
    display.write(DASH);
    display.write(DASH);
    display.setFont();
    display.display();
}

void displayReceiverValues(const uint8_t batt, const uint16_t engTime, const EngineState engState)
{
    check_mem();
    println(F("batt: "), batt, F(" time: "), engTime, F(" state: "), (uint8_t)engState);
    drawBattery(32, 34, batt);
    // update text @ (27, 66) 5 chars
    display.setCursor(27, 66);
    switch (engState)
    {
        case EngineState::OFF:
            display.print(F(" OFF "));
            break;
        case EngineState::STARTING:
            display.print(F("START"));
            break;
        case EngineState::RUNNING:
            display.print(F(" RUN "));
            break;
    }
    drawEngineTime();
    display.display();
}

void updateEngineTime(const uint32_t now)
{
    if (_engState == EngineState::RUNNING)
    {
        if ((now - _lastEngTimeUpdate) >= 1000)
        {
            _engTime += 1;
            _lastEngTimeUpdate = now;
            drawEngineTime();
            display.display();
        }
    }
}

void displayTxIcon()
{
    if (!_txIconActive)
    {
        display.drawBitmap(8, 13, TX_ICON, 9, 5, 1);
        display.display();
        _txIconActive = true;
    }
    _txIconTimer = millis();
}

void displayRxIcon()
{
    if (!_rxIconActive)
    {
        display.drawBitmap(8, 25, RX_ICON, 9, 5, 1);
        display.display();
        _rxIconActive = true;
    }
    _rxIconTimer = millis();
}

void updateIcons()
{
    const uint32_t now = millis();
    if (_txIconActive && (now - _txIconTimer) >= ICON_FLASH_TIME)
    {
        display.fillRect(8, 13, 9, 5, 0);
        display.display();
        _txIconActive = false;
    }
    if (_rxIconActive && (now - _rxIconTimer) >= ICON_FLASH_TIME)
    {
        display.fillRect(8, 25, 9, 5, 0);
        display.display();
        _rxIconActive = false;
    }
}
