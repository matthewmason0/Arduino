//TemperatureMonitor v1.0
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
     2           LCD D7
     3           LCD D6
     4           LCD D5
     5           LCD D4
     6           not used
     7           not used
     8           not used
     9           not used
    10           not used
    11           LCD ENABLE
    12           LCD RS
    13           not used
*/
const int waitMs = 1000;
//clock mechanism
unsigned long previousMillissecs = 0;
unsigned long hrs = 8;
unsigned long hrs2 = hrs;
unsigned long mins = 36;
unsigned long secs = 0;
unsigned long secs1 = 0;
//lcd
#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
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
byte c[8] = {
  B11100,
  B10000,
  B10000,
  B10000,
  B11100,
  B00000,
  B00000,
};
void setup()
  {
  Serial.begin(9600);
  lcd.createChar(0, degree);
  lcd.createChar(1, f);
  lcd.createChar(2, c);
  lcd.begin(16, 2);
  lcd.noBlink();
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  }
void loop()
  {
//sensor
  int sensorVal = analogRead(A0);
  float voltage = (sensorVal/1024.0) * 5.0;
  float cTemp = (voltage - .5) * 100;
  float fTemp = (cTemp * 9) / 5 + 32;
//clock mechanism
  unsigned long currentMillis = millis();
  secs1 = (currentMillis-previousMillissecs);
  secs = (secs1/1000);
  if(secs > 59)
    {
    previousMillissecs = currentMillis;
    secs = 0;
    mins = mins+1;
    }
  if(mins > 59)
    {
    mins = 0;
    hrs = (hrs2+1);
    }
  hrs2 = hrs;
  if(hrs > 12)
    {
    hrs = 1;
    }
//lcd
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(fTemp);
  lcd.setCursor(4,0);
  lcd.write(byte(0));
  lcd.setCursor(5,0);
  lcd.write(byte(1));
  lcd.setCursor(0,1);
  lcd.print(cTemp);
  lcd.setCursor(4,1);
  lcd.write(byte(0));
  lcd.setCursor(5,1);
  lcd.write(byte(2));
  lcd.setCursor(8,1);
  lcd.print(hrs);
  if(hrs>9)
    {
    lcd.setCursor(10,1);
    }
  else
    {
    lcd.setCursor(9,1);
    }
  lcd.print(":");
  if(hrs>9)
    {
    lcd.setCursor(11,1);
    if(mins<10)
      {
      lcd.print("0");
      lcd.setCursor(12,1);
      }
    }
  else
    {
    lcd.setCursor(10,1);
    if(mins<10)
      {
      lcd.print("0");
      lcd.setCursor(11,1);
      }
    }
  lcd.print(mins);
  if(hrs>9)
    {
    lcd.setCursor(13,1);
    }
  else
    {
    lcd.setCursor(12,1);
    }
  lcd.print(":");
  if(hrs>9)
    {
    lcd.setCursor(14,1);
    if(secs<10)
      {
      lcd.print("0");
      lcd.setCursor(15,1);
      }
    }
  else
    {
    lcd.setCursor(13,1);
    if(secs<10)
      {
      lcd.print("0");
      lcd.setCursor(14,1);
      }
    }
  lcd.print(secs);
  delay(waitMs);
  }
