void setup(){
  pinMode(2,OUTPUT);
  pinMode(3,OUTPUT);
  pinMode(13,OUTPUT);
  digitalWrite(13,LOW);
  Serial.begin(9600);
}

void loop(){
  while(Serial.available()){
    char c=Serial.read();
    if(c == '1'){
      digitalWrite(2,HIGH);
      delay(1000);
      digitalWrite(2,LOW);
    }
    else if(c == '0'){
      digitalWrite(3,HIGH);
      delay(1000);
      digitalWrite(3,LOW);
    }
  }
}
