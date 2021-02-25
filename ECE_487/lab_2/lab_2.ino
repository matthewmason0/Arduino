/*****************************************************************************
 * lab_2
 * Created by Matt Mason,
 * CWID 11800439
******************************************************************************/

// switch pin & debounce threshold
static constexpr int SW = 53;
static constexpr int DEBOUNCE_THRESH = 100; // ~100 ms

// LED pins
static constexpr int LED = 13;
static constexpr int EXT_LED = 52;

// seven-segment display pins & character mappings
#define SEVEN_SEG PORTC
#define SEVEN_SEG_DDR DDRC
static constexpr byte CHARACTERS[] = {
    B00000011, // 0
    B10011111, // 1
    B00100101, // 2
    B00001101, // 3
    B10011001, // 4
    B01001001, // 5
    B01000001, // 6
    B00011111, // 7
    B00000001, // 8
    B00001001, // 9
    B00010000, // A.
    B11000000, // b.
    B01100010, // C.
    B10000100, // d.
    B01100000, // E.
    B01110000  // F.
};

/*****************************************************************************
 * Function:        readSwitch
 * Parameters:      int count – the current count of switch state changes
 * Return value:    int — an updated count of switch state changes
 * Purpose:         Read the switch input, debounce it, and calculate a new
 *                  value for the count variable accordingly
******************************************************************************/
int readSwitch(const int count)
{
    // initialize debounce variables
    static bool switchState = digitalRead(SW);
    static int readCount = 0;

    // debounce logic
    if (switchState != digitalRead(SW))
    {
        if (readCount == DEBOUNCE_THRESH) // switch has been flipped
        {
            // reset read counter
            readCount = 0;
            // toggle saved switch state
            switchState = !switchState;
            // calculate new value for switch state-change counter
            return (count + 1) % 16;
        }
        else
            // increment read counter on successive reads != saved switch state
            readCount++;
    }
    else
        // reset read counter when the read value == saved switch state
        readCount = 0;

    // no change to switch state-change counter
    return count;
}

void setup()
{
    // configure switch pin as input w/ internal pullup
    pinMode(SW, INPUT_PULLUP);

    // configure LED pins as outputs
    pinMode(LED, OUTPUT);
    pinMode(EXT_LED, OUTPUT);

    // configure all seven-seg. pins as outputs
    SEVEN_SEG_DDR = 0xFF;

    // open serial connection
    Serial.begin(9600);
    Serial.println("lab_2 by Matt Mason");

    // print initial switch state-change count to serial terminal
    Serial.println("count = (decimal) 0   (hex) 0");
}

void loop()
{
    // initialize switch state-change counter
    static int count = 0;
    // initialize LED blinking state to initial switch value
    static bool blinking = digitalRead(SW);
    // get current time
    unsigned long currentTime = millis();
    // initialize LED toggle timepoint to 1 second from now
    static unsigned long toggleTime = currentTime + 1000;

    // read switch and get updated count
    int newCount = readSwitch(count);

    // if switch state-change count has changed
    if (newCount != count)
    {
        // update switch state-change counter
        count = newCount;
        // print new value to serial terminal
        Serial.print("count = (decimal) ");
        Serial.print(count);
        Serial.print((count < 10) ? "   (hex) " : "  (hex) ");
        Serial.println(count, HEX);
        // toggle LED blinking state
        blinking = !blinking;
        // set LED toggle timepoint to 1 second from now
        toggleTime = currentTime + 1000;
    }

    // when switch is on, blink LEDs according to specified pattern
    if (blinking)
    {
        // if the toggle cycle has finished, update LED toggle timepoint
        if (currentTime > (toggleTime + 1000))
            toggleTime = currentTime + 1000;

        // turn on the appropriate LED based on the toggle timepoint
        bool toggleState = (currentTime < toggleTime);
        digitalWrite(LED, toggleState);
        digitalWrite(EXT_LED, !toggleState);
    }
    else
    {
        // turn LEDs off when switch is off
        digitalWrite(LED, 0);
        digitalWrite(EXT_LED, 0);
    }

    // update seven-segment display
    SEVEN_SEG = CHARACTERS[count];

    // delay 1 ms to regulate loop speed
    delay(1);
}