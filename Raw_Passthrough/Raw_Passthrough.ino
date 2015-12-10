int inputVal=0;

void setup()
{
  pinMode(A0,INPUT);
  pinMode(3,OUTPUT);
}

void loop()
{
  inputVal=analogRead(A0);
  analogWrite(3,inputVal/4);
}
