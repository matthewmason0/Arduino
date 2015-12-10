int inputL=0;
int inputR=0;
int Lspeed;
int Rspeed;
int L=A0;
int R=A1;
int Lr=5;
int Lf=6;
int Rr=10;
int Rf=11;
bool Lreverse=false;
bool Rreverse=false;

void setup() {
  pinMode(L,INPUT);
  pinMode(R,INPUT);
  pinMode(Lr,OUTPUT);
  pinMode(Lf,OUTPUT);
  pinMode(Rr,OUTPUT);
  pinMode(Rf,OUTPUT);
  Serial.begin(9600);
}

//0--100  0 - 100
//0-155  156-335

void loop() {
  inputL=analogRead(L);
  inputR=analogRead(R);
  if(inputL!=0) {
    if(inputL>155) {
      Lspeed=map(inputL,156,335,0,255);
      Lreverse=false;
    }
    else {
      Lspeed=map(inputL,0,155,0,255);
      Lreverse=true;
    }
  }
  else Lspeed=0;
  if(inputR!=0) {
    if(inputR>155) {
      Rspeed=map(inputR,156,335,0,255);
      Rreverse=false;
    }
    else {
      Rspeed=map(inputR,0,155,0,255);
      Rreverse=true;
    }
  }
  else Rspeed=0;
  
  if(Lreverse) {
    Serial.print(-Lspeed);
    analogWrite(Lr,Lspeed);
    analogWrite(Lf,0);
  }
  else {
    Serial.print(Lspeed);
    analogWrite(Lr,0);
    analogWrite(Lf,Lspeed);
  }
  
  Serial.print(", ");
  
  if(Rreverse) {
    Serial.print(-Rspeed);
    analogWrite(Rr,Rspeed);
    analogWrite(Rf,0);
  }
  else {
    Serial.print(Rspeed);
    analogWrite(Rr,0);
    analogWrite(Rf,Rspeed);
  }
  
  Serial.print("\n");
  
  
  delay(1);
    
}
