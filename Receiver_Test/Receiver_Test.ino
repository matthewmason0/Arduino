#include <SoftwareSerial.h>
#include <custom_print.h>

SoftwareSerial hc12(5, 6); // RX, TX

static constexpr int SET = 9;
static constexpr int START = 7;
static constexpr int IGNITION = 10;
static constexpr int BATT = A6;

// ASCII Control Characters
static constexpr uint8_t ACK = 6  | 0x80; // request completed
static constexpr uint8_t NAK = 21 | 0x80; // request not completed
static constexpr uint8_t SOH = 1  | 0x80; // start of connection, initiates TX/RX sync
static constexpr uint8_t ENQ = 5  | 0x80; // enquiry of current values
static constexpr uint8_t STX = 2  | 0x80; // start engine request, followed by password
static constexpr uint8_t ETX = 3  | 0x80; // stop engine request

static constexpr uint8_t PASSWORD = 0xDB; // 11011011

// static constexpr unsigned long startTimeout = 15000; // ms

// static constexpr int debounceThreshold = 50;
// bool ignitionState = false;
// int ignitionReadCount = 0;

// bool startRequest = false;
// bool running = false;
// int index = 0;

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
    // while (Serial.available())
    // {
    //     hc12.write(Serial.read());
    // }
    if (hc12.available())
    {
        char c = hc12.read();
        hc12.write(c);
        println(c);
        // hc12.write(SOH);
    }
    // Serial.println("hi");
    // delay(1000);
}
