#include "Transmitter_v2.h"
#include "logo.h"
#include "background.h"
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
    // Serial.println(F("test"));
    check_mem();
    // Serial.print(F("heap ")); Serial.println((uint16_t)heapptr);
    // Serial.print(F("stack ")); Serial.println((uint16_t)stackptr);

    delay(250); // wait for the OLED to power up
    uint8_t val = display.begin(0x3C, true);
    Serial.println(val);
    display.clearDisplay();
    display.setRotation(2);
    display.drawBitmap(10, 0, splashScreen, 44, 128, 1);
    display.display();
    check_mem();
    // Serial.print(F("heap ")); Serial.println((uint16_t)heapptr);
    // Serial.print(F("stack ")); Serial.println((uint16_t)stackptr);
    // Serial.println(F("x"));
    delay(800);
    // Serial.println(F("y"));

    display.clearDisplay();
    display.drawBitmap(0, 0, background, 64, 128, 1);
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
    display.setCursor(39, 4);
    display.print(F("69%"));
    display.setCursor(39, 33);
    display.print(F("42%"));
    display.setCursor(27, 66);
    display.print(F("START"));
    display.display();

    println(F("connecting..."));
    _syncState_DISCOVERY(millis());
    while (true)
    {
        while (!hc12.available())
        {
            updateSync(millis());
        }
        uint8_t c = hc12.read();
        if (c == ACK)
            break;
        else
            println(F("failed to connect"));
    }
    // response corresponds to the PREVIOUS discovery TX
    _syncState_SYNCED(_syncTimer - DISCOVERY_PERIOD);
    _state_CONNECTED();
}

void loop()
{
    const uint32_t now = millis();

    uint8_t c = 0;
    if (hc12.available())
    {
        c = (uint8_t)hc12.read();
        c = c ? c : ZERO;
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
    if (msg && _requestState == RequestState::WAITING_FOR_REPLY)
        processMessage(now, msg);
    else if (msg)
        println(F("message ignored"));
    switch (_state)
    {
        case TransmitterState::CONNECTING:
        {
            updateSync(now);
            break;
        }
        case TransmitterState::CONNECTED:
        {
            updateSync(now);
            break;
        }
    }
}

// non-blocking message processing
void processMessage(const uint32_t now, const uint8_t msg)
{
    static uint8_t rxBatt = 0;
    static uint16_t engTime = 0;
    static uint8_t engState = 0;
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
            engTime = msg;
            _activeRequest_ENQ2();
            break;
        }
        case ActiveRequest::ENQ2:
        {
            // lower 5 bits of msg -> upper 5 bits of engTime (12 bits total)
            engTime |= (uint16_t)(msg & 0x1F) << 7;
            // upper 2 bits of msg -> engState
            engState = msg >> 5;
            displayReceiverValues(rxBatt, engTime, engState);
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
    ; // draw retries bar
}

void displayReceiverValues(const uint8_t batt, const uint16_t engTime, const uint8_t engState)
{
    // draw values
    check_mem();
    println(F("batt: "), batt, F(" time: "), engTime, F(" state: "), engState);
}

void displayTxIcon()
{
    ; // set timer for 
}

void displayRxIcon()
{
    ;
}

void updateIcons()
{
    // check timers
}
