//StrobeLight v1.0
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
     2           Red LED1
     3           Green LED1
     4           Blue LED1
     5           White LED
     6           Red LED2
     7           Green LED2
     8           Blue LED2
     9           not used
    10           not used
    11           not used
    12           not used
    13           not used
*/
const int onMs = 2;
const int offMs = 100;
int pin = 2;
void setup()
  {
  for(pin = 2; pin<9; pin++)
    {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
    }
  }
void loop()
  {
  for(pin = 2; pin<9; pin++)
    {
    digitalWrite(pin, HIGH);
    }
  delay(onMs);
  for(pin = 2; pin<9; pin++)
    {
    digitalWrite(pin, LOW);
    }
  delay(offMs);
  }
