#include <Servo.h>

Servo s;

String input = "";

int currentPos = 1800;
int targetPos = 1800;
int delayTime = 4;
int extra = 100;

void moveServo(int pos)
{
    targetPos = pos;
    Serial.print(currentPos); Serial.print(" "); Serial.println(targetPos);
    if (targetPos == currentPos)
        return;

    bool dir = targetPos > currentPos;
    int extraPos = targetPos + (dir ? extra : -extra);

    // go to extraPos first
    s.attach(6);
    while (currentPos != extraPos)
    {
        currentPos += dir ? 1 : -1;
        s.writeMicroseconds(currentPos);
        delay(delayTime);
    }
    // come back to targetPos
    while (currentPos != targetPos)
    {
        currentPos += dir ? -1 : 1;
        s.writeMicroseconds(currentPos);
        delay(delayTime);
    }
    s.detach();
}

void setup()
{
    pinMode(6, OUTPUT);
    s.attach(6);
    s.writeMicroseconds(currentPos);
    Serial.begin(9600);
}

void loop()
{
    while (Serial.available())
    {
        char c = Serial.read();
        if (c == '\n')
        {
            int i = input.toInt();
            if (i >= 0)
                moveServo(map(i, 0, 100, 800, 1800));
            input = "";
        }
        else if (c == 'u')
            delayTime += 1;
        else if (c == 'd')
            delayTime -= 1;
        else
            input += c;
        Serial.println(input);
    }
}
