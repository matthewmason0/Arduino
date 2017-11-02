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
      digitalWrite(13, HIGH);
      delay(500);
      if(digitalRead(4)) //if circuit reconnected within .5 s, ignore
        return;
      digitalWrite(13, LOW);
      delay(200);
      digitalWrite(13, HIGH);
      lightsOn();
      locked = false;
  }

  digitalWrite(13, LOW);
  
  delay(100);
}

void lightsOn() {
  servo.write(170);
  delay(1500);
  servo.write(0);
}

