#include <SoftwareSerial.h>

SoftwareSerial hc12(5, 6); // RX, TX

static constexpr int SET = 9;
static constexpr int START = 7;
static constexpr int IGNITION = 10;
static constexpr int BATT = A6;

static constexpr char STX = 2; // beginning of start request, password to follow
static constexpr char ENQ = 5; // ping
static constexpr char ACK = 6; // reply / request received
static constexpr char DC1 = 17; // request battery level
static constexpr char CAN = 24; // cancel

static constexpr char password[5] = "matt";
static constexpr int passwordLen = 4;

static constexpr unsigned long startTimeout = 15000; // ms

static constexpr int debounceThreshold = 50;
bool ignitionState = false;
int ignitionReadCount = 0;

bool startRequest = false;
bool running = false;
int index = 0;

void handleStartRequest(uint8_t c);
bool engineRunning();
void sendBattery();
void startEngine();
void cancel();

void setup()
{
    pinMode(SET, OUTPUT);
    pinMode(START, OUTPUT);
    pinMode(IGNITION, INPUT);
    pinMode(13, OUTPUT);
    digitalWrite(SET, 1);
    digitalWrite(START, 0);
    digitalWrite(13, 0);
    hc12.begin(1200);
    Serial.begin(9600);
}

void loop()
{
    uint8_t c = 0;
    if (hc12.available())
    {
        c = (uint8_t)hc12.read();
        Serial.println(c);

        if (startRequest)
            handleStartRequest(c);
        else if (c == ENQ)
            hc12.write(ACK);
        else if (c == DC1)
            sendBattery();
        else if (c == STX)
            startRequest = true;
        else if (c == CAN)
            cancel();
        else if (c == '?')
            hc12.write(running ? '1' : '0');
    }
    running = engineRunning();
    if (!running)
        cancel();
}

bool handleStartRequest(uint8_t c)
{
    if (c == password[index])
        index++;
    else
    {
        startRequest = false;
        index = 0;
        return false;
    }
    if (index == passwordLen)
    {
        startEngine();
        startRequest = false;
        index = 0;
    }
    return true;
}

bool engineRunning()
{
    if (digitalRead(IGNITION) != ignitionState)
    {
        if (ignitionReadCount == debounceThreshold)
        {
            ignitionReadCount = 0;
            ignitionState = !ignitionState;
        }
        else
            ignitionReadCount++;
    }
    else
        ignitionReadCount = 0;
    return ignitionState;
}

void sendBattery()
{
    float reading = analogRead(BATT) / 1024.0f;
    float voltage = reading * 3.3f * 2.0f;
    Serial.println(voltage);
    int percent = round((voltage - 3.2f) * 100.0f); // 3.2-4.2 V => 0-100 %
    Serial.print(percent); Serial.println('%');
    uint8_t message = constrain(percent, 0, 100) + 128;
    hc12.write(message);
}

void startEngine()
{
    if (!running)
    {
        digitalWrite(START, 1);
        digitalWrite(13, 1);
        unsigned long startTime = millis();
        while (millis() - startTime < startTimeout)
        {
            if (engineRunning())
            {
                running = true;
                break;
            }
        }
    }
    hc12.write(running ? '1' : '0');
}

void cancel()
{
    digitalWrite(START, 0);
    digitalWrite(13, 0);
    running = false;
}
