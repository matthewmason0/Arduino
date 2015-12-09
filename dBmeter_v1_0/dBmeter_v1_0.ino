//dBmeter v1.0
  //Created by Matthew Mason,
  //November 2013

/*
Pin Setup:
Analog Pin  | Connected to
     0           Piezoelectric element + (w/Overcurrent of 5V)
     1           not used
     2           not used
     3           not used
     4           not used
     5           not used
Digital Pin | Connected to         |         Threshold
     0           not used
     1           not used
     2           (220-ohm Resistor)GreenLED1 +  (1)
     3           (220-ohm Resistor)GreenLED2 +  (5)
     4           (220-ohm Resistor)GreenLED3 +  (10)
     5           (220-ohm Resistor)GreenLED4 +  (15)
     6           (220-ohm Resistor)YellowLED1 + (20)
     7           (220-ohm Resistor)YellowLED2 + (24)
     8           (220-ohm Resistor)YellowLED3 + (28)
     9           (220-ohm Resistor)RedLED1 +    (32)
    10           (220-ohm Resistor)RedLED2 +    (35)
    11           Transistor
    12           not used
    13           not used
*/
//Pin #s
const int led1 = 2;
const int led2 = 3;
const int led3 = 4;
const int led4 = 5;
const int led5 = 6;
const int led6 = 7;
const int led7 = 8;
const int led8 = 9;
const int led9 = 10;
//Thresholds
const int thresh1 = 1;
const int thresh2 = 5;
const int thresh3 = 10;
const int thresh4 = 15;
const int thresh5 = 20;
const int thresh6 = 24;
const int thresh7 = 28;
const int thresh8 = 32;
const int thresh9 = 35;
//Delay between samples
const int waitMs = 10
//Number of samples
const int numReadings = 2;
//Averaging system variables
const int transistor = 11;
int readings[numReadings];
int index = 0;
int total = 0;
int average = 0;
int indexLimit = numReadings+1;

void setup()
  {
  Serial.begin(9600);
  for(int pinNumber = 2; pinNumber<11; pinNumber++)
    {
    pinMode(pinNumber,OUTPUT);
    digitalWrite(pinNumber, LOW);
    }
  pinMode(11,OUTPUT);
  digitalWrite(11, HIGH);
  for (int thisReading = 0; thisReading < 2; thisReading++)
    {
    readings[thisReading] = 0;
    }
  }

void loop()
  {
  total= total - readings[index];
  readings[index] = analogRead(A0);
  total= total + readings[index];
  index = index + 1;
  average = total / numReadings;
  if(index=indexLimit)
    {
    if(average<=1)
      {
      digitalWrite(led1, LOW);
      digitalWrite(led2, LOW);
      digitalWrite(led3, LOW);
      digitalWrite(led4, LOW);
      digitalWrite(led5, LOW);
      digitalWrite(led6, LOW);
      digitalWrite(led7, LOW);
      digitalWrite(led8, LOW);
      digitalWrite(led9, LOW);
      }
    if(average>thresh1)
      {
      digitalWrite(led1, HIGH);
      digitalWrite(led2, LOW);
      digitalWrite(led3, LOW);
      digitalWrite(led4, LOW);
      digitalWrite(led5, LOW);
      digitalWrite(led6, LOW);
      digitalWrite(led7, LOW);
      digitalWrite(led8, LOW);
      digitalWrite(led9, LOW);
      }
    if(average>thresh2)
      {
      digitalWrite(led1, HIGH);
      digitalWrite(led2, HIGH);
      digitalWrite(led3, LOW);
      digitalWrite(led4, LOW);
      digitalWrite(led5, LOW);
      digitalWrite(led6, LOW);
      digitalWrite(led7, LOW);
      digitalWrite(led8, LOW);
      digitalWrite(led9, LOW);
      }
    if(average>thresh3)
      {
      digitalWrite(led1, HIGH);
      digitalWrite(led2, HIGH);
      digitalWrite(led3, HIGH);
      digitalWrite(led4, LOW);
      digitalWrite(led5, LOW);
      digitalWrite(led6, LOW);
      digitalWrite(led7, LOW);
      digitalWrite(led8, LOW);
      digitalWrite(led9, LOW);
      }
    if(average>thresh4)
      {
      digitalWrite(led1, HIGH);
      digitalWrite(led2, HIGH);
      digitalWrite(led3, HIGH);
      digitalWrite(led4, HIGH);
      digitalWrite(led5, LOW);
      digitalWrite(led6, LOW);
      digitalWrite(led7, LOW);
      digitalWrite(led8, LOW);
      digitalWrite(led9, LOW);
      }
    if(average>thresh5)
      {
      digitalWrite(led1, HIGH);
      digitalWrite(led2, HIGH);
      digitalWrite(led3, HIGH);
      digitalWrite(led4, HIGH);
      digitalWrite(led5, HIGH);
      digitalWrite(led6, LOW);
      digitalWrite(led7, LOW);
      digitalWrite(led8, LOW);
      digitalWrite(led9, LOW);
      }
    if(average>thresh6)
      {
      digitalWrite(led1, HIGH);
      digitalWrite(led2, HIGH);
      digitalWrite(led3, HIGH);
      digitalWrite(led4, HIGH);
      digitalWrite(led5, HIGH);
      digitalWrite(led6, HIGH);
      digitalWrite(led7, LOW);
      digitalWrite(led8, LOW);
      digitalWrite(led9, LOW);
      }
    if(average>thresh7)
      {
      digitalWrite(led1, HIGH);
      digitalWrite(led2, HIGH);
      digitalWrite(led3, HIGH);
      digitalWrite(led4, HIGH);
      digitalWrite(led5, HIGH);
      digitalWrite(led6, HIGH);
      digitalWrite(led7, HIGH);
      digitalWrite(led8, LOW);
      digitalWrite(led9, LOW);
      }
    if(average>thresh8)
      {
      digitalWrite(led1, HIGH);
      digitalWrite(led2, HIGH);
      digitalWrite(led3, HIGH);
      digitalWrite(led4, HIGH);
      digitalWrite(led5, HIGH);
      digitalWrite(led6, HIGH);
      digitalWrite(led7, HIGH);
      digitalWrite(led8, HIGH);
      digitalWrite(led9, LOW);
      }
    if(average>thresh9)
      {
      digitalWrite(led1, HIGH);
      digitalWrite(led2, HIGH);
      digitalWrite(led3, HIGH);
      digitalWrite(led4, HIGH);
      digitalWrite(led5, HIGH);
      digitalWrite(led6, HIGH);
      digitalWrite(led7, HIGH);
      digitalWrite(led8, HIGH);
      digitalWrite(led9, HIGH);
      }
    digitalWrite(transistor, LOW);
    digitalWrite(transistor, HIGH);
    delay(wait);
    index = 0;
    }
  }
