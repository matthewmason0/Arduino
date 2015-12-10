//Light Sweep v1.0
  //Created by Matthew Mason,
  //March 2013

/*
Pin Setup:
Analog Pin  | Connected to
     0           Light Sensor
     1           not used
     2           not used
     3           not used
     4           not used
     5           not used
Digital Pin | Connected to
     0           not used
     1           not used
     2           not used
     3           not used
     4           not used
     5           not used
     6           not used
     7           not used
     8           not used
     9           Servo SIG
    10           not used
    11           not used
    12           not used
    13           not used
*/
#include <Servo.h>
Servo myServo;
int degree[175];
int maximum = 0;
int maxDegree = 0;
int x = 0;
int y = 0;
int maximumNew = 0;

void moveServoL(int s)
{
  for(int i=0;i<175;i++)
  {
    myServo.write(i);
    delay(s);
  }
}

void moveServoR(int s)
{
  for(int i=174;i>=0;i--)
  {
    myServo.write(i);
    delay(s);
    degree[i] = analogRead(A0);
    Serial.println(degree[i]);
  }
}

void setup()
{
  myServo.attach(9);
  pinMode(13, OUTPUT);
  pinMode(11, OUTPUT);
  digitalWrite(13, LOW);
  
}

void loop()
{
  Serial.begin(9600);
  myServo.write(174);
  delay(500);
  moveServoR(25);
  maximum = 0;
  maxDegree = 0;
  for(int i=0;i<175;i++)
  {
    if(maximum==0)
    {
      x = degree[i];
      y = degree[i+1];
      maximum = max(x,y);
    }
    else
    {
      x = degree[i];
      maximumNew = max(x,maximum);
      if(maximumNew>maximum)
      {
        maxDegree = i;
        maximum = maximumNew;
      }
    }
  }
  myServo.write(maxDegree);
  Serial.print("maxDegree: ");
  Serial.println(maxDegree);
  Serial.print("value: ");
  Serial.print(maximum);
  delay(500);
  digitalWrite(11, HIGH);
  while(true) {}
  }
