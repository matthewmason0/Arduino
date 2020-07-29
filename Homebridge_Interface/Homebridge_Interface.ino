#include <AltSoftSerial.h>
#include <ServoTimer2.h>
#include <DHT.h>

AltSoftSerial swSerial; // 8, 9

DHT dht(2, DHT22);
static constexpr int DHT_ENABLE = 3;

static constexpr int CS_ENABLE = 10;
static constexpr int CS = 11;

static constexpr int HL = 12;

ServoTimer2 blinds;
static constexpr int BLINDS = 4;
static constexpr int BLINDS_ENABLE = 5;
int currentPos = -1;
int targetPos = -1;
void enableBlinds() { digitalWrite(BLINDS_ENABLE, 1); }
void disableBlinds() { digitalWrite(BLINDS_ENABLE, 0); }

unsigned long sensorReadInterval = 5000;
unsigned long currentTime;
unsigned long previousRead = 0;

static constexpr int debounceThreshold = 10;
int csReadCount = 0;
bool csState;

String hbCommand = "";
String swCommand = "";

void moveBlinds();
void readDHT();
void checkContactSensor();
void receiveHomebridge();
void receiveSwitches();

void setup()
{
    pinMode(DHT_ENABLE, OUTPUT);
    pinMode(BLINDS_ENABLE, OUTPUT);
    pinMode(CS_ENABLE, OUTPUT);
    pinMode(CS, INPUT);
    pinMode(HL, OUTPUT);
    pinMode(13, OUTPUT);
    digitalWrite(DHT_ENABLE, 1);
    digitalWrite(BLINDS_ENABLE, 0);
    digitalWrite(CS_ENABLE, 1);
    digitalWrite(HL, 0);
    digitalWrite(13, 0);

    hbCommand.reserve(32);
    swCommand.reserve(32);

    Serial.begin(9600);
    while (!Serial);
    Serial.println();
    Serial.println("?");
    swSerial.begin(1200);

    dht.begin();

    blinds.attach(BLINDS);

    csState = digitalRead(CS);
}

void loop()
{
    currentTime = millis();

    if (currentTime - previousRead > sensorReadInterval && currentPos == targetPos)
        readDHT();

    if (Serial.available())
        receiveHomebridge();
    
    if (swSerial.available())
        receiveSwitches();
    
    moveBlinds();

    checkContactSensor();
}

void moveBlinds()
{   
    if (targetPos == -1) // target is unset
        return;
    
    if (currentPos == -1) // initial update of currentPos
        currentPos = targetPos;
    
    if (currentPos == targetPos) // target reached
    {
        disableBlinds();
        return;
    }
    
    const bool dir = currentPos < targetPos;

    currentPos += dir ? 1 : -1;

    enableBlinds();
    blinds.write(currentPos);
    delay(7);
    
    if (currentPos == 900 && !dir) // closing and reached 0%
        targetPos = 800;

    if (currentPos == 800 && !dir) // reached turnaround point
        targetPos = 900;
}

void readDHT()
{
    float temp = dht.readTemperature();
    float humidity = dht.readHumidity();

    if (!isnan(temp) && !isnan(humidity))
    {
        Serial.print("dht "); Serial.print(temp); Serial.print(" "); Serial.println(humidity);
        previousRead = currentTime;
    }
}

void checkContactSensor()
{
    if (csState != digitalRead(CS))
    {
        if (csReadCount == debounceThreshold)
        {
            csReadCount = 0;
            csState = !csState;
            Serial.print("cs "); Serial.println(csState);
        }
        else
            csReadCount++;
    }
    else
        csReadCount = 0;
}

void receiveHomebridge()
{
    char c = 0;
    while (Serial.available()) // read until newline encountered or buffer empty
    {
        c = (char)Serial.read();
        if (c == '\n')
            break;
        hbCommand += c;
    }
    if (c == '\n')
    {
        hbCommand.trim();
//        Serial.print("###"); Serial.println(hbCommand);
        if (hbCommand.equals("mlon"))
            swSerial.println("lon");
        else if (hbCommand.equals("mloff"))
            swSerial.println("loff");
        else if (hbCommand.equals("fon"))
            swSerial.println("fon");
        else if (hbCommand.equals("foff"))
            swSerial.println("foff");
        else if (hbCommand.equals("fhi"))
            swSerial.println("fhi");
        else if (hbCommand.equals("flo"))
            swSerial.println("flo");
        else if (hbCommand.equals("hlon"))
        {
            digitalWrite(HL, 1);
            swSerial.println("nlon");
        }
        else if (hbCommand.equals("hloff"))
        {
            digitalWrite(HL, 0);
            swSerial.println("nloff");
        }
        else if (hbCommand.equals("nmon"))
            swSerial.println("nmon");
        else if (hbCommand.equals("nmoff"))
            swSerial.println("nmoff");
        else
        {
            int index = hbCommand.indexOf('b');
            if (index != -1)
            {
                int percent = hbCommand.substring(index + 1).toInt();
                targetPos = percent * 12 + 900;
            }
        }
        hbCommand = "";
    }
}

void receiveSwitches()
{
    char c = 0;
    while (swSerial.available()) // read until newline encountered or buffer empty
    {
        c = (char)swSerial.read();
        if (c == '\n')
            break;
        swCommand += c;
    }
    if (c == '\n')
    {
        swCommand.trim();
        if (swCommand.equals("?"))
            Serial.println("?");
        else if (swCommand.length() == 4)
        {
            Serial.println("sw " + swCommand);
            int hl = swCommand[1] - '0';
            digitalWrite(HL, hl);
        }
        swCommand = "";
    }
}
