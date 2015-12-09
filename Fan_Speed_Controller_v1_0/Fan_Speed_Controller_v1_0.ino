//Fan Speed Controller v1.0
  //Created by Matthew Mason,
  //November 2013

const int sensorPin = A0;
const int ledPin = 4;
int ledState = LOW;
long previousMs = 0;
long interval = 1000;

void setup()
  {
  Serial.begin(9600);
  pinMode(2,OUTPUT);
  pinMode(3,OUTPUT);
  pinMode(ledPin,OUTPUT);
  digitalWrite(2, LOW);
  digitalWrite(3, LOW);
  digitalWrite(ledPin, LOW);
  }

void loop()
  {
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
    digitalWrite(2, LOW);
    digitalWrite(3, HIGH);
    ledState = LOW;
    digitalWrite(ledPin, ledState);
    }
  else if(fTemp > 85)
    {
    digitalWrite(2, HIGH);
    digitalWrite(3, LOW);
    ledState = LOW;
    digitalWrite(ledPin, ledState);
    }
  else
    {
    digitalWrite(2, LOW);
    digitalWrite(3, LOW);
    ledState = LOW;
    digitalWrite(ledPin, ledState);
    }
  }
