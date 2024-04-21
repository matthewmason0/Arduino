#include <SoftwareSerial.h>

SoftwareSerial mySerial(5, 6); // RX, TX

static constexpr char STX = 2; // beginning of start request, password to follow
static constexpr char ENQ = 5; // ping
static constexpr char ACK = 6; // reply
static constexpr char DC1 = 17; // request battery level
static constexpr char CAN = 24; // cancel

static constexpr char password[5] = "matt";
static constexpr int passwordLen = 4;

static constexpr unsigned long timeout = 20000; // ms

void setup() {
  Serial.begin(9600);
  mySerial.begin(1200);
  pinMode(4, OUTPUT);
  digitalWrite(4, 1);
}

void loop(){
  if (mySerial.available()){
    byte c = (byte)mySerial.read();
    if (c == ACK)
      Serial.println("ACK");
    else if (c > 127)
      Serial.println((int)(c - 128));
    else
      Serial.println(c);
  }
  if (Serial.available()){
    byte c = (byte)Serial.read();
    if (c == 'z')
    {
      mySerial.write(STX);
      for (int i = 0; i < passwordLen; i++)
      {
        delay(300);
        mySerial.write(password[i]);
        Serial.println(password[i]);
      }
      unsigned long requestTime = millis();
      while (millis() - requestTime < timeout)
      {
        if (mySerial.available())
        {
          byte c = (byte)mySerial.read();
          if (c == '1')
            Serial.println("Success!");
          else
            Serial.println("failed");
          break;
        }
      }
    }
    else if (c == 'x')
      mySerial.write(ENQ);
    else if (c == 'c')
      mySerial.write(DC1);
    else if (c == 'v')
      mySerial.write(CAN);
    else
      mySerial.write(c);
  }
}