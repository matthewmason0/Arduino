#include <Servo.h>
#include <DHT.h>

Servo mlon;
Servo mloff;
Servo unlock;
Servo lock;

DHT supplyDHT(12, DHT11);
DHT spaceDHT(10, DHT11);

static constexpr int MLON = 3;
static constexpr int MLOFF = 4;
static constexpr int UNLOCK = 5;
static constexpr int LOCK = 6;

int mlon_info[]   = {130, 170, 200}; //normal, activated, individual slew time
int mloff_info[]  = {90, 165,  300};
int unlock_info[] = {160, 0,   500};
int lock_info[]   = {0, 170,   400};

int slew_time = 500;

unsigned long currentTime;
unsigned long previousTransmit = 0;
unsigned long transmitInterval = 2000;

String command = "";
bool commandAvailable = false;

void setup()
{
  command.reserve(200);

  pinMode( 9, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(13, OUTPUT);
  digitalWrite( 9, HIGH);
  digitalWrite(11, HIGH);
  digitalWrite(13, LOW);

  Serial.begin(9600);
  
  supplyDHT.begin();
  spaceDHT.begin();
  
  mlon.attach(MLON);
  mloff.attach(MLOFF);
  unlock.attach(UNLOCK);
  lock.attach(LOCK);

  mlon.write(mlon_info[0]);
  mloff.write(mloff_info[0]);
  unlock.write(unlock_info[0]);
  lock.write(lock_info[0]);
  
  delay(slew_time);

  mlon.detach();
  mloff.detach();
  unlock.detach();
  lock.detach();
}

void loop()
{ 
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
      Serial.print(supplyTemp); Serial.print(", "); Serial.print(supplyHum); Serial.print(", ");
      Serial.print(spaceTemp); Serial.print(", "); Serial.println(spaceHum);
      previousTransmit = currentTime;
    }
  }
  
  if (commandAvailable)
  {
    command.trim();
    if (command.equals("unlockmlon"))
    {
      unlock.attach(UNLOCK);
      mlon.attach(MLON);
      unlock.write(unlock_info[1]);
      mlon.write(mlon_info[1]);
      delay(slew_time);
      unlock.write(unlock_info[0]);
      mlon.write(mlon_info[0]);
      delay(slew_time);
      unlock.detach();
      mlon.detach();
    }
    if (command.equals("lockmloff"))
    {
      lock.attach(LOCK);
      mloff.attach(MLOFF);
      lock.write(lock_info[1]);
      mloff.write(mloff_info[1]);
      delay(slew_time);
      lock.write(lock_info[0]);
      mloff.write(mloff_info[0]);
      delay(slew_time);
      lock.detach();
      mloff.detach();
    }
    if (command.equals("mlon"))
    {
      mlon.attach(MLON);
      mlon.write(mlon_info[1]);
      delay(mlon_info[2]);
      mlon.write(mlon_info[0]);
      delay(mlon_info[2]);
      mlon.detach();
    }
    if (command.equals("mloff"))
    {
      mloff.attach(MLOFF);
      mloff.write(mloff_info[1]);
      delay(mloff_info[2]);
      mloff.write(mloff_info[0]);
      delay(mloff_info[2]);
      mloff.detach();
    }
    if (command.equals("unlock"))
    {
      unlock.attach(UNLOCK);
      unlock.write(unlock_info[1]);
      delay(unlock_info[2]);
      unlock.write(unlock_info[0]);
      delay(unlock_info[2]);
      unlock.detach();
    }
    if (command.equals("lock"))
    {
      lock.attach(LOCK);
      lock.write(lock_info[1]);
      delay(lock_info[2]);
      lock.write(lock_info[0]);
      delay(lock_info[2]);
      lock.detach();
    }
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
