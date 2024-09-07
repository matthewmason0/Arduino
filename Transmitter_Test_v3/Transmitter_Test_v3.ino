#include <RH_RF95.h>
#include <custom_print.h>

static constexpr int RF95_CS  = 8;
static constexpr int RF95_INT = 7;
static constexpr int RF95_RST = 4;
static constexpr int START     = 5;
static constexpr int IGNITION  = 6;
static constexpr int BATT      = A9;

RH_RF95 rf95(RF95_CS, RF95_INT);

// static constexpr int BUTTON_A = 9;
// static constexpr int BUTTON_B = 6;
// static constexpr int BUTTON_C = 5;

// ASCII Control Characters
static constexpr uint8_t ACK = 6  | 0x80; // request completed
static constexpr uint8_t NAK = 21 | 0x80; // request not completed
static constexpr uint8_t SOH = 1  | 0x80; // start of connection, initiates TX/RX sync
static constexpr uint8_t ENQ = 5  | 0x80; // enquiry of current values
static constexpr uint8_t STX = 2  | 0x80; // start engine request, followed by password
static constexpr uint8_t ETX = 3  | 0x80; // stop engine request

static constexpr uint8_t PASSWORD = 0xDB; // 11011011

static constexpr uint32_t DISCOVERY_PERIOD = 2000; // ms
static constexpr uint32_t SYNC_PERIOD = 3000;
uint32_t _syncTimer = 0;

static constexpr size_t TX_BUFFER_LEN = 32;
char _txBuffer[TX_BUFFER_LEN] = "";

// char message[64] = "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz12345678901";

void printTxBuffer()
{
    for (int i = 0; _txBuffer[i]; i++)
        print((uint8_t)_txBuffer[i], ' ');
    println();
}

// append c to _txBuffer
void tx(const char c)
{
    size_t i;
    for (i = 0; _txBuffer[i]; i++);
    if ((i + 1) < TX_BUFFER_LEN)
    {
        _txBuffer[i] = c;
        _txBuffer[i + 1] = 0;
    }
    print("_txBuffer: ");
    printTxBuffer();
}

bool flag = true;

enum class SyncState
{
    DISCOVERY,
    SYNCED
};
SyncState _syncState = SyncState::DISCOVERY;
void _syncState_DISCOVERY(const uint32_t syncTime)
{
    uint8_t data[] = {SOH};
    rf95.send(data, 1);
    println("_syncState DISCOVERY");
    _txBuffer[0] = 0;
    _syncTimer = syncTime;
    _syncState = SyncState::DISCOVERY;
}
void _syncState_SYNCED(const uint32_t syncTime)
{
    if (_txBuffer[0])
    {
        size_t i;
        for (i = 0; _txBuffer[i]; i++);
        uint32_t a = millis();
        rf95.send((uint8_t*)_txBuffer, i);
        uint32_t b = millis();
        rf95.waitPacketSent();
        uint32_t c = millis();
        println("Call: ", b-a, " ms | TX: ", c-b, " ms");
        print("sent ");
        printTxBuffer();
        _txBuffer[0] = 0;
    }
    println("sync");
    flag = true;
    _syncTimer = syncTime;
    _syncState = SyncState::SYNCED;
}

void updateSync(const uint32_t now)
{
    switch (_syncState)
    {
        case SyncState::DISCOVERY:
        {
            if ((now - _syncTimer) >= DISCOVERY_PERIOD)
                _syncState_DISCOVERY(_syncTimer + DISCOVERY_PERIOD);
            break;
        }
        case SyncState::SYNCED:
        {
            // update timer and TX at start of new period
            if ((now - _syncTimer) >= SYNC_PERIOD)
                _syncState_SYNCED(_syncTimer + SYNC_PERIOD);
            if (flag && (now - _syncTimer) >= 1500)
            {
                println("half");
                flag = false;
            }
            break;
        }
    }
}

// static constexpr unsigned long timeout = 20000; // ms

// unsigned long lastTime = 0;

uint32_t start = 0;
uint32_t end = 0;
uint32_t timer = 6000;

void setup()
{
    // pinMode(BUTTON_A, INPUT_PULLUP);
    // pinMode(BUTTON_B, INPUT_PULLUP);
    // pinMode(BUTTON_C, INPUT_PULLUP);
    pinMode(RF95_RST, OUTPUT);
    pinMode(START, OUTPUT);
    pinMode(IGNITION, INPUT);
    digitalWrite(RF95_RST, 1);
    digitalWrite(START, 0);
    Serial.begin(9600);
    digitalWrite(RF95_RST, 0);
    delay(10);
    digitalWrite(RF95_RST, 1);
    delay(10);
    rf95.init();
    rf95.setFrequency(915.0f);
    rf95.setModemConfig(RH_RF95::ModemConfigChoice::Bw125Cr45Sf2048);
    rf95.setTxPower(23);
    delay(1000);
    // hc12.write(message);
    // println('0');
    // hc12.write('0');
    // println('0');
    // timer = millis();
    println("connecting...");
    start = millis();
    _syncState_DISCOVERY(start);
    // tx(SOH);
    // _syncState_SYNCED(millis());
    while (true)
    {
        while (!rf95.available())
        {
            updateSync(millis());
        }
        uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
        uint8_t len = sizeof(buf);
        rf95.recv(buf, &len);
        if (len && buf[0] == ACK)
            break;
        else
            println("failed to connect");
    }
    println("connected in ", millis() - start);
    // response corresponds to the PREVIOUS discovery TX
    _syncState_SYNCED(_syncTimer - DISCOVERY_PERIOD);
}

char i = '0';
void loop()
{
    // hc12.write(i);
    // println("i ", i);
    // while ((millis() - timer) < 1990)
    // {
    //     if (hc12.available())
    //         println("->", (char)hc12.read());
    // }
    // timer += 1990;
    // i++;

    // start = millis();
    // while ((millis() - timer) < 2980)
    // {
    //     if (hc12.available())
    //     {
    //         end = millis();
    //         hc12.read();
    //         println(end - start);
    //         break;
    //     }
    // }
    // while ((millis() - timer) < 2980);
    // if (end < start)
    //     println("request timed out");
    // timer += 2980;

    const uint32_t now = millis();
    if (Serial.available())
    {
        char c = Serial.read();
        if (c == '0')
            tx(SOH);
        else if (c == '1')
            tx(ENQ);
        else if (c == '2')
        {
            tx(STX);
            tx(PASSWORD);
        }
        else if (c == '3')
            tx(ETX);
        else
            tx(c);
    }
    // if (hc12.available())
    // {
    //     Serial.println(millis() - lastTime);
    //     lastTime = millis();
    //     // int count = 0;
    //     while (hc12.available())
    //     {
    //         // Serial.write(hc12.read());
    //         hc12.read();
    //         // count++;
    //     }
    //     // Serial.println(count);
    // }
    updateSync(now);
    while (rf95.available())
    {
        end = millis();
        uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
        uint8_t len = sizeof(buf);
        rf95.recv(buf, &len);
        if (len)
        {
            println(buf[0]);
            int16_t syncTime = end - _syncTimer;
            println("sync: ", syncTime - 1500 - 420, " ms");
        }
    }
    
}
