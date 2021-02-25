#define OS 0
static constexpr int DISPLAY_0_PINS[] = { 22+OS, 23+OS, 24+OS, 25+OS, 22+16+OS, 23+16+OS, 24+16+OS, 25+16+OS };
#define OS 4
static constexpr int DISPLAY_1_PINS[] = { 22+OS, 23+OS, 24+OS, 25+OS, 22+16+OS, 23+16+OS, 24+16+OS, 25+16+OS };
#define OS 8
static constexpr int DISPLAY_2_PINS[] = { 22+OS, 23+OS, 24+OS, 25+OS, 22+16+OS, 23+16+OS, 24+16+OS, 25+16+OS };
#define OS 12
static constexpr int DISPLAY_3_PINS[] = { 22+OS, 23+OS, 24+OS, 25+OS, 22+16+OS, 23+16+OS, 24+16+OS, 25+16+OS };
#undef OS

static constexpr int PIN_TO_SEG[] = { 6, 5, 0, 1, 4, 3, 2, 7 };

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

static constexpr byte MATT[] = {
    B01111111,
    B10111111,
    B11011111,
    B11101111,
    B11110111,
    B11111011
};

void updateSevenSeg(const int pins[], const int character, const int dp)
{
    for (int i = 0; i < 7; i++)
    {
        digitalWrite(pins[i], CHARACTERS[character] & (1<<(7-PIN_TO_SEG[i])));
    }
    digitalWrite(pins[7], !dp);
}

void matt(const int pins[], const int character, const int dp)
{
    for (int i = 0; i < 7; i++)
    {
        digitalWrite(pins[i], MATT[character] & (1<<(7-PIN_TO_SEG[i])));
    }
    digitalWrite(pins[7], !dp);
}

void setup()
{
    // configure all seven-seg. pins as outputs
    for (int i = 0; i < 8; i++)
    {
        pinMode(DISPLAY_0_PINS[i], OUTPUT);
        pinMode(DISPLAY_1_PINS[i], OUTPUT);
        pinMode(DISPLAY_2_PINS[i], OUTPUT);
        pinMode(DISPLAY_3_PINS[i], OUTPUT);
    }

    // open serial connection
    // Serial.begin(9600);
}

unsigned long time;
int ten, one, tenth, hundredth, thousandth;
int lastTenth, lastHundredth, lastThousandth;
int a, b, c;
int d = 0, e = 0, f = 0;
void loop()
{
    time = millis();
    // ten = time / 10000;
    one = (time % 10000) / 1000;
    tenth = (time % 1000) / 100;
    hundredth = (time % 500) / 50;
    thousandth = (time % 250) / 25;
    a = (tenth < lastTenth) ? (tenth + 10 - lastTenth) : (tenth - lastTenth);
    b = (hundredth < lastHundredth) ? (hundredth + 10 - lastHundredth) : (hundredth - lastHundredth);
    c = (thousandth < lastThousandth) ? (thousandth + 10 - lastThousandth) : (thousandth - lastThousandth);
    d = (d + a) % 6;
    e = (e + b) % 6;
    f = (f + c) % 6;
    lastTenth = tenth;
    lastHundredth = hundredth;
    lastThousandth = thousandth;
    // updateSevenSeg(DISPLAY_0_PINS, ten, 0);
    updateSevenSeg(DISPLAY_0_PINS, one, 1);
    // updateSevenSeg(DISPLAY_2_PINS, tenth, 0);
    // updateSevenSeg(DISPLAY_3_PINS, hundredth, 0);
    // updateSevenSeg(DISPLAY_3_PINS, thousandth, 0);
    matt(DISPLAY_1_PINS, d, 0);
    matt(DISPLAY_2_PINS, e, 0);
    matt(DISPLAY_3_PINS, f, 0);
}