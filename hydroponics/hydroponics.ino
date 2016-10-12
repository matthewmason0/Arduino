/*
 hydroponics.ino
  Created by Matthew Mason,
  October 2016
  
  matthewmason0@gmail.com
*/

#include <Time.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(8,7,6,5,4,3);

time_t waitLength;
time_t runLength;

int offTime = 20; //minutes
int onTime = 10;  //minutes
int pump = 2;

void setup() {
  waitLength = offTime * 60; //time_t is in seconds
  runLength = onTime * 60;   
  
  lcd.begin(16, 2);
//Serial.begin(9600);
  
  pinMode(pump, OUTPUT);
  digitalWrite(pump, LOW);
  
//Serial.println("OFF");
}

void loop() {
  lcd.setCursor(1,0);
  lcd.print("Next");
  lcd.setCursor(0,1);
  lcd.print("Cycle: ");
  lcd.setCursor(13,0);
  lcd.print("OFF");
  time_t waitEnd = now() + waitLength;
  while(now() < waitEnd) //pump is off
  {
    time_t remaining = waitEnd - now();
    int minutes = minute(remaining);
    int seconds = second(remaining);
    lcd.setCursor(7,1);
    lcd.print(minutes);
    lcd.print(":");
    lcd.print(pad(seconds));
    lcd.print(" ");
    
//  Serial.print("Next Cycle: ");
//  Serial.print(minutes);
//  Serial.print(":");
//  Serial.println(pad(seconds));
    
    delay(1000);
  }
  digitalWrite(pump, HIGH);
  
//Serial.println("ON");

  lcd.setCursor(1,0);
  lcd.print("Time");
  lcd.setCursor(0,1);
  lcd.print(" Left: ");
  lcd.setCursor(13,0);
  lcd.print(" ON");
  time_t runEnd = now() + runLength;
  while(now() < runEnd) //pump is on
  {
    time_t remaining = runEnd - now();
    int minutes = minute(remaining);
    int seconds = second(remaining);
    lcd.setCursor(7,1);
    lcd.print(minutes);
    lcd.print(":");
    lcd.print(pad(seconds));
    lcd.print(" ");
    
//  Serial.print("Time Left: ");
//  Serial.print(minutes);
//  Serial.print(":");
//  Serial.println(pad(seconds));
    
    delay(1000);
  }
  digitalWrite(pump, LOW);
  
//Serial.println("OFF");
  
}

String pad(int s)
{
  String zero = "0";  //have to initialize strings first,
  String noZero = ""; //otherwise weird things happen
  if(s < 10)
    return zero + s;
  else
    return noZero + s;
}
