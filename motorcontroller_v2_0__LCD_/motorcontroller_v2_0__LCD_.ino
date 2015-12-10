//MotorController v2.0 (w/LCD)
  //Created by Matthew Mason,
  //December 2013

/*
Pin Setup:
Analog Pin  | Connected to
     0           10K var res
     1           not used
     2           not used
     3           not used
     4           not used
     5           not used
Digital Pin | Connected to
     0           not used
     1           not used
     2           not used
     3           MOSFET
     4           LCD D7
     5           LCD D6
     6           LCD D5
     7           LCD D4
     8           not used
     9           not used
    10           not used
    11           LCD ENABLE
    12           LCD RS
    13           not used
LCD Ex.:
------------------
|2.7vlsD   Gate: |
|              9%|
------------------
*/
unsigned long startingMillis = 0;
unsigned long previousMillis = 0;
unsigned long mins = 0;
unsigned long secs = 0;
unsigned long secs1 = 0;
const int switchPin = 2;
const int transistorPin = 3;
const int varResPin = A0;
const int spoolDelay = 10;  //ms delay between each +1 of motor speed (out of 255) when spooling
const int inputVolts = 30;  //total external voltage input (multiple of 10 to work correctly)
int switchState = LOW;
int secsCursor = 0;
int newTime = 1;
int motorSpeed = 0;
int speedPercent = 0;
int volts = 0;
int untruncVolts = 0;
int truncVolts = 0;
int voltsDecimal = 0;
int voltsDiv10 = 0;
#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 7, 6, 5, 4);
byte vo[8] = {
  B10110,
  B10101,
  B01010,
  B00000,
  B00000,
  B00000,
  B00000,
};
byte lt[8] = {
  B10111,
  B10010,
  B11010,
  B00000,
  B00000,
  B00000,
  B00000,
};
byte s[8] = {
  B01100,
  B01000,
  B11000,
  B00000,
  B00000,
  B00000,
  B00000,
};
byte dc[8] = {
  B11001,
  B10110,
  B10110,
  B11001,
  B00000,
  B00000,
  B00000,
};
byte blank[8] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
};
byte percent[8] = {
  B11001,
  B11010,
  B00100,
  B01011,
  B10011,
  B00000,
  B00000,
};
byte rt[9] = {
  B11000,
  B10101,
  B11100,
  B10100,
  B00001,
  B11100,
  B01000,
  B01000,
};
void setup()
  {
  pinMode(switchPin, INPUT);
  pinMode(transistorPin, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(10, OUTPUT);
  digitalWrite(13, LOW);
  lcd.createChar(0, vo);
  lcd.createChar(1, lt);
  lcd.createChar(2, s);
  lcd.createChar(3, dc);
  lcd.createChar(4, blank);
  lcd.createChar(5, percent);
  lcd.createChar(6, rt);
  lcd.begin(16, 2);
  lcd.noBlink();
  lcd.setCursor(10,0);
  lcd.print("Gate:");
  lcd.setCursor(10,1);
  lcd.print("CLOSED");
  analogWrite(10, 230);
  }
void loop()
  {
  motorSpeed = analogRead(varResPin)/4;
  analogWrite(transistorPin, motorSpeed);
  speedPercent = motorSpeed*100/255;
  voltsDiv10 = inputVolts/10;
  volts = speedPercent*voltsDiv10/10;
  untruncVolts = speedPercent*voltsDiv10;
  truncVolts = volts*10;
  voltsDecimal = untruncVolts-truncVolts;
  lcd.setCursor(0,0);
  lcd.print(volts);  
  if(volts<10)
    {
    lcd.setCursor(1,0);
    lcd.print(".");
    lcd.setCursor(2,0);
    lcd.print(voltsDecimal);
    for(int number = 0; number<5; number++)
      {
      lcd.setCursor(number+3,0);
      lcd.write(byte(number));
      }
    }
  else
    {
    lcd.setCursor(2,0);
    lcd.print(".");
    lcd.setCursor(3,0);
    lcd.print(voltsDecimal);
    for(int number = 0; number<4; number++)
      {
      lcd.setCursor(number+4,0);
      lcd.write(byte(number));
      }
    }
  if(speedPercent>9)
    {
    if(speedPercent>99)
      {
      for(int number = 10; number<12; number++)
        {
        lcd.setCursor(number,1);
        lcd.write(byte(4));
        }
      lcd.setCursor(12,1);
      lcd.print(speedPercent);
      lcd.setCursor(15,1);
      lcd.write(byte(5));
      }
    else
      {
      for(int number = 10; number<13; number++)
        {
        lcd.setCursor(number,1);
        lcd.write(byte(4));
        }
      lcd.setCursor(13,1);
      lcd.print(speedPercent);
      lcd.setCursor(15,1);
      lcd.write(byte(5));
      }
    }
  else
    {
    if(speedPercent == 0)
      {
      lcd.setCursor(10,1);
      lcd.print("CLOSED");
      }
    else
      {
      for(int number = 10; number<14; number++)
        {
        lcd.setCursor(number,1);
        lcd.write(byte(4));
        }
      lcd.setCursor(14,1);
      lcd.print(speedPercent);
      lcd.setCursor(15,1);
      lcd.write(byte(5));
      }
    }
  if(speedPercent<100)
    {
    switchState = digitalRead(switchPin);
    if(switchState == HIGH)
      {
      lcd.setCursor(0,1);
      lcd.print("Spooling..");
      for(motorSpeed = motorSpeed; motorSpeed<256; motorSpeed++)
        {
        analogWrite(transistorPin, motorSpeed);
        speedPercent = motorSpeed*100/255;
        voltsDiv10 = inputVolts/10;
        volts = speedPercent*voltsDiv10/10;
        untruncVolts = speedPercent*voltsDiv10;
        truncVolts = volts*10;
        voltsDecimal = untruncVolts-truncVolts;
        lcd.setCursor(0,0);
        lcd.print(volts);  
        if(volts<10)
          {
          lcd.setCursor(1,0);
          lcd.print(".");
          lcd.setCursor(2,0);
          lcd.print(voltsDecimal);
          for(int number = 0; number<5; number++)
            {
            lcd.setCursor(number+3,0);
            lcd.write(byte(number));
            }
          }
        else
          {
          lcd.setCursor(2,0);
          lcd.print(".");
          lcd.setCursor(3,0);
          lcd.print(voltsDecimal);
          for(int number = 0; number<4; number++)
            {
            lcd.setCursor(number+4,0);
            lcd.write(byte(number));
            }
          }
        if(speedPercent>9)
          {
        if(speedPercent>99)
            {
            for(int number = 10; number<12; number++)
              {
              lcd.setCursor(number,1);
              lcd.write(byte(4));
              }
            lcd.setCursor(12,1);
            lcd.print(speedPercent);
            lcd.setCursor(15,1);
            lcd.write(byte(5));
            }
          else
            {
            for(int number = 10; number<13; number++)
              {
              lcd.setCursor(number,1);
              lcd.write(byte(4));
              }
            lcd.setCursor(13,1);
            lcd.print(speedPercent);
            lcd.setCursor(15,1);
            lcd.write(byte(5));
            }
          }
        else
          {
          if(speedPercent == 0)
            {
            lcd.setCursor(10,1);
            lcd.print("CLOSED");
            }
          else
            {
            for(int number = 10; number<14; number++)
              {
              lcd.setCursor(number,1);
              lcd.write(byte(4));
              }
            lcd.setCursor(14,1);
            lcd.print(speedPercent);
            lcd.setCursor(15,1);
            lcd.write(byte(5));
            }
          }
        delay(spoolDelay);
        }
      previousMillis = millis();
      while(analogRead(varResPin)<1020)
        {
        lcd.setCursor(0,1);
        lcd.print("Turn it up!");
        }
      for(int number = 4; number<10; number++)
        {
        lcd.setCursor(number,1);
        lcd.write(byte(4));
        }
      newTime = 0;
      }
    }
  if(speedPercent == 100)
    {
    unsigned long currentMillis = millis();
    if(newTime == 1)
      {
      previousMillis = currentMillis;
      mins = 0;
      newTime = 0;
      }
    secs1 = (currentMillis-previousMillis);
    secs = (secs1/1000);
    if(secs > 59)
      {
      previousMillis = currentMillis;
      secs = 0;
      mins = mins+1;
      }
    lcd.setCursor(0,1);
    lcd.write(byte(6));
    lcd.setCursor(1,1);
    lcd.print(mins);
    if(mins>9)
      {
      if(mins>99)
        {
        lcd.setCursor(4,1);
        }
      else
        {
        lcd.setCursor(3,1);
        }
      }
    else
      {
      lcd.setCursor(2,1);
      }
    lcd.print(":");
    if(mins>9)
      {
      if(mins>99)
        {
        secsCursor = 5;
        }
      else
        {
        secsCursor = 4;
        }
      }
    else
      {
      secsCursor = 3;
      }
    lcd.setCursor(secsCursor,1);
    if(secs<10)
      {
      lcd.print("0");
      lcd.setCursor(secsCursor+1,1);
      }
    lcd.print(secs);
    }
  else
    {
    for(int number = 0; number<10; number++)
      {
      lcd.setCursor(number,1);
      lcd.write(byte(4));
      }
    newTime = 1;
    }
  }
