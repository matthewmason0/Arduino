#include <Servo.h>
//#include <DHT.h>

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
int info[][4] = { { 2, 70,  150, 600 },   //MLON
                  { 3, 80,  30,  400 },   //MLOFF
                  { 4, 85,  120, 300 },   //FON
                  { 5, 75,  0,   700 },   //FOFF
                  { 6, 65,  115, 500 },   //CLON
                  { 7, 80,  180, 600 } }; //CLOFF

static constexpr int HL = 8;

static constexpr int SLEW_TIME = 500;

//DHT supplyDHT(12, DHT11);
//DHT spaceDHT(10, DHT11);

unsigned long servoResetInterval = 500;
//unsigned long sensorReadInterval = 2000;
unsigned long currentTime;
unsigned long previousReset = 0;
//unsigned long previousRead = 0;

String command = "";

void setup()
{
  command.reserve(200);

  //pinMode( 9, OUTPUT);
  //pinMode(11, OUTPUT);
  pinMode(HL, OUTPUT);
  pinMode(13, OUTPUT);
  //digitalWrite( 9, HIGH);
  //digitalWrite(11, HIGH);
  digitalWrite(HL, LOW);
  digitalWrite(13, LOW);

  Serial.begin(9600);

  //supplyDHT.begin();
  //spaceDHT.begin();

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
/*
  if(currentTime - previousTransmit > transmitInterval)
  {
    float supplyTemp = supplyDHT.readTemperature(true);
    float spaceTemp  = spaceDHT.readTemperature(true);
    float supplyHum = supplyDHT.readHumidity();
    float spaceHum  = spaceDHT.readHumidity();
    if(isnan(supplyTemp) || isnan(spaceTemp) || isnan(supplyHum) || isnan(spaceHum));
    else
    {
      Serial.print(supplyTemp); Serial.print(", "); Serial.print(supplyHum); Serial.print("; ");
      Serial.print(spaceTemp); Serial.print(", "); Serial.println(spaceHum);
      previousTransmit = currentTime;
    }
  }
*/

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
