#include <Servo.h>
#include <DHT.h>

Servo mlon;
Servo mloff;
Servo unlock;
Servo lock;

DHT dht(12, DHT11);

int mlon_range[]   = {0, 170}; //normal, activated
int mloff_range[]  = {0, 165};
int unlock_range[] = {160, 0};
int lock_range[]   = {0, 170};

int activation_time = 1500;

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

  mlon.attach(3);
  mloff.attach(4);
  unlock.attach(5);
  lock.attach(6);

  dht.begin();

  mlon.write(mlon_range[0]);
  mloff.write(mloff_range[0]);
  unlock.write(unlock_range[0]);
  lock.write(lock_range[0]);

  unlock.detach();
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
      unlock.attach(5);
      unlock.write(unlock_range[1]);
      mlon.write(mlon_range[1]);
      delay(activation_time);
      unlock.write(unlock_range[0]);
      mlon.write(mlon_range[0]);
      unlock.detach();
    }
    if (command.equals("lockmloff"))
    {
      lock.write(lock_range[1]);
      mloff.write(mloff_range[1]);
      delay(activation_time);
      lock.write(lock_range[0]);
      mloff.write(mloff_range[0]);
    }
    if (command.equals("mlon"))
    {
      mlon.write(mlon_range[1]);
      delay(activation_time);
      mlon.write(mlon_range[0]);
    }
    if (command.equals("mloff"))
    {
      mloff.write(mloff_range[1]);
      delay(activation_time);
      mloff.write(mloff_range[0]);
    }
    if (command.equals("unlock"))
    {
      unlock.attach(5);
      unlock.write(unlock_range[1]);
      delay(activation_time);
      unlock.write(unlock_range[0]);
      unlock.detach();
    }
    if (command.equals("lock"))
    {
      lock.write(lock_range[1]);
      delay(activation_time);
      lock.write(lock_range[0]);
    }
    command = "";
    commandAvailable = false;
  }
  
  delay(10);
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
