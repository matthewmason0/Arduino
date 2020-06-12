#include <SoftwareSerial.h>
#include <Servo.h>
#include <DHT.h>

SoftwareSerial swSerial(2, 3);

static constexpr int HL = 8;

DHT dht(9, DHT22);

Servo blinds;
static constexpr int BLINDS = 10;
static constexpr int BLINDS_ENABLE = 11;
int blindsPos = -1;
void enableBlinds() { digitalWrite(BLINDS_ENABLE, HIGH); }
void disableBlinds() { digitalWrite(BLINDS_ENABLE, LOW); }

unsigned long sensorReadInterval = 2000;
unsigned long currentTime;
unsigned long previousRead = 0;

String command = "";

void moveBlinds(int percent);
void readDHT();
void receive();
void receiveSwitches();

void setup()
{
    pinMode(HL, OUTPUT);
    pinMode(BLINDS_ENABLE, OUTPUT);
    pinMode(13, OUTPUT);
    digitalWrite(HL, LOW);
    digitalWrite(BLINDS_ENABLE, LOW);
    digitalWrite(13, LOW);

    command.reserve(16);

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

    if (currentTime - previousRead > sensorReadInterval)
        readDHT();

    if (Serial.available())
        receive();
    
    if (swSerial.available())
        receiveSwitches();
}

void moveBlinds(int percent)
{
    int targetPos = percent * 12 + 900;
    
    if (blindsPos == -1)
        blindsPos = targetPos;
    
    if (blindsPos == targetPos)
        return;
    
    if (targetPos == 900 && blindsPos != 900)
        moveBlinds(-33);
    
    enableBlinds();
    bool dir = blindsPos < targetPos;
    for (int i = blindsPos; dir ? i <= targetPos : i >= targetPos; dir ? i++ : i--)
    {
        blinds.writeMicroseconds(i);
        delay(7);
    }
    blindsPos = targetPos;
    disableBlinds();
}

void readDHT()
{
    float temp = dht.readTemperature();
    float humidity = dht.readHumidity();

    if (isnan(temp) || isnan(humidity));
    else
    {
        Serial.print("dht "); Serial.print(temp); Serial.print(" "); Serial.println(humidity);
        previousRead = currentTime;
    }
}

void receive()
{
    char c = 0;
    while (c != '\n')
    {
        if (Serial.available())
        {
            c = (char)Serial.read();
            command += c;
        }
    }
    command.trim();
    if (command.equals("mlon"))
        swSerial.println("lon");
    else if (command.equals("mloff"))
        swSerial.println("loff");
    else if (command.equals("fon"))
        swSerial.println("fon");
    else if (command.equals("foff"))
        swSerial.println("foff");
    else if (command.equals("fhi"))
        swSerial.println("fhi");
    else if (command.equals("flo"))
        swSerial.println("flo");
    else if (command.equals("hlon"))
        digitalWrite(HL, 1);
    else if (command.equals("hloff"))
        digitalWrite(HL, 0);
    else
    {
        int index = command.indexOf('b');
        if (index != -1)
        {
            int percent = command.substring(index + 1).toInt();
            moveBlinds(percent);
        }
    }
    command = "";
}

void receiveSwitches()
{
    char c = 0;
    while (c != '\n')
    {
        if (swSerial.available())
        {
            c = (char)swSerial.read();
            command += c;
        }
    }
    command.trim();
    if (command.equals("?"))
        Serial.println("?");
    else if (command.length() == 3)
        Serial.println("sw " + command);
    command = "";
}
