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
long previousMillis = 0;
int light = 0;

//light up led at pin 13 when receiving noteON message with note = 60

void setup(){
  Serial.begin(31250);
}

void checkMIDI(){
  do{
    if (Serial.available()){
      commandByte = Serial.read();//read first byte
      noteByte = Serial.read();//read next byte
      velocityByte = Serial.read();//read final byte
      if (commandByte == noteOn){//if note on message
        //check if note == 60 and velocity > 0
        if (noteByte > 60 && velocityByte > 0){
          Serial.end();
          Serial.begin(9600);
          Serial.write(commandByte);
          Serial.write(noteByte);
          Serial.write(velocityByte);
          Serial.end();
          Serial.begin(31250);
        }
      }
    }
  }
  while (Serial.available() > 2);//when at least three bytes available
}
    

void loop(){
  checkMIDI();
}
