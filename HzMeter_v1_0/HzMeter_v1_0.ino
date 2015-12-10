  //HzMeter v1.0
  //Created by Matthew Mason,
  //February 2013

/*
Pin Setup:
Analog Pin  | Connected to
     0           Audio Input (+5v~)
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
    11           Sig LED
    12           not used
    13           not used
*/
//Pin #s
  const int led[11] = {0, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  int input = 0;
  
//Thresholds
  const int thresh1 = 2;
  const int thresh2 = 3;
  const int thresh3 = 4;
  const int thresh4 = 5;
  const int thresh5 = 6;
  const int thresh6 = 7;
  const int thresh7 = 8;
  const int thresh8 = 9;
  const int thresh9 = 10;

//Per-Light Delay System
  const int onTime = 20;
  const int onTimeLED1 = 25;
  unsigned long currentMillis = millis();
  unsigned long previousMillis[11];

//Frequency System
  unsigned long zeroCurrent = millis();
  unsigned long zeroPrev = millis();
  unsigned long freq = 0;


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
  analogWrite(11,input*4);
  if(input>1)
    {
    while(input>1)
      {
      input=analogRead(A0);
      analogWrite(11,input*4);
      zeroCurrent = millis();
      freq = zeroCurrent-zeroPrev;
      if(freq<=thresh1)
        {
        }
      if(freq>thresh1)
        {
        digitalWrite(led[1], HIGH);
        previousMillis[1] = millis();
        }
      if(freq>thresh2)
        {
        for(int i=1;i<3;i++)
          {
          digitalWrite(led[i], HIGH);
          previousMillis[i] = millis();
          }
        }
      if(freq>thresh3)
        {
        for(int i=1;i<4;i++)
          {
          digitalWrite(led[i], HIGH);
          previousMillis[i] = millis();
          }
        }
      if(freq>thresh4)
        {
        for(int i=1;i<5;i++)
          {
          digitalWrite(led[i], HIGH);
          previousMillis[i] = millis();
          }
        }
      if(freq>thresh5)
        {
        for(int i=1;i<6;i++)
          {
          digitalWrite(led[i], HIGH);
          previousMillis[i] = millis();
          }
        }
      if(freq>thresh6)
        {
        for(int i=1;i<7;i++)
          {
          digitalWrite(led[i], HIGH);
          previousMillis[i] = millis();
          }
        }
      if(freq>thresh7)
        {
        for(int i=1;i<8;i++)
          {
          digitalWrite(led[i], HIGH);
          previousMillis[i] = millis();
          }
        }
      if(freq>thresh8)
        {
        for(int i=1;i<9;i++)
          {
          digitalWrite(led[i], HIGH);
          previousMillis[i] = millis();
          }
        }
      if(freq>thresh9)
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
      }
    }
  zeroCurrent = millis();
  freq = zeroCurrent-zeroPrev;
  if(freq<=thresh1)
    {
    }
  if(freq>thresh1)
    {
    digitalWrite(led[1], HIGH);
    previousMillis[1] = millis();
    }
  if(freq>thresh2)
    {
    for(int i=1;i<3;i++)
      {
      digitalWrite(led[i], HIGH);
      previousMillis[i] = millis();
      }
    }
  if(freq>thresh3)
    {
    for(int i=1;i<4;i++)
      {
      digitalWrite(led[i], HIGH);
      previousMillis[i] = millis();
      }
    }
  if(freq>thresh4)
    {
    for(int i=1;i<5;i++)
      {
      digitalWrite(led[i], HIGH);
      previousMillis[i] = millis();
      }
    }
  if(freq>thresh5)
    {
    for(int i=1;i<6;i++)
      {
      digitalWrite(led[i], HIGH);
      previousMillis[i] = millis();
      }
    }
  if(freq>thresh6)
    {
    for(int i=1;i<7;i++)
      {
      digitalWrite(led[i], HIGH);
      previousMillis[i] = millis();
      }
    }
  if(freq>thresh7)
    {
    for(int i=1;i<8;i++)
      {
      digitalWrite(led[i], HIGH);
      previousMillis[i] = millis();
      }
    }
  if(freq>thresh8)
    {
    for(int i=1;i<9;i++)
      {
      digitalWrite(led[i], HIGH);
      previousMillis[i] = millis();
      }
    }
  if(freq>thresh9)
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
  zeroPrev = millis();
  }
