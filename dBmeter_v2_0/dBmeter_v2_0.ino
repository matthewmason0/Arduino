//dBmeter v2.0
  //Created by Matthew Mason,
  //February 2014

/*
Pin Setup:
Analog Pin  | Connected to
     0           Audio Input (+5v)
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
  const int led[11] = {0, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  
//Thresholds
  const int thresh1 = 1;
  const int thresh2 = 5;
  const int thresh3 = 10;
  const int thresh4 = 15;
  const int thresh5 = 20;
  const int thresh6 = 25;
  const int thresh7 = 31;
  const int thresh8 = 37;
  const int thresh9 = 43;
  
//Delay between samples
  const int waitMs = 2;
  
//Number of samples
  const int numReadings = 2;
  
//Averaging system variables
  const int transistor = 11;
  int readings[numReadings];
  int index = 0;
  int total = 0;
  int average = 0;
  int indexLimit = numReadings+1;
  
//Per-Light Delay System
  const int onTime = 15;
  unsigned long currentMillis = millis();
  unsigned long previousMillis[11];


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
      }
    if(average>thresh1)
      {
      digitalWrite(led[1], HIGH);
      previousMillis[1] = millis();
      }
    if(average>thresh2)
      {
      for(int i=1;i<3;i++)
        {
        digitalWrite(led[i], HIGH);
        previousMillis[i] = millis();
        }
      }
    if(average>thresh3)
      {
      for(int i=1;i<4;i++)
        {
        digitalWrite(led[i], HIGH);
        previousMillis[i] = millis();
        }
      }
    if(average>thresh4)
      {
      for(int i=1;i<5;i++)
        {
        digitalWrite(led[i], HIGH);
        previousMillis[i] = millis();
        }
      }
    if(average>thresh5)
      {
      for(int i=1;i<6;i++)
        {
        digitalWrite(led[i], HIGH);
        previousMillis[i] = millis();
        }
      }
    if(average>thresh6)
      {
      for(int i=1;i<7;i++)
        {
        digitalWrite(led[i], HIGH);
        previousMillis[i] = millis();
        }
      }
    if(average>thresh7)
      {
      for(int i=1;i<8;i++)
        {
        digitalWrite(led[i], HIGH);
        previousMillis[i] = millis();
        }
      }
    if(average>thresh8)
      {
      for(int i=1;i<9;i++)
        {
        digitalWrite(led[i], HIGH);
        previousMillis[i] = millis();
        }
      }
    if(average>thresh9)
      {
      for(int i=1;i<10;i++)
        {
        digitalWrite(led[i], HIGH);
        previousMillis[i] = millis();
        }
      }
    digitalWrite(transistor, LOW);
    digitalWrite(transistor, HIGH);
    for(int i=1;i<10;i++)
      {
      currentMillis = millis();
      if(currentMillis-previousMillis[i]>onTime)
        {
        digitalWrite(led[i], LOW);
        }
      }
    delay(waitMs);
    index = 0;
    }
  }
