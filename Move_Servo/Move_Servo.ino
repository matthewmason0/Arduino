#include <Servo.h>

Servo s;

String input = "";

void setup()
{
  for (int i = 2; i <= 10; i++)
    pinMode(i, OUTPUT);
  Serial.begin(9600);
}

void loop()
{
  for (int i = 2; i <= 10; i++)
    digitalWrite(i, LOW);
  bool init = true;
  while (init)
  {
    while (Serial.available())
    {
      char c = Serial.read();
      if (c == '\n')
      {
        s.attach(input.toInt());
        init = false;
        input = "";
      }
      else
        input += c;
    }
    delay(1);
  }
  bool run = true;
  while (run)
  {
    while (Serial.available())
    {
      char c = Serial.read();
      if (c == '\n')
      {
        s.writeMicroseconds(input.toInt());
        input = "";
      }
      else if (c == 'q')
      {
        run = false;
        break;
      }
      else
        input += c;
    }
    delay(1);
  }
  s.detach();
  input = "";
}
