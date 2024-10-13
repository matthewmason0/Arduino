#ifndef DISPLAY_FUNCTIONS_H
#define DISPLAY_FUNCTIONS_H

#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include "logo.h"
#include "background.h"
#include "icons.h"
#include "NumericMono.h"
#include "EngineFont.h"
#include "EngineState.h"
#include <custom_print.h>

//******************************************************************************
// Public Variables
//******************************************************************************

Adafruit_SH1107 display(64, 128, &Wire);

bool _refreshDisplay = false;

//******************************************************************************
// Private Variables
//******************************************************************************

static constexpr uint32_t LOGO_TIME = 800; // ms

static constexpr uint32_t ICON_FLASH_TIME = 300; // ms
uint32_t _txIconTimer = 0;
uint32_t _rxIconTimer = 0;
bool _txIconActive = false;
bool _rxIconActive = false;

static constexpr uint32_t BUTTON_HIGHLIGHT_TIME = 500; // ms
uint32_t _btnAHighlightTimer = 0;
uint32_t _btnCHighlightTimer = 0;
bool _btnAHighlightActive = false;
bool _btnCHighlightActive = false;

//******************************************************************************
// Private Functions
//******************************************************************************

void drawBattery(const uint8_t x, const uint8_t y, const int8_t batt)
{
    if (batt < 0)
    {
        // clear battery icon @ (x, y) 3x5
        display.fillRect(x, y, 3, 5, 0);
        // clear text @ (x+7, y-1) 4 chars
        display.setCursor(x+7, y-1);
        display.print(F("---%"));
        return;
    }
    // fill battery icon @ (x, y) 3x5
    uint8_t fill = (batt + 10) / 20;
    uint8_t empty = 5 - fill;
    if (empty) // don't call fillRect with a height of 0 - it breaks
        display.fillRect(x, y, 3, empty, 0);
    if (fill)
        display.fillRect(x, y+empty, 3, fill, 1);
    // update text @ (x+7, y-1) 4 chars
    display.setCursor(x+7, y-1);
    display.print(batt);
    display.write('%');
    if (batt < 100)
        display.write(' ');
    if (batt < 10)
        display.write(' ');
    _refreshDisplay = true;
}

void drawEngineTime(const uint16_t engTime)
{
    // update text @ (19, 85) 44x13
    uint8_t mins = engTime / 60;
    uint8_t secs = engTime % 60;
    display.fillRect(19, 85, 44, 13, 0);
    display.setFont(&NumericMono);
    display.setCursor(19, 85+12);
    if (mins < 10)
        display.write('0');
    display.print(mins);
    display.write(':');
    if (secs < 10)
        display.write('0');
    display.print(secs);
    display.setFont();
    _refreshDisplay = true;
}

void clearReceiverValues()
{
    drawBattery(32, 34, -1);
    // clear area (23, 65) 39x10
    display.fillRect(23, 65, 39, 10, 0);
    // draw text @ (29, 65) 27x10
    display.setFont(&EngineFont);
    display.setCursor(29, 65+9);
    display.print(F("---"));
    // clear text @ (19, 85) 44x13
    display.fillRect(19, 85, 44, 13, 0);
    display.setFont(&NumericMono);
    display.setCursor(19, 85+12);
    display.print(F("--:--"));
    display.setFont();
    _refreshDisplay = true;
}

void drawButtonALabel(const __FlashStringHelper* text, const bool highlight = false)
{
    // clear text @ (0, 111) 5 chars
    display.setCursor(0, 111);
    display.print(F("     "));
    // draw text @ (0, 111) 0-5 chars
    display.setCursor(0, 111);
    if (highlight)
        display.setTextColor(0, 1);
    display.print(text);
    display.setTextColor(1, 0);
    _refreshDisplay = true;
}

void drawButtonCLabel(const __FlashStringHelper* text, const bool highlight = false)
{
    // draw text @ (41, 111) 4 chars
    display.setCursor(41, 111);
    if (highlight)
        display.setTextColor(0, 1);
    display.print(text);
    display.setTextColor(1, 0);
    _refreshDisplay = true;
}

//******************************************************************************
// Public Functions
//******************************************************************************

void drawTransmitterBattery(const uint8_t batt)
{
    drawBattery(32, 5, batt);
}

void drawStatusText(const __FlashStringHelper* text, const uint8_t numEllipses = 0)
{
    // clear text @ (0, 48) 11 chars
    display.setCursor(0, 48);
    display.print(F("           "));
    // draw text @ (0, 48) 0-10 chars
    display.setCursor(0, 48);
    display.print(text);
    // add ellipses
    for (int i = 0; i < numEllipses; i++)
        display.drawPixel(display.getCursorX()+(i*3), 54, 1);
    _refreshDisplay = true;
}

void drawEngineIcon()
{
    display.drawBitmap(0, 63, ENG_ICON, 20, 12, 1, 0);
    _refreshDisplay = true;
}

void drawEngineAutoIcon()
{
    display.drawBitmap(0, 63, ENG_AUTO_ICON, 20, 12, 1, 0);
    _refreshDisplay = true;
}

void updateDisplay()
{
    if (_refreshDisplay)
    {
        display.display();
        _refreshDisplay = false;
    }
}

void initializeDisplay()
{
    display.begin(0x3C, true);
    display.clearDisplay();
    display.setRotation(2);
    display.drawBitmap(10, 0, LOGO, 44, 128, 1, 0);
    display.display();
    delay(LOGO_TIME);

    display.drawBitmap(0, 0, BACKGROUND, 64, 128, 1, 0);
    display.setTextSize(1);
    display.setTextColor(1, 0);
    display.setTextWrap(false);
    drawButtonALabel(F("START"));
    drawButtonCLabel(F("STOP"));
    drawStatusText(F("Searching"), 3);
    drawTransmitterBattery(-1);
    clearReceiverValues();
    drawEngineIcon();

    updateDisplay();
    // while (true);
}

void drawRetries(const uint8_t retries, const uint8_t maxRetries, const bool inDiscovery)
{
    // fill retries bar @ (31, 20) 20x3
    uint8_t fill = (maxRetries - retries) * 2;
    if (inDiscovery)
        fill = 0;
    uint8_t empty = 20 - fill;
    display.fillRect(31, 20, fill, 3, 1);
    display.fillRect(31+fill, 20, empty, 3, 0);
    _refreshDisplay = true;
}

void drawReceiverValues(const uint8_t batt, const uint16_t engTime, const EngineState engState)
{
    // println(F("batt: "), batt, F(" time: "), engTime, F(" state: "), (uint8_t)engState);
    drawBattery(32, 34, batt);
    // clear area (23, 65) 39x10
    display.fillRect(23, 65, 39, 10, 0);
    display.setFont(&EngineFont);
    switch (engState)
    {
        case EngineState::OFF:
            // draw text @ (28, 65) 29x10
            display.setCursor(28, 65+9);
            display.print(F("OFF"));
            break;
        case EngineState::STARTING:
            // draw text @ (23, 65) 39x10
            display.setCursor(23, 65+9);
            display.print(F("START"));
            break;
        case EngineState::RUNNING:
            // draw text @ (28, 65) 27x10
            display.setCursor(28, 65+9);
            display.print(F("RUN"));
            break;
    }
    display.setFont();
    drawEngineTime(engTime);
    _refreshDisplay = true;
}

void drawTxIcon()
{
    if (!_txIconActive)
    {
        _txIconActive = true;
        display.drawBitmap(8, 13, TX_ICON, 9, 5, 1, 0);
        _refreshDisplay = true;
    }
    _txIconTimer = millis();
}

void drawRxIcon()
{
    if (!_rxIconActive)
    {
        _rxIconActive = true;
        display.drawBitmap(8, 25, RX_ICON, 9, 5, 1, 0);
        _refreshDisplay = true;
    }
    _rxIconTimer = millis();
}

void updateIcons()
{
    const uint32_t now = millis();
    if (_txIconActive && (now - _txIconTimer) >= ICON_FLASH_TIME)
    {
        _txIconActive = false;
        display.fillRect(8, 13, 9, 5, 0);
        _refreshDisplay = true;
    }
    if (_rxIconActive && (now - _rxIconTimer) >= ICON_FLASH_TIME)
    {
        _rxIconActive = false;
        display.fillRect(8, 25, 9, 5, 0);
        _refreshDisplay = true;
    }
}

void updateButtonLabels(const bool btnAState, const bool btnCState)
{
    const uint32_t now = millis();

    if (btnAState && !_btnAHighlightActive)
        drawButtonALabel(F("START"), true);
    if (btnAState) // regardless of _btnAHighlightActive
    {
        _btnAHighlightActive = true;
        _btnAHighlightTimer = now;
    }
    else if (_btnAHighlightActive && (now - _btnAHighlightTimer) >= BUTTON_HIGHLIGHT_TIME)
    {
        _btnAHighlightActive = false;
        drawButtonALabel(F("START"), false);
    }

    if (btnCState && !_btnCHighlightActive)
        drawButtonCLabel(F("STOP"), true);
    if (btnCState) // regardless of _btnCHighlightActive
    {
        _btnCHighlightActive = true;
        _btnCHighlightTimer = now;
    }
    else if (_btnCHighlightActive && (now - _btnCHighlightTimer) >= BUTTON_HIGHLIGHT_TIME)
    {
        _btnCHighlightActive = false;
        drawButtonCLabel(F("STOP"), false);
    }
}

#endif // DISPLAY_FUNCTIONS_H
