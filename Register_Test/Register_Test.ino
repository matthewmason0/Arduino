#include <SoftwareSerial.h>
#include <custom_print.h>
#include <avr/wdt.h>
#include <avr/sleep.h>


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

static constexpr char PASSWORD[5] = "matt";
static constexpr int PASSWORD_LEN = 4;

static constexpr uint32_t START_TIMEOUT = 15000; // ms

static constexpr int DEBOUNCE_THRESHOLD = 50;
bool _ignitionState = false;
int _ignitionReadCount = 0;

bool _startRequest = false;
bool _running = false;
int _index = 0;

enum class ReceiverState
{
    NOT_CONNECTED,
    CONNECTED,
    SLEEP
};

enum class EngineState
{
    OFF,
    STARTING,
    RUNNING
};

ReceiverState _state = ReceiverState::NOT_CONNECTED;
EngineState _engState = EngineState::OFF;

static constexpr uint32_t CONNECTION_TIMEOUT = 30000; // ms
uint32_t _connectionTimer = 0;

static constexpr uint32_t SLEEP_TIME = 4000; // ms
static constexpr uint32_t AWAKE_TIME = 1000; // ms
uint32_t _awakeTimer = 0;

void blink()
{
    digitalWrite(13, 1);
    delay(100);
    digitalWrite(13, 0);
}

void hc12_sleep()
{
    digitalWrite(SET, 0);
    delay(40);
    hc12.println("AT+SLEEP");
    digitalWrite(SET, 1);
}

void hc12_wake()
{
    digitalWrite(SET, 0);
    delay(40);
    digitalWrite(SET, 1);
}

void adc_off()
{
    ADCSRA &= ~(1<<ADEN);
    PRR |= (1<<PRADC);
}

void adc_on()
{
    PRR &= ~(1<<PRADC);
    ADCSRA |= (1<<ADEN);
}

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
    PRR = (1<<PRTWI) | (1<<PRSPI); // shut down TWI & SPI
    // cli();
    // wdt_reset();
    // WDTCSR |= (1<<WDCE) | (1<<WDE); // enable changes
    // WDTCSR = (1<<WDIE) | (0<<WDE) | (1<<WDP3); // ~4s interrupt
    // sei();
    // println("measure original...");
    // blink();
    // delay(3000);

    // println("measure hc12 sleep...");
    // blink();
    // hc12_sleep();
    // delay(3000);

    blink();
    print("ACSR ");
    Serial.println(ACSR, BIN);
    delay(5000);

    blink();
    println("measure AC off...");
    ACSR &= ~(1<<ACBG) & ~(1<<ACIE); // disable voltage ref & interrupt
    ACSR |= (1<<ACD); // disable analog comparator
    print("ACSR ");
    Serial.println(ACSR, BIN);
    delay(5000);

    // println("measure TWI off...");
    // blink();
    // PRR = 0b10000000; // shut down TWI
    // delay(3000);

    // println("measure ADC off...");
    // blink();
    // ADCSRA &= 0b01111111;
    // PRR |= 0b00000001;
    // delay(3000);



    // blink();
    // cli();
    // wdt_reset();
    // set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    // sleep_enable();
    // sleep_bod_disable();
    // sei();
    // sleep_cpu();
    // sleep_disable();
    // blink();

    // print("PRR ");
    // Serial.println(PRR, BIN);
    // print("ACSR ");
    // Serial.println(ACSR, BIN);
    // print("ADCSRA ");
    // Serial.println(ADCSRA, BIN);
    // print("ADCSRB ");
    // Serial.println(ADCSRB, BIN);
    // print("WDTCSR ");
    // Serial.println(WDTCSR, BIN);
    // print("MCUSR ");
    // Serial.println(MCUSR, BIN);
    // print("TWCR ");
    // Serial.println(TWCR, BIN);

    // blink();
    // print("ADCSRA ");
    // Serial.println(ADCSRA, BIN);
    // delay(5000);
    // blink();
    // println("analogRead");
    // println(analogRead(BATT));
    // delay(5000);
    // blink();
    // println("turn adc off");
    // adc_off();
    // print("ADCSRA ");
    // Serial.println(ADCSRA, BIN);
    // delay(5000);
    // blink();
    // println("turn adc on");
    // adc_on();
    // println(analogRead(BATT));
    // println("analogRead");
    // delay(5000);
    
}

void loop()
{

}

ISR(WDT_vect) {}
