#include <Servo.h>

Servo mlon;
Servo mloff;
Servo unlock;
Servo lock;

int mlon_range[]   = {0, 170}; //normal, activated
int mloff_range[]  = {0, 170};
int unlock_range[] = {0, 170};
int lock_range[]   = {0, 170};

int activation_time = 1500;

String command = "";
bool commandAvailable = false;

void setup()
{
  command.reserve(200);
  
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);

  Serial.begin(9600);

  mlon.attach(3);
  mloff.attach(4);
  unlock.attach(5);
  lock.attach(6);

  mlon.write(mlon_range[0]);
  mloff.write(mloff_range[0]);
  unlock.write(unlock_range[0]);
  lock.write(lock_range[0]);
}

void loop()
{
  if (digitalRead(2))
  {
    command = "mlon";
    commandAvailable = true;
  }
  
  if (commandAvailable)
  {
    if (command.equals("mlon"))
    {
      mlon.write(mlon_range[1]);
      delay (activation_time);
      mlon.write(mlon_range[0]);
    }
    if (command.equals("mloff"))
    {
      mloff.write(mloff_range[1]);
      delay (activation_time);
      mloff.write(mloff_range[0]);
    }
    if (command.equals("unlock"))
    {
      unlock.write(unlock_range[1]);
      delay (activation_time);
      unlock.write(unlock_range[0]);
    }
    if (command.equals("lock"))
    {
      lock.write(lock_range[1]);
      delay (activation_time);
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
