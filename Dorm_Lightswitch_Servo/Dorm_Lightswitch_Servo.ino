#include <Servo.h>
#include <DHT.h>

static constexpr int NUM_SERVOS = 4;
static constexpr int SERVOS_ENABLE = 6;
void enableServos() { digitalWrite(SERVOS_ENABLE, HIGH); delay(300); }
void disableServos() { digitalWrite(SERVOS_ENABLE, LOW); }

Servo mlon;
Servo mloff;
Servo fon;
Servo foff;

enum Servos      { MLON, MLOFF, FON, FOFF };
Servo servos[] = { mlon, mloff, fon, foff };

enum Info { PIN, NORMAL, ACTIVATED, SLEW };
int info[][4] = { { 2, 70,  150, 1800 },   //MLON
                  { 3, 80,  30,  800 },   //MLOFF
                  { 4, 85,  150, 500 },   //FON
                  { 5, 75,  0,   850 } }; //FOFF

static constexpr int INITIAL_SLEW = 800;
static constexpr int RESET_SLEW = 50;

static constexpr int HL = 8;

DHT spaceDHT(9, DHT22);

Servo blinds;
static constexpr int BLINDS = 10;
static constexpr int BLINDS_ENABLE = 11;
int blindsPos = 900;
void enableBlinds() { digitalWrite(BLINDS_ENABLE, HIGH); }
void disableBlinds() { digitalWrite(BLINDS_ENABLE, LOW); }

unsigned long servoResetInterval = 500;
unsigned long sensorReadInterval = 2000;
unsigned long currentTime;
unsigned long previousReset = 0;
unsigned long previousRead = 0;

String command = "";

void setup()
{
  command.reserve(200);

  pinMode(SERVOS_ENABLE, OUTPUT);
  pinMode(HL, OUTPUT);
  pinMode(BLINDS_ENABLE, OUTPUT);
  pinMode(13, OUTPUT);
  digitalWrite(SERVOS_ENABLE, LOW);
  digitalWrite(HL, LOW);
  digitalWrite(BLINDS_ENABLE, LOW);
  digitalWrite(13, LOW);

  Serial.begin(9600);

  spaceDHT.begin();

  enableServos();
  for (int i = 0; i < NUM_SERVOS; i++)
  {
    servos[i].attach(info[i][PIN]);
    servos[i].write(info[i][NORMAL]);
  }
  delay(INITIAL_SLEW);
  for (int i = 0; i < NUM_SERVOS; i++)
    servos[i].detach();
  disableServos();

  blinds.attach(BLINDS);
  enableBlinds();
  blinds.writeMicroseconds(blindsPos);
  delay(INITIAL_SLEW);
  disableBlinds();
}

void moveBlinds(int targetPos)
{
  if (blindsPos == targetPos)
    return;
  if (targetPos == 900 && blindsPos != 900)
    moveBlinds(500);
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

void activateServo(Servos s)
{
  enableServos();
  servos[s].attach(info[s][PIN]);
  servos[s].write(info[s][ACTIVATED]);
  delay(info[s][SLEW]);
  servos[s].write(info[s][NORMAL]);
  delay(info[s][SLEW]);
  servos[s].detach();
  disableServos();
}

void loop()
{
  currentTime = millis();

//  if (currentTime - previousReset > servoResetInterval)
//  {
//    for (int i = 0; i < NUM_SERVOS; i++)
//    {
//      servos[i].attach(info[i][PIN]);
//      servos[i].write(info[i][NORMAL]);
//    }
//    delay(RESET_SLEW);
//    for (int i = 0; i < NUM_SERVOS; i++)
//      servos[i].detach();
//    previousReset = currentTime;
//  }

  if (currentTime - previousRead > sensorReadInterval)
  {
    float spaceTemp  = spaceDHT.readTemperature(false); //true F, false C
    float spaceHum  = spaceDHT.readHumidity();
    if (isnan(spaceTemp) || isnan(spaceHum));
    else
    {
      //spaceTemp -= 2.5;
      //spaceHum -= 10;
      Serial.print(spaceTemp); Serial.print(" "); Serial.println(spaceHum);
      previousRead = currentTime;
    }
  }

  if (Serial.available())
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
      activateServo(MLON);
    if (command.equals("mloff"))
      activateServo(MLOFF);
    if (command.equals("fon"))
      activateServo(FON);
    if (command.equals("foff"))
      activateServo(FOFF);
    if (command.equals("hlon"))
      digitalWrite(HL, HIGH);
    if (command.equals("hloff"))
      digitalWrite(HL, LOW);
    int index = command.indexOf('b');
    if (index != -1)
    {
      int percent = command.substring(index + 1).toInt();
      int targetPos = percent * 12 + 900;
      moveBlinds(targetPos);
    }
    command = "";
  }
  
  delay(1);
}
