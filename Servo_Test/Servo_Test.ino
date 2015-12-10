/*
  Arduino Starter Kit example
 Project 5  - Servo Mood Indicator
 
 This sketch is written to accompany Project 5 in the
 Arduino Starter Kit
 
 Parts required:
 servo motor 
 10 kilohm potentiometer 
 2 100 uF electrolytic capacitors
 
 Created 13 September 2012
 by Scott Fitzgerald
 
 http://arduino.cc/starterKit
 
 This example code is part of the public domain 
 */
 
#include <Servo.h>
Servo myServo;

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
  }
}

void setup()
{
  myServo.attach(9);
}

void loop() 
{
  myServo.write(0);
  delay(500);
  moveServoL(200);
  moveServoR(100);
  moveServoL(50);
  moveServoR(25);
  moveServoL(15);
  moveServoR(10);
  moveServoL(5);
  moveServoR(3);
  while(true) {}
}
