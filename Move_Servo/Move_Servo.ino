#include <Servo.h>

Servo s;

String input = "";

bool run = false;
int step = 100;
float val = 800.0f;
int dir = 1;

void setup()
{
  for (int i = 2; i <= 10; i++)
    pinMode(i, OUTPUT);
  for (int i = 2; i <= 10; i++)
    digitalWrite(i, LOW);
  Serial.begin(9600);
  s.attach(6);
}

void loop()
{
  while (Serial.available())
  {
    char c = Serial.read();
    if (c == '\n')
    {
      int i = input.toInt();
      if (i > 0)
        step = i;
      // s.writeMicroseconds(input.toInt());
      input = "";
    }
    else if (c == 'q')
      run = !run;
    else
      input += c;
  }
  if (run)
  {
    if (val >= 1800)
      dir = -1;
    else if (val <= 800)
      dir = 1;
    val += 10.0f / step * dir;
    // Serial.print(val); Serial.print(" "); Serial.print(step); Serial.print(" "); Serial.println(dir);
    s.writeMicroseconds((int)val);
  }
  // else
    // s.writeMicroseconds(900);
  delay(1);
}
