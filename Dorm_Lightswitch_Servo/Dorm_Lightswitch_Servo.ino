#include <Servo.h>
#include <DHT.h>

static constexpr int NUM_SERVOS = 6;

Servo mlon;
Servo mloff;
Servo fon;
Servo foff;
Servo clon;
Servo cloff;

enum Servos      { MLON, MLOFF, FON, FOFF, CLON, CLOFF };
Servo servos[] = { mlon, mloff, fon, foff, clon, cloff };

enum Info { PIN, NORMAL, ACTIVATED, SLEW };
int info[][4] = { { 2, 70,  150, 800 },   //MLON
                  { 3, 80,  30,  400 },   //MLOFF
                  { 4, 85,  130, 400 },   //FON
                  { 5, 75,  0,   700 },   //FOFF
                  { 6, 65,  115, 500 },   //CLON
                  { 7, 80,  180, 600 } }; //CLOFF

static constexpr int HL = 8;

static constexpr int SLEW_TIME = 500;

DHT spaceDHT(9, DHT11);

unsigned long servoResetInterval = 500;
unsigned long sensorReadInterval = 2000;
unsigned long currentTime;
unsigned long previousReset = 0;
unsigned long previousRead = 0;

String command = "";

void setup()
{
  command.reserve(200);

  pinMode(HL, OUTPUT);
  pinMode(13, OUTPUT);
  digitalWrite(HL, LOW);
  digitalWrite(13, LOW);

  Serial.begin(9600);

  spaceDHT.begin();

  for (int i = 0; i < NUM_SERVOS; i++)
  {
    servos[i].attach(info[i][PIN]);
    servos[i].write(info[i][NORMAL]);
  }
  delay(SLEW_TIME);

   for (int i = 0; i < NUM_SERVOS; i++)
     servos[i].detach();
}

void activateServo(Servos s)
{
  servos[s].attach(info[s][PIN]);
  servos[s].write(info[s][ACTIVATED]);
  delay(info[s][SLEW]);
  servos[s].write(info[s][NORMAL]);
  delay(info[s][SLEW]);
  servos[s].detach();
}

void loop()
{
  currentTime = millis();

  if(currentTime - previousReset > servoResetInterval)
  {
    for (int i = 0; i < NUM_SERVOS; i++)
    {
      servos[i].attach(info[i][PIN]);
      servos[i].write(info[i][NORMAL]);
    }
    delay(50);
    for (int i = 0; i < NUM_SERVOS; i++)
      servos[i].detach();
    previousReset = currentTime;
  }

  if(currentTime - previousRead > sensorReadInterval)
  {
    float spaceTemp  = spaceDHT.readTemperature(false); //true F, false C
    float spaceHum  = spaceDHT.readHumidity();
    if(isnan(spaceTemp) || isnan(spaceHum));
    else
    {
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
    if (command.equals("clon"))
      activateServo(CLON);
    if (command.equals("cloff"))
      activateServo(CLOFF);
    if (command.equals("hlon"))
      digitalWrite(HL, HIGH);
    if (command.equals("hloff"))
      digitalWrite(HL, LOW);
    command = "";
  }
  
  delay(1);
}
