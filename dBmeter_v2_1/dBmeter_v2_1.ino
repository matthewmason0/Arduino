//dBmeter v2.1
  //Created by Matthew Mason,
  //February 2013

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
     7           (220-ohm Resistor)YellowLED2 + (25)
     8           (220-ohm Resistor)YellowLED3 + (31)
     9           (220-ohm Resistor)RedLED1 +    (38)
    10           (220-ohm Resistor)RedLED2 +    (46)
    11           not used
    12           not used
    13           not used
*/
//Pin #s
  const int led[11] = {0, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  int input = 0;
  
//Thresholds
  const int thresh1 = 1;
  const int thresh2 = 5;
  const int thresh3 = 10;
  const int thresh4 = 15;
  const int thresh5 = 20;
  const int thresh6 = 25;
  const int thresh7 = 31;
  const int thresh8 = 38;
  const int thresh9 = 46;

//Per-Light Delay System
  const int onTime = 8;
  const int onTimeLED1 = 80;
  unsigned long currentMillis = millis();
  unsigned long previousMillis[11];


void setup()
  {
  for(int pinNumber = 2; pinNumber<11; pinNumber++)
    {
    pinMode(pinNumber,OUTPUT);
    digitalWrite(pinNumber, LOW);
    }
  }

void loop()
  {
  input=analogRead(A0);
  if(input<=thresh1)
    {
    }
  if(input>thresh1)
    {
    digitalWrite(led[1], HIGH);
    previousMillis[1] = millis();
    }
  if(input>thresh2)
    {
    for(int i=1;i<3;i++)
      {
      digitalWrite(led[i], HIGH);
      previousMillis[i] = millis();
      }
    }
  if(input>thresh3)
    {
    for(int i=1;i<4;i++)
      {
      digitalWrite(led[i], HIGH);
      previousMillis[i] = millis();
      }
    }
  if(input>thresh4)
    {
    for(int i=1;i<5;i++)
      {
      digitalWrite(led[i], HIGH);
      previousMillis[i] = millis();
      }
    }
  if(input>thresh5)
    {
    for(int i=1;i<6;i++)
      {
      digitalWrite(led[i], HIGH);
      previousMillis[i] = millis();
      }
    }
  if(input>thresh6)
    {
    for(int i=1;i<7;i++)
      {
      digitalWrite(led[i], HIGH);
      previousMillis[i] = millis();
      }
    }
  if(input>thresh7)
    {
    for(int i=1;i<8;i++)
      {
      digitalWrite(led[i], HIGH);
      previousMillis[i] = millis();
      }
    }
  if(input>thresh8)
    {
    for(int i=1;i<9;i++)
      {
      digitalWrite(led[i], HIGH);
      previousMillis[i] = millis();
      }
    }
  if(input>thresh9)
    {
    for(int i=1;i<10;i++)
      {
      digitalWrite(led[i], HIGH);
      previousMillis[i] = millis();
      }
    }
  for(int i=2;i<10;i++)
    {
    currentMillis = millis();
    if(currentMillis-previousMillis[i]>onTime)
      {
      digitalWrite(led[i], LOW);
      }
    }
  currentMillis = millis();
  if(currentMillis-previousMillis[1]>onTimeLED1)
    {
    digitalWrite(led[1], LOW);
    }
  if(input>=1)
    {
    digitalWrite(13, HIGH);
    }
  else
    {
    digitalWrite(13, LOW);
    }    
  }
