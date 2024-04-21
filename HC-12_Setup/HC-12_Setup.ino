#include <SoftwareSerial.h>

SoftwareSerial mySerial(5, 6); // RX, TX

static constexpr int SET = 9;

void setup() {
  Serial.begin(9600);
  Serial.println("Enter AT commands:");
  mySerial.begin(1200);
  pinMode(SET, OUTPUT);
  digitalWrite(SET, 1);
  delay(3000);
  digitalWrite(SET, 0);
  delay(40);
  mySerial.println("AT+SLEEP");
  digitalWrite(SET, 1);
  delay(5000);
  digitalWrite(SET, 0);
  delay(40);
  digitalWrite(SET, 1);
}

uint32_t time = 0;
bool waiting = false;

void loop(){
  if (mySerial.available()){
    uint8_t c = mySerial.read();
    Serial.write(c);
    if (c == 'P')
    {
      // digitalWrite(SET, 1);
      // Serial.println("exit");
      // delay(5000);
      // digitalWrite(SET, 0);
      // delay(40);
      // digitalWrite(SET, 1);
    }
  }
  if (Serial.available()){
    mySerial.write(Serial.read());
    // if (c == '\n')
    // {
    //   time = millis();
    //   waiting = true;
    // }
  }
  // if (waiting && ((millis() - time) > 5000))
  // {
  //   waiting = false;
  //   digitalWrite(SET, 1);
  // }
}
