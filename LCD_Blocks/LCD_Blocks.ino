//LCD Blocks - a simple program
  //Created by Matthew Mason,
  //December 2013
const int wait = 50;
#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
byte block[8] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
};
byte empty[8] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
};
void setup()
  {
  lcd.createChar(0, block);
  lcd.createChar(1, empty);
  lcd.begin(16, 2);
  lcd.noBlink();
  }
void loop()
  {
  lcd.clear();
  for(int slot = 0; slot<16; slot++)
    {
    lcd. setCursor(slot,0);
    lcd.write(byte(0));
    delay(wait);
    }
  for(int slot = 0; slot<16; slot++)
    {
    lcd. setCursor(slot,1);
    lcd.write(byte(0));
    delay(wait);
    }
  for(int slot = 0; slot<16; slot++)
    {
    lcd. setCursor(slot,0);
    lcd.write(byte(1));
    delay(wait);
    }
  for(int slot = 0; slot<16; slot++)
    {
    lcd. setCursor(slot,1);
    lcd.write(byte(1));
    delay(wait);
    }
  }
