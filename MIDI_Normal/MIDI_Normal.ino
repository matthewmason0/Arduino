/*Receive Midi
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

void setup(){
  Serial.begin(9600);
}

void checkMIDI(){
  do{
    if (Serial.available()){
      commandByte = Serial.read();//read first byte
      noteByte = Serial.read();//read next byte
      velocityByte = Serial.read();//read final byte
    }
  }
  while (Serial.available() > 2);//when at least three bytes available
}

void loop(){
  checkMIDI();
  if(commandByte==10010000) {
  MIDImessage(commandByte, noteByte, velocityByte);
  }
  if(commandByte==10000000) {
  MIDImessage(commandByte, noteByte, velocityByte);
  }
  //delay(100);
}

void MIDImessage(int command, int MIDInote, int MIDIvelocity) {
  Serial.write(command);//send note on or note off command 
  Serial.write(MIDInote);//send pitch data
  Serial.write(MIDIvelocity);//send velocity data
}
