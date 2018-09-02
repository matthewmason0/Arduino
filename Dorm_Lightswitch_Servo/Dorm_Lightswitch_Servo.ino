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
int info[][4] = { { 2, 70,  130, 700 },
                  { 3, 90,  10,  600 },
                  { 4, 85,  120, 300 },
                  { 5, 90,  165, 300 },
                  { 6, 130, 170, 200 },
                  { 7, 90,  165, 300 } };

static constexpr int HL = 8;

static constexpr int SLEW_TIME = 500;

//DHT supplyDHT(12, DHT11);
//DHT spaceDHT(10, DHT11);

//unsigned long currentTime;
//unsigned long previousTransmit = 0;
//unsigned long transmitInterval = 2000;

String command = "";
bool commandAvailable = false;

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
/*
  currentTime = millis();
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
  
  if (commandAvailable)
  {
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
    commandAvailable = false;
  }
  
  delay(1);
}

void serialEvent()
{
  while (Serial.available())
  {
    char c = (char)Serial.read();
    command += c;
    if (c == '\n') {
      commandAvailable = true;
    }
  }
}
