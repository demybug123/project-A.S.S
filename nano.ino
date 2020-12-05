void setup() {
  Serial.begin(9600);
  pinMode(A0,INPUT);
  pinMode(9,INPUT);
  pinMode(5,OUTPUT);
  pinMode(2,OUTPUT);
}

void loop() {
  Serial.println(digitalRead(9));
  if(digitalRead(9)!=1){
  digitalWrite(5,HIGH);
  digitalWrite(2,HIGH);
  delay(100);
  }
 else {digitalWrite(5,LOW);
 digitalWrite(2,LOW);
 }
}
