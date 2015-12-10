/*Receive MIDI and check if note = 60
By Amanda Ghassaei
July 2012
http://www.instructables.com/id/Send-and-Receive-MIDI-with-Arduino/

 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.

*/

byte commandByte;
byte noteByte;
byte velocityByte;

byte noteOn = 144;

//light up led at pin 13 when receiving noteON message with note = 60

void setup(){
  Serial.begin(31250);
  pinMode(13,OUTPUT);
  digitalWrite(13,LOW);
}

void checkMIDI(){
  do{
    if (Serial.available()){
      commandByte = Serial.read();//read first byte
      noteByte = Serial.read();//read next byte
      velocityByte = Serial.read();//read final byte
      if (commandByte == noteOn){//if note on message
        //check if note == 60 and velocity > 0
        if (noteByte == 60 && velocityByte > 0){
          digitalWrite(13,HIGH);//turn on led
          Serial.end();
          delay(20);
          Serial.begin(38400);
          delay(20);
          Serial.print(commandByte);
          Serial.print(noteByte);
          Serial.print(velocityByte);
          delay(20);
          Serial.end();
          delay(20);
          Serial.begin(31250);
          delay(20);
        }
      }
    }
  }
  while (Serial.available() > 2);//when at least three bytes available
}
    

void loop(){
  checkMIDI();
  delay(100);
  digitalWrite(13,LOW);//turn led off
}
