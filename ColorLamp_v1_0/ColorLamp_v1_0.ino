//ColorLamp v1.0
  //Created by Matthew Mason,
  //November 2013

/*
Pin Setup:
Analog Pin  | Connected to
     0           not used
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
     6           not used
     7           not used
     8           not used
     9           Blue lead  \
    10           Green lead  > RGB LED(- lead conn. to GND)
    11           Red lead   /
    12           not used
    13           not used
*/
int red = 0;
int green = 0;
int blue = 0;
int rg = 0;
int wait = 10;
void setup()
  {
  for(int pinNum = 9; pinNum<12; pinNum++)
    {
    pinMode(pinNum,OUTPUT);
    digitalWrite(pinNum, LOW);
    }
  pinMode(13,OUTPUT);
  digitalWrite(13, LOW);
  }
void loop()
  {
  for(green = 0; green<=255; green++)
    {
    analogWrite(10, green);
    delay(wait);
    }
  for(blue = 255; blue>=0; blue--)
    {
    analogWrite(9, blue);
    delay(wait);
    }
  for(red = 0; red<=255; red++)
    {
    analogWrite(11, red);
    delay(wait);
    }
  for(green = 255; green>=0; green--)
    {
    analogWrite(10, green);
    delay(wait);
    }
  for(blue = 0; blue<=255; blue++)
    {
    analogWrite(9, blue);
    delay(wait);
    }
  for(green = 0; green<=255; green++)
    {
    analogWrite(10, green);
    delay(wait);
    }
  for(rg = 255; rg>=0; rg--)
    {
    analogWrite(10, rg);
    analogWrite(11, rg);
    delay(wait);
    }
  }
