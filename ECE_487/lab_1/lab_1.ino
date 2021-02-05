/* lab_1
 * Created by Matt Mason,
 * CWID 11800439
 */

// LED pin numbers
static constexpr int EXT_LED = 2;
static constexpr int LED = 13;

// print the input prompt
void printPrompt()
{
    Serial.print("\nEnter \'g\' to begin blinking or \'s\' to stop: ");
}

// flip the state of both LEDs at the same time
void toggleLEDs()
{
    digitalWrite(EXT_LED, !digitalRead(EXT_LED));
    digitalWrite(LED, !digitalRead(LED));
}

void setup()
{
    // set LED pins as outputs
    pinMode(EXT_LED, OUTPUT);
    pinMode(LED, OUTPUT);

    // open serial connection and print prompt
    Serial.begin(9600);
    Serial.print("lab_1 by Matt Mason");
    printPrompt();
}

bool blinking = false;
void loop()
{
    // read from serial port while there is available data
    while(Serial.available())
    {
        char c = Serial.read();
        if (c == 'g') // received 'g' command to begin blinking
        {
            blinking = true;
            digitalWrite(EXT_LED, 0);
            digitalWrite(LED, 1);
            Serial.print("go");
            printPrompt();
            break;
        }
        else if (c == 's') // received 's' command to stop blinking
        {
            blinking = false;
            digitalWrite(EXT_LED, 0);
            digitalWrite(LED, 0);
            Serial.print("stop");
            printPrompt();
            break;
        }
        else if (c != '\n' && c != '\r') // disregard line-ending chars
        {
            // print error message for characters that aren't 'g', 's', or line-endings
            Serial.print("Invalid character \'"); Serial.print(c); Serial.print("\'!");
            printPrompt();
        }
    }

    // toggle LEDs according to the specefied pattern
    if (blinking)
    {
        delay(1000);
        toggleLEDs();
        delay(2000);
        toggleLEDs();
    }
}