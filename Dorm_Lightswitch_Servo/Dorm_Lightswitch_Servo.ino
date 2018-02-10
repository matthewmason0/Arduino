#include <Servo.h>
#include <DHT.h>

Servo mlon;
Servo mloff;
Servo unlock;
Servo lock;

DHT dht(12, DHT11);

static constexpr int MLON = 3;
static constexpr int MLOFF = 4;
static constexpr int UNLOCK = 5;
static constexpr int LOCK = 6;

int mlon_range[]   = {0, 170}; //normal, activated
int mloff_range[]  = {0, 165};
int unlock_range[] = {160, 0};
int lock_range[]   = {0, 170};

int slew_time = 400;

unsigned long currentTime;
unsigned long previousTransmit = 0;
unsigned long transmitInterval = 2000;

String command = "";
bool commandAvailable = false;

void setup()
{
  command.reserve(200);

  pinMode(11, OUTPUT);
  pinMode(13, OUTPUT);
  digitalWrite(11, HIGH);
  digitalWrite(13, LOW);

  Serial.begin(9600);
  
  dht.begin();
  
  mlon.attach(MLON);
  mloff.attach(MLOFF);
  unlock.attach(UNLOCK);
  lock.attach(LOCK);

  mlon.write(mlon_range[0]);
  mloff.write(mloff_range[0]);
  unlock.write(unlock_range[0]);
  lock.write(lock_range[0]);
  
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
    float temp = dht.readTemperature(true);
    float hum = dht.readHumidity();
    if(isnan(temp) || isnan(hum));
    else
    {
      Serial.print(temp); Serial.print(", "); Serial.println(hum);
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
      unlock.write(unlock_range[1]);
      mlon.write(mlon_range[1]);
      delay(slew_time);
      unlock.write(unlock_range[0]);
      mlon.write(mlon_range[0]);
      delay(slew_time);
      unlock.detach();
      mlon.detach();
    }
    if (command.equals("lockmloff"))
    {
      lock.attach(LOCK);
      mloff.attach(MLOFF);
      lock.write(lock_range[1]);
      mloff.write(mloff_range[1]);
      delay(slew_time);
      lock.write(lock_range[0]);
      mloff.write(mloff_range[0]);
      delay(slew_time);
      lock.detach();
      mloff.detach();
    }
    if (command.equals("mlon"))
    {
      mlon.attach(MLON);
      mlon.write(mlon_range[1]);
      delay(slew_time);
      mlon.write(mlon_range[0]);
      delay(slew_time);
      mlon.detach();
    }
    if (command.equals("mloff"))
    {
      mloff.attach(MLOFF);
      mloff.write(mloff_range[1]);
      delay(slew_time);
      mloff.write(mloff_range[0]);
      delay(slew_time);
      mloff.detach();
    }
    if (command.equals("unlock"))
    {
      unlock.attach(UNLOCK);
      unlock.write(unlock_range[1]);
      delay(slew_time);
      unlock.write(unlock_range[0]);
      delay(slew_time);
      unlock.detach();
    }
    if (command.equals("lock"))
    {
      lock.attach(LOCK);
      lock.write(lock_range[1]);
      delay(slew_time);
      lock.write(lock_range[0]);
      delay(slew_time);
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
