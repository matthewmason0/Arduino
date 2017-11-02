#include <Servo.h>
Servo servo;

bool locked = false;

void setup() {
  pinMode(2, INPUT);
  pinMode(4, INPUT);
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  servo.attach(3);
  servo.write(0);
}

void loop() {
  if (digitalRead(2)) {
    lightsOn();
    delay(60000);
  }
  
  if (digitalRead(4))
    locked = true;
  else if(locked) {
      lightsOn();
      locked = false;
  }
  
  delay(100);
}

void lightsOn() {
  servo.write(170);
  delay(1500);
  servo.write(0);
}

