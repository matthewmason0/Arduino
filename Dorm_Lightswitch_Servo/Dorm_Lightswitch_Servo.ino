#include <Servo.h>
Servo servo;

void setup() {
  pinMode(2, INPUT);
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  servo.attach(3);
  servo.write(0);
}

void loop() {
  if (digitalRead(2)) {
    servo.write(180);
    delay(1500);
    servo.write(0);
    delay(1000*60);
  }
  delay(100);
}
