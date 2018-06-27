void setup()   {                
  pinMode(13, OUTPUT);      

}
 
void loop()                     
{

}
 
// This procedure sends a 38KHz pulse to the IRledPin 
// for a certain # of microseconds. We'll use this whenever we need to send codes
void pulseIR(long microsecs) {
  // we'll count down from the number of microseconds we are told to wait
 
  cli();  // this turns off any background interrupts
 
  while (microsecs > 0) {
    // 38 kHz is about 13 microseconds high and 13 microseconds low
   digitalWrite(IRledPin, HIGH);  // this takes about 3 microseconds to happen
   delayMicroseconds(10);         // hang out for 10 microseconds
   digitalWrite(IRledPin, LOW);   // this also takes about 3 microseconds
   delayMicroseconds(10);         // hang out for 10 microseconds
 
   // so 26 microseconds altogether
   microsecs -= 26;
  }
 
  sei();  // this turns them back on
}
 
void SendPowerButton() {  
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
