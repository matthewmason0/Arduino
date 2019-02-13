#include <Servo.h>

Servo s;

String input = "";

void setup()
{
  for (int i = 2; i <= 7; i++)
  {
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }
  Serial.begin(9600);
}

void loop()
{
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
        s.write(input.toInt());
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
