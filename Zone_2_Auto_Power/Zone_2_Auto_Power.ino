#include <SoftwareSerial.h>

SoftwareSerial swSerial(10, 11);

String readString;

bool readNext = false;
bool ignoreNextZ2ON = false;

void setup()
{
  pinMode(3, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  analogWrite(3, 95);
  digitalWrite(12, LOW);
  digitalWrite(13, LOW);

  swSerial.begin(9600);
  Serial.begin(9600);
}

void loop()
{
  while (swSerial.available() > 0)
  {
    char c = swSerial.read();

    //null without \r indicates power state change
    if (c == 0)
      continue; //don't put a null the (null-terminated) string

    if (c == '\r')
    {
      Serial.println(readString);
      //second Z2ON from standby
      if (ignoreNextZ2ON && readString.equals("Z2ON"))
      {
        readNext = false;
        ignoreNextZ2ON = false;
      }
      //second PWON from standby
      else if (ignoreNextZ2ON && readString.equals("PWON"))
        readNext = false;
      //PWSTANDBY w/ no Z2OFF bug fix
      else if (ignoreNextZ2ON && readString.equals("PWSTANDBY"))
      {
        readNext = false;
        ignoreNextZ2ON = false;
        sendPowerButton();
      }
      //first Z2ON from standby
      else if (readNext && readString.equals("Z2ON"))
      {
        readNext = false;
        ignoreNextZ2ON = true;
        sendPowerButton();
      }
      //first PWON from standby
      else if (readString.indexOf("PWON") >= 0) //contains
      {
        readNext = true;
        ignoreNextZ2ON = false;
      }
      //normal (main zone already on)
      else if (readString.equals("Z2ON") || readString.equals("Z2OFF"))
      {
        readNext = false;
        ignoreNextZ2ON = false;
        sendPowerButton();
      }

      readString = "";
    }
    else
      readString += c;
  }

  delay(1);
}

// This procedure sends a 38KHz pulse to the IRledPin
// for a certain # of microseconds. We'll use this whenever we need to send codes
void pulseIR(long microsecs)
{
  // we'll count down from the number of microseconds we are told to wait

  cli();  // this turns off any background interrupts

  while (microsecs > 0)
  {
    // 38 kHz is about 13 microseconds high and 13 microseconds low
   digitalWrite(12, HIGH);  // this takes about 3 microseconds to happen
   delayMicroseconds(10);         // hang out for 10 microseconds
   digitalWrite(12, LOW);   // this also takes about 3 microseconds
   delayMicroseconds(10);         // hang out for 10 microseconds

   // so 26 microseconds altogether
   microsecs -= 26;
  }

  sei();  // this turns them back on
}

void sendPowerButton()
{
  for(int i = 0; i < 2; i++)
  {
    pulseIR(8800);
    delayMicroseconds(4400);

    pulseIR(550);
    delayMicroseconds(550);
    pulseIR(550);
    delayMicroseconds(550);
    pulseIR(550);
    delayMicroseconds(550);
    pulseIR(550);
    delayMicroseconds(1650);
    pulseIR(550);
    delayMicroseconds(1650);
    pulseIR(550);
    delayMicroseconds(1650);
    pulseIR(550);
    delayMicroseconds(550);
    pulseIR(550);
    delayMicroseconds(1650);
    pulseIR(550);
    delayMicroseconds(1650);
    pulseIR(550);
    delayMicroseconds(1650);
    pulseIR(550);
    delayMicroseconds(1650);
    pulseIR(550);
    delayMicroseconds(550);
    pulseIR(550);
    delayMicroseconds(550);
    pulseIR(550);
    delayMicroseconds(550);
    pulseIR(550);
    delayMicroseconds(1650);
    pulseIR(550);
    delayMicroseconds(550);
    pulseIR(550);
    delayMicroseconds(1650);
    pulseIR(550);
    delayMicroseconds(550);
    pulseIR(550);
    delayMicroseconds(1650);
    pulseIR(550);
    delayMicroseconds(1650);
    pulseIR(550);
    delayMicroseconds(1650);
    pulseIR(550);
    delayMicroseconds(550);
    pulseIR(550);
    delayMicroseconds(550);
    pulseIR(550);
    delayMicroseconds(1650);
    pulseIR(550);
    delayMicroseconds(550);
    pulseIR(550);
    delayMicroseconds(1650);
    pulseIR(550);
    delayMicroseconds(550);
    pulseIR(550);
    delayMicroseconds(550);
    pulseIR(550);
    delayMicroseconds(550);
    pulseIR(550);
    delayMicroseconds(1650);
    pulseIR(550);
    delayMicroseconds(1650);
    pulseIR(550);
    delayMicroseconds(550);

    pulseIR(550);
    delayMicroseconds(38500);
  }
}
