#include <Servo.h>

Servo mlon;
Servo mloff;
Servo unlock;
Servo lock;

void setup() {
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);

  Serial.begin(9600);

  mlon.attach(3);
  mloff.attach(4);
  unlock.attach(5);
  lock.attach(6);

  mlon.write(0);
}

void loop() {
    
  }
  
  delay(100);
}