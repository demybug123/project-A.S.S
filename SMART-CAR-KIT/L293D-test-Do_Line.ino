#include <AFMotor.h>

/************************************************/
AF_DCMotor motor1(2);        //motor - phai- noi pin 2 của L293D
AF_DCMotor motor2(3);        //motor - trai- noi pin 3 của L293D
/*************************Define Line Track pins************************************/
const int SensorLeft   = A5;      //Left sensor input (A1)
const int SensorMiddle = A4;       //Midd sensor input (A2)
const int SensorRight  = A3;       //Right sensor input(A3)
int SL;    //Status of Left line track sensor
int SM;    //Status of Midd line track sensor
int SR;    //Status of Righ line track sensor

unsigned char old_SL,old_SM,old_SR;     //trang thai truoc cua cam bien do line
int dolech1=22;
int dolech2=23;
int MAXSPEEDOFFSET=42;
bool doStartup=0;
void setup()
{
      Serial.begin(9600);
      pinMode(SensorLeft,  INPUT);    //Init left sensor
      pinMode(SensorMiddle, INPUT);   //Init Middle sensor
      pinMode(SensorRight, INPUT);    //Init Right sensor
      delay(2500);
      startUp();
      delay(10);
}

void loop()
{
      SL = digitalRead(SensorLeft);
      SM = digitalRead(SensorMiddle);
      SR = digitalRead(SensorRight);
      Serial.print("SL=");Serial.println(SL);Serial.print("SM=");Serial.println(SM);Serial.print("SR=");Serial.println(SR);delay(20);
      //right fast 20 200
      if (SM == HIGH && SR == HIGH && SL == LOW)
      {
            go(40,60);delay(100);
      }
      if (SM == HIGH && SR == LOW && SL == HIGH)
      {
            go(60,40);delay(100);
      }
      
      if(SM == LOW && SR == LOW && SL == HIGH)
      {
            go(70,40);delay(200);
      }
      if (SM == LOW && SR == HIGH && SL == HIGH)
      {
            go(60,60);delay(150);
      }
      if(SM == LOW && SR == HIGH && SL == LOW)
      {
            go(40,60);delay(100);
      }
      else
      {
            Stop(); delay(100);
      }

}

void go(int a, int b)
{
  motor1.setSpeed(b); //Define maximum velocity
  motor1.run(FORWARD); //rotate the motor clockwise
  motor2.setSpeed(a+dolech1); //Define maximum velocity
  motor2.run(FORWARD); //rotate the motor clockwise
  delay(100);
  motor1.setSpeed(b); //Define maximum velocity
  motor2.setSpeed(a+dolech2); //Define maximum velocity
  delay(100);
}
void Stop()
{
  motor1.setSpeed(0);
  motor2.run(RELEASE); 
  motor2.setSpeed(0);
  motor2.run(RELEASE); 
}

void startUp(){
  int SpeedSet=0;
  motor1.run(FORWARD);      
  motor2.run(FORWARD);
  for (SpeedSet = 30; SpeedSet < 80; SpeedSet +=2) // slowly bring the speed up to avoid loading down the batteries too quickly
   {
    motor1.setSpeed(SpeedSet);
    
    motor2.setSpeed(SpeedSet+MAXSPEEDOFFSET+2);
    delay(70);
   }
  }
