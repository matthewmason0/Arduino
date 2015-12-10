//MotorController v1.0
  //Created by Matthew Mason,
  //December 2013
/*
pin setup blah blah nobody reads this anyway - 
just figure it out on your own
*/
const int transistorPin = 3;
const int varResPin = A0;
int motorSpeed = 0;
void setup()
  {
  pinMode(transistorPin, OUTPUT);
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  }
void loop()
  {
  motorSpeed = analogRead(varResPin)/4;
  analogWrite(transistorPin, motorSpeed);
  }
//And that's it! Only 22 lines
