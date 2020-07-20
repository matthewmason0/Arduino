#include <SoftwareSerial.h>
#include <Servo.h>
#include <DHT.h>

SoftwareSerial swSerial(2, 3);

static constexpr int HL = 8;

DHT dht(9, DHT22);

Servo blinds;
static constexpr int BLINDS = 10;
static constexpr int BLINDS_ENABLE = 11;
int currentPos = -1;
int targetPos = -1;
void enableBlinds() { digitalWrite(BLINDS_ENABLE, HIGH); }
void disableBlinds() { digitalWrite(BLINDS_ENABLE, LOW); }

unsigned long sensorReadInterval = 2000;
unsigned long currentTime;
unsigned long previousRead = 0;

String hbCommand = "";
String swCommand = "";

void moveBlinds();
void readDHT();
void receiveHomebridge();
void receiveSwitches();

void setup()
{
    pinMode(HL, OUTPUT);
    pinMode(BLINDS_ENABLE, OUTPUT);
    pinMode(13, OUTPUT);
    digitalWrite(HL, LOW);
    digitalWrite(BLINDS_ENABLE, LOW);
    digitalWrite(13, LOW);

    hbCommand.reserve(32);
    swCommand.reserve(32);

    Serial.begin(9600);
    while (!Serial);
    Serial.println();
    Serial.println("?");
    swSerial.begin(1200);

    dht.begin();

    blinds.attach(BLINDS);
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
    blinds.writeMicroseconds(currentPos);
    delay(7);
    
    if (currentPos == 900 && !dir) // closing and reached 0%
        targetPos = 500;

    if (currentPos == 500 && !dir) // reached turnaround point
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
            digitalWrite(HL, 1);
        else if (hbCommand.equals("hloff"))
            digitalWrite(HL, 0);
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
