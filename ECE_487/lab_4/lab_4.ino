/*****************************************************************************
 * lab_4
 * Created by Matt Mason,
 * CWID 11800439
******************************************************************************/

#include <avr/wdt.h>

// potentiometer pin
#define POT A5

/*****************************************************************************
 * Function:        doConversions
 * Parameters:      none
 * Return value:    none
 * Purpose:         Performs 30 A->D conversions for the POT analog input.
 *                  Outputs the hex value and the conversion time for each,
 *                  as well as the average conversion time at the end.
******************************************************************************/
void doConversions()
{
    unsigned long totalTime = 0;
    unsigned long startTime, conversionTime;
    for (int i = 0; i < 30; i++)
    {
        // record start time
        unsigned long startTime = micros();
        // do conversion
        int value = analogRead(POT);
        // calculate elapsed time
        unsigned long conversionTime = micros() - startTime;
        // display #, value, and conversion time nicely formatted
        if (i+1 < 10)
            Serial.print(" ");
        Serial.print("#");
        Serial.print(i + 1);
        Serial.print(": digital value = ");
        if (value < 0x010)
            Serial.print("0");
        if (value < 0x100)
            Serial.print("0");
        Serial.print(value, HEX);
        Serial.print(", conversion time = ");
        Serial.print(conversionTime);
        Serial.println(" us");
        // sum all conversion times
        totalTime += conversionTime;
    }
    // calculate average conversion time
    float averageTime = totalTime / 30.0f;
    // display average conversion time
    Serial.print("average conversion time = ");
    Serial.print(averageTime, 2);
    Serial.println(" us");
    // disregard any received input by clearing serial buffer
    while (Serial.read() != -1);
}

void setup()
{
    // setup input pin
    pinMode(POT, INPUT);

    // open serial connection
    Serial.begin(9600);
    Serial.println("\nlab_4 — Board Reset");

    // enable watchdog timer with a 4-second timeout
    wdt_enable(WDTO_4S);
}

void loop()
{
    // display prompt
    Serial.print("Enter 'c' to start a set of conversions: ");
    // refresh watchdog
    wdt_reset();

    // loop condition variable
    bool waiting = true;
    // user input string
    String input = "";
    while (waiting)
    {
        // read serial into input string until buffer is empty or newline received
        char c = 0;
        while (Serial.available())
        {
            c = (char)Serial.read();
            if (c == '\n')
                break;
            input += c;
        }
        // if last character reveived was a newline, user input is ready
        if (c == '\n')
            waiting = false;
    }

    // refresh watchdog and respond to user input
    wdt_reset();
    if (input.equals("c"))
    {
        // do A->D conversions
        Serial.println(input);
        Serial.println("Starting a set of 30 conversions:");
        doConversions();
    }
    else // invalid input
    {
        // display error message
        Serial.println(input);
        Serial.println("Invalid input!!");
    }
}