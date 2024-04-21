#include <Servo.h>

Servo s1;
Servo s2;

String input = "";

void up()
{
  for (int offset = 0; offset <= 500; offset += 1)
  {
    s1.writeMicroseconds(1500 + offset);
    s2.writeMicroseconds(1500 + offset);
    delay(5);
  }
}

void down()
{
  for (int offset = 0; offset <= 500; offset += 1)
  {
    s1.writeMicroseconds(1500 - offset);
    s2.writeMicroseconds(1500 - offset);
    delay(5);
  }
}

void setup()
{
  s1.attach(2);
  s2.attach(3);
  Serial.begin(9600);
}

void loop()
{
  while (Serial.available())
  {
    char c = Serial.read();
    if (c == '\n')
    {
      if(input.length() != 0)
      {
        int offset = input.toInt();
        s1.writeMicroseconds(1500 + offset);
        s2.writeMicroseconds(1500 + offset);
      }
      input = "";
    }
    else if (c == 'u')
    {
      up();
    }
    else if (c == 'd')
    {
      down();
    }
    else if (c == '0')
    {
      s1.writeMicroseconds(1500);
      s2.writeMicroseconds(1500);
    }
    else
      input += c;
  }
  delay(1);
}
