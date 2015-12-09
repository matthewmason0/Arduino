//Fan Speed Controller v2.0
  //Created by Matthew Mason,
  //November 2013

/*
Pin Setup:
Analog Pin  | Connected to
     0           Temperature Sensor
     1           not used
     2           not used
     3           not used
     4           not used
     5           not used
Digital Pin | Connected to
     0           not used
     1           not used
     2           not used
     3           not used
     4           not used
     5           not used
     6           Transistor - White to HIGH
     7           Transistor - HIGH to HIGH
     8           220 Resistor > LED > GND
     9           not used
    10           not used
    11           not used
    12           not used
    13           Program Status LED (onboard)
*/
const int sensorPin = A0;
const int ledPin = 8;
int ledState = LOW;
long previousMs = 0;
long interval = 1000;

void setup()
  {
  Serial.begin(9600);
  pinMode(6,OUTPUT);
  pinMode(7,OUTPUT);
  pinMode(ledPin,OUTPUT);
  pinMode(13,OUTPUT);
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
  digitalWrite(ledPin, LOW);
  digitalWrite(13,HIGH);
  }

void loop()
  {
//status LED
  digitalWrite(13,LOW);
  delay(200);
  digitalWrite(13,HIGH);
  
//sensor
  int sensorVal = analogRead(sensorPin);
  Serial.print("sensor Value: ");
  Serial.print(sensorVal); 
  float voltage = (sensorVal/1024.0) * 5.0;
  Serial.print(", Volts: ");
  Serial.print(voltage);
  Serial.print(", degrees C: "); 
  float cTemp = (voltage - .5) * 100;
  float fTemp = (cTemp * 9) / 5 + 32;
  Serial.println(fTemp);
  
//conditionals
  if(fTemp > 93)
    {
    unsigned long currentMs = millis();
    if(currentMs - previousMs > interval)
      {
      previousMs = currentMs;
      if(ledState == LOW)
        {
        ledState = HIGH;
        }
      else
        {
        ledState = LOW;
        }
      digitalWrite(ledPin, ledState);
      }
    }
  else if(fTemp > 92)
    {
    digitalWrite(6, LOW);
    digitalWrite(7, HIGH);
    ledState = LOW;
    digitalWrite(ledPin, ledState);
    }
  else if(fTemp > 85)
    {
    digitalWrite(6, HIGH);
    digitalWrite(7, LOW);
    ledState = LOW;
    digitalWrite(ledPin, ledState);
    }
  else
    {
    digitalWrite(6, LOW);
    digitalWrite(7, LOW);
    ledState = LOW;
    digitalWrite(ledPin, ledState);
    }
  }
