#include "Receiver_v2.h"
#include "EngineState.h"

void setup()
{
    pinMode(SET, OUTPUT);
    pinMode(START, OUTPUT);
    pinMode(IGNITION, INPUT);
    pinMode(13, OUTPUT);
    hc12_wake();
    digitalWrite(START, 0);
    digitalWrite(13, 0);
    hc12.begin(1200);
    Serial.begin(9600);
    println("setting up sleep...");
    setup_sleep();
    println("setup complete");
}

void loop()
{
    const uint32_t now = millis();

    uint8_t c = 0;
    if (hc12.available())
    {
        c = (uint8_t)hc12.read();
        println("Received: ", c);
    }

    const bool blocked = step(now, c);

    const uint32_t dt = millis() - now;
    if (!blocked && (dt > 10))
        println("slow loop: ", dt, "ms");
}

// runs once per loop (non-blocking except sleep)
// returns true if step was blocking
bool step(const uint32_t now, const uint8_t msg)
{
    if (msg)
    {
        processMessage(now, msg);
        if (_syncState == SyncState::SYNCED)
            _state_CONNECTED(now);
    }
    updateEngine(now);
    bool blocked = false;
    switch (_state)
    {
        case ReceiverState::NOT_CONNECTED:
        {
            _syncState_IDLE();
            if (_engState == EngineState::OFF &&
                (now - _awakeTimer) > AWAKE_TIME)
            {
                _state_SLEEP();
            }
            break;
        }
        case ReceiverState::CONNECTED:
        {
            if ((now - _connectionTimer) > CONNECTION_TIMEOUT)
            {
                println("Connection timed out");
                _state_NOT_CONNECTED();
            }
            updateSync(now);
            break;
        }
        case ReceiverState::SLEEP:
        {
            println("Entering sleep mode @ ", now);
            sleep();
            uint32_t future = millis();
            println("Woke up @ ", future);
            blocked = true;
            _awakeTimer = future;
            _state_NOT_CONNECTED();
            break;
        }
    }
    return blocked;
}

// non-blocking message processing
void processMessage(const uint32_t now, const uint8_t msg)
{
    // println("processMessage(", now, ", ", msg, ")");
    println("_sessionTimer: ", (now - _sessionTimer));
    if (msg == SOH && (now - _sessionTimer) > SYNC_PERIOD)
    {
        tx(ACK);
        _sessionTimer = now;
        // immediately sends response, then aligns to sync
        _syncState_SYNCED(millis() - SYNC_DELAY);
        return;
    }
    // if messsage is a request, override current _requestState
    if (msg & 0x80)
    {
        if (msg == ENQ)
            _requestState_ENQ();
        else if (msg == STX)
            _requestState_STX();
        else if (msg == ETX)
            _requestState_ETX();
    }
    switch(_requestState)
    {
        case RequestState::NONE:
            break;
        case RequestState::ENQ:
        {
            uint8_t batt = measureBattery();
            uint16_t time = getEngineTime(now);
            uint8_t state = (uint8_t)_engState;
            // println("batt: ", batt, " time: ", time, " state: ", state);
            tx(batt);
            tx((uint8_t)(time & 0x7F)); // lower 7 bits of time
            tx((state << 5) | (uint8_t)(time >> 7)); // state | upper 5 bits of time
            _requestState_NONE();
            break;
        }
        case RequestState::STX:
        {
            if (msg == PASSWORD)
            {
                _engState_STARTING(now);
                tx(ACK);
                _requestState_NONE();
            }
            // cancel request if something other than STX or PASSWORD received
            else if (msg != STX)
            {
                tx(NAK);
                _requestState_NONE();
            }
            break;
        }
        case RequestState::ETX:
        {
            _engState_OFF(now);
            tx(ACK);
            _requestState_NONE();
        }
    }
}

void updateEngine(const uint32_t now)
{
    static bool ignState = false;
    static bool debounceState = false;
    static uint32_t debounceTimer = 0;
    debounce(ignState, digitalRead(IGNITION),
             debounceState, debounceTimer, now, IGN_DEBOUNCE_TIME);
    switch(_engState)
    {
        case EngineState::OFF:
        {
            digitalWrite(START, 0);
            digitalWrite(13, 0);
            if (ignState)
                _engState_RUNNING(now);
            break;
        }
        case EngineState::STARTING:
        {
            digitalWrite(START, 1);
            digitalWrite(13, 1);
            if (ignState)
                _engState_RUNNING(now);
            else if ((now - _startTimer) > START_TIMEOUT)
            {
                println("Start timed out");
                _engState_OFF(now);
            }
            break;
        }
        case EngineState::RUNNING:
        {
            if (!ignState)
                _engState_OFF(now);
            break;
        }
    }
}

uint8_t measureBattery()
{
    adc_on();
    float reading = analogRead(BATT) / 1024.0f;
    adc_off();
    float voltage = reading * 3.3f * 2.0f;
    // println(voltage);
    float rawPercent = (voltage - 3.2f) * 100.0f; // 3.2-4.2 V => 0-100 %
    uint8_t percent = (uint8_t)round(constrain(rawPercent, 0.0f, 100.0f));
    // println(percent, '%');
    return percent;
}
