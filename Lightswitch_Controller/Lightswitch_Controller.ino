#include <SoftwareSerial.h>

SoftwareSerial swSerial(5, 6);

bool light = false;
bool fan = false;
bool fanSpeed = true;

static constexpr int debounceThreshold = 5;
bool lightSwitchState;
bool fanSwitchState;
int lightSwitchReadCount = 0;
int fanSwitchReadCount = 0;

String command = "";

void setLight(bool state);
void setFan(bool state, bool speed);
void send();
void receive();

void setup()
{
    pinMode(2, INPUT);
    pinMode(3, INPUT);
    pinMode(9, OUTPUT);
    pinMode(10, OUTPUT);
    pinMode(11, OUTPUT);
    pinMode(13, OUTPUT);
    digitalWrite(13, 0);

    setLight(light);
    setFan(fan, fanSpeed);

    lightSwitchState = digitalRead(2);
    fanSwitchState = digitalRead(3);

    command.reserve(32);

//    Serial.begin(9600);
//    while (!Serial);
    swSerial.begin(1200);
    swSerial.println("?");
}

void loop()
{
    if (lightSwitchState != digitalRead(2))
    {
        if (lightSwitchReadCount == debounceThreshold)
        {
            lightSwitchReadCount = 0;
            lightSwitchState = !lightSwitchState;
            setLight(!light);
            send();
        }
        else
            lightSwitchReadCount++;
    }
    else
        lightSwitchReadCount = 0;

    if (fanSwitchState != digitalRead(3))
    {
        if (fanSwitchReadCount == debounceThreshold)
        {
            fanSwitchReadCount = 0;
            fanSwitchState = !fanSwitchState;
            setFan(!fan, fanSpeed);
            send();
        }
        else
            fanSwitchReadCount++;
    }
    else
        fanSwitchReadCount = 0;

    if (swSerial.available())
        receive();
}

void setLight(bool state)
{
    digitalWrite(9, state);
    light = state;
}

void setFan(bool state, bool speed)
{
    if (speed)
    {
        digitalWrite(10, state);
        digitalWrite(11, 0);
    }
    else
    {
        digitalWrite(11, state);
        if (state && !fan)
        {
            digitalWrite(10, 1);
            delay(1000);
        }
        digitalWrite(10, 0);
    }
    fan = state;
    fanSpeed = speed;
}

void send()
{
    swSerial.print(light);
    swSerial.print(fan);
    swSerial.println(fanSpeed);
}

void receive()
{
    char c = 0;
    while (swSerial.available()) // read until newline encountered or buffer empty
    {
        c = (char)swSerial.read();
        if (c == '\n')
            break;
        command += c;
    }
    if (c == '\n')
    {
        command.trim();
        Serial.println(command);
        if (command.equals("lon"))
            setLight(1);
        else if (command.equals("loff"))
            setLight(0);
        else if (command.equals("fon"))
            setFan(1, fanSpeed);
        else if (command.equals("foff"))
            setFan(0, fanSpeed);
        else if (command.equals("flo"))
            setFan(fan, 0);
        else if (command.equals("fhi"))
            setFan(fan, 1);
        command = "";
    }
}