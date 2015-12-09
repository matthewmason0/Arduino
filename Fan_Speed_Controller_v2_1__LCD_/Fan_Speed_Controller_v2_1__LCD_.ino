//Fan Speed Controller v2.1 (w/LCD)
  //Created by Matthew Mason,
  //November 2013

/*
Pin Setup:
**Refer to LCD_Circut.png or LCD_Circut.docx for circut diagram
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
     2           LCD D7
     3           LCD D6
     4           LCD D5
     5           LCD D4
     6           Transistor - White to HIGH
     7           Transistor - HIGH to HIGH
     8           220 Resistor > LED > GND
     9           not used
    10           not used
    11           LCD ENABLE
    12           LCD RS
    13           Program Status LED (onboard)
*/
//pins
const int sensorPin = A0;
const int ledPin = 8;
//led
int ledState = LOW;
unsigned long previousMs = 0;
unsigned long interval = 1000;
//lcd
#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
int fanspeed = LOW;
char fanSpeed[5] = "LOW";
int ynWarning;
char warning[2] = "n";
//custom chars
byte degree[8] = {
  B01110,
  B01010,
  B01110,
  B00000,
  B00000,
  B00000,
  B00000,
};
byte f[8] = {
  B11100,
  B10000,
  B11000,
  B10000,
  B10000,
  B00000,
  B00000,
};

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
  digitalWrite(13,LOW);
  lcd.createChar(0, degree);
  lcd.createChar(1, f);
  lcd.begin(16, 2);
  lcd.noBlink();
  }

void loop()
  {
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
    char fanSpeed[5] = "HIGH";
    char warning[2] = "y";
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
    char fanSpeed[5] = "HIGH";
    char warning[2] = "n";
    digitalWrite(6, LOW);
    digitalWrite(7, HIGH);
    ledState = LOW;
    digitalWrite(ledPin, ledState);
    }
  else if(fTemp > 85)
    {
    char fanSpeed[5] = "MED";
    char warning[2] = "n";
    digitalWrite(6, HIGH);
    digitalWrite(7, LOW);
    ledState = LOW;
    digitalWrite(ledPin, ledState);
    }
  else
    {
    char fanSpeed[5] = "LOW";
    char warning[2] = "n";
    digitalWrite(6, LOW);
    digitalWrite(7, LOW);
    ledState = LOW;
    digitalWrite(ledPin, ledState);
    }
    
//lcd
  fanspeed = fanSpeed[5];
  ynWarning = warning[2];
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(fTemp);
  lcd.setCursor(5,0);
  lcd.write(byte(0));
  lcd.setCursor(6,0);
  lcd.write(byte(1));
  lcd.setCursor(0,1);
  lcd.print(fanspeed);
  if(warning == "y")
    {
    lcd.setCursor(5,1);
    lcd.print("WARNING HOT");
    }
  }
