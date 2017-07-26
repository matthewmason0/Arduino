#include <TimerOne.h>

volatile boolean bitStarted = false;
volatile boolean packetStarted = false;
volatile boolean bitAvailable = false;
volatile boolean data = 0;
volatile boolean fall = false;
volatile boolean rise = false;

void setup() {
  Serial.begin(115200);
  pinMode(2, INPUT);
  Timer1.initialize(2500);
  Timer1.stop();
  Timer1.attachInterrupt(timeout);
  attachInterrupt(0, falling, CHANGE);
  //attachInterrupt(0, rising, RISING);
}

void falling() {
  if(digitalRead(2)) {
    rise = true;
//    if(bitStarted) {
//      unsigned long time = Timer1.read();
//      if(time<1000)
//        data = 0;
//      else
//        data = 1;
//      bitStarted = false;
//      bitAvailable = true;
//    }
  }  
  else {
    fall = true;
//    bitStarted = true;
//    packetStarted = true;
//    Timer1.start();
  }
}

void rising() {

}

void timeout() { //triggers when there is no rising edge after
                 //a falling edge (bit timed out), or no falling
                 //edge after a rising edge (end of packet)
  bitStarted = false;
  packetStarted = false;
}
  
void loop() {
//  int bits = 0;
//  boolean buffer[64];
//  while(!packetStarted);
//  while(packetStarted) {
//    if(bitAvailable) {
//      buffer[bits] = data;
//      bits++;
//      bitAvailable = false;
//    }
//  }
//  int bytes = bits%8==0 ? bits/8 : bits/8+1;
//  byte packet[bytes];
//  for(int i=0;i<bytes;i++)
//    packet[i] = 0;
//  for(int i=0;i<bits;i++) {
//    int j = i/8;
//    bitWrite(packet[j], (j+1)*8-i-1, buffer[i]);
//  }
//  for(int i=0;i<bytes;i++)
//    Serial.print(packet[i], HEX);
//  Serial.println();   
  if(fall) {
    fall = false;
    Serial.print("0 ");
    Serial.println(micros());
  }
  if(rise) {
    rise = false;
    Serial.print("1 ");
    Serial.println(micros());
  }
}
