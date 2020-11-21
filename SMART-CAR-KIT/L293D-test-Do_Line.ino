#include <AFMotor.h>

/************************************************/
AF_DCMotor motor1(2);        //motor - phai- noi pin 2 của L293D
AF_DCMotor motor2(3);        //motor - trai- noi pin 3 của L293D
/*************************Define Line Track pins************************************/
const int SensorLeft   = 10;      //Left sensor input (A1)
const int SensorMiddle = 9;       //Midd sensor input (A2)
const int SensorRight  = 8;       //Right sensor input(A3)
int SL;    //Status of Left line track sensor
int SM;    //Status of Midd line track sensor
int SR;    //Status of Righ line track sensor

unsigned char old_SL,old_SM,old_SR;     //trang thai truoc cua cam bien do line

void setup()
{

      pinMode(SensorLeft,  INPUT);    //Init left sensor
      pinMode(SensorMiddle, INPUT);   //Init Middle sensor
      pinMode(SensorRight, INPUT);    //Init Right sensor
}

void loop()
{
      SL = digitalRead(SensorLeft);
      SM = digitalRead(SensorMiddle);
      SR = digitalRead(SensorRight);
      
      //right fast 20 200
      if (SM == HIGH && SR == HIGH && SL == LOW)
      {
            goRight(160,40);delay(100);
      }
      if (SM == HIGH && SR == LOW && SL == HIGH)
      {
            goLeft(40,160);delay(100);
    
      }
      if (SM == HIGH && SR == LOW && SL == LOW)
      {
            forward(160,160);
      }
      if(SM == LOW && SR == LOW && SL == HIGH)
      {
            goLeft(200,80);delay(100);
      }
      if(SM == LOW && SR == HIGH && SL == LOW)
      {
            goRight(80,200);delay(100);
      }
      else
      {
            Stop(); delay(100);
      }

}
void forward(int a, int b)
{
  motor1.setSpeed(b); //Define maximum velocity
  motor1.run(FORWARD) ; //rotate the motor clockwise
  motor2.setSpeed(a); //Define maximum velocity
  motor2.run(FORWARD); //rotate the motor clockwise
}
void back(int a, int b)
{
  motor1.setSpeed(b); 
  motor1.run(BACKWARD); 
  motor2.setSpeed(a); 
  motor2.run(BACKWARD); 
}
void left(int a, int b)
{
  motor1.setSpeed(b); 
  motor1.run(FORWARD); 
  motor2.setSpeed(a);
  motor2.run(BACKWARD); 
}
void right(int a, int b)
{
  motor1.setSpeed(b);
  motor1.run(BACKWARD); 
  motor2.setSpeed(a);  
  motor2.run(FORWARD); 
}
void goLeft(int a, int b)
{
  motor1.setSpeed(b); 
  motor1.run(FORWARD); 
  motor2.setSpeed(a); 
  motor2.run(FORWARD); 
}
void goRight(int a, int b)
{
  motor1.setSpeed(b); 
  motor1.run(FORWARD); 
  motor2.setSpeed(a); 
  motor2.run(FORWARD); 
}
void backRight()
{
  motor1.setSpeed(125); 
  motor1.run(BACKWARD);
  motor2.setSpeed(255);
  motor2.run(BACKWARD); 
}
void backLeft()
{
  motor1.setSpeed(255);
  motor1.run(BACKWARD); 
  motor2.setSpeed(125); 
  motor2.run(BACKWARD); 
}
void Stop()
{
  motor1.setSpeed(0);
  motor2.run(RELEASE); 
  motor2.setSpeed(0);
  motor2.run(RELEASE); 
}
