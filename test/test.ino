#include <SoftwareSerialWithHalfDuplex.h>
#include <Mbus.h>
Mbus bus(3);
byte array[]={B00000001,B00001110,B00001111,B00000001};
void setup()
{
  Serial.begin(9600);
  pinMode(13,OUTPUT);
  digitalWrite(13,LOW);
}
void loop()
{
  bus.readPacketTest();
}
