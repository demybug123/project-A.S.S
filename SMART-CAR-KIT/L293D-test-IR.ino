#include <AFMotor.h>
#include <NewPing.h>
#include <Servo.h>

/************************************************/
#include "UCNEC.h"
int32_t temp = 0;
UCNEC myIR(10);          //SERVO1 IN L293D

#define F 16718055
#define B 16732845
#define L 16713975
#define R 16734885
#define S 16719075

char command; 


AF_DCMotor motor1(2);        //motor - phai- noi pin 2 của L293D
AF_DCMotor motor2(3);        //motor - trai- noi pin 3 của L293D


void setup() 
{   

  Serial.begin(9600);  //Set the baud rate to your Bluetooth module.

  myIR.begin();
}
void loop(){
  if(myIR.available())
      {
        temp =  myIR.read();
      }
             
      if (temp == F){forward();} 
      else if (temp == B) {back();}
      else if (temp == L) {left();} 
      else if (temp == R) {right();} 
      else if (temp == S) {Stop();temp=0;}
      
}
void forward()
{
  motor1.setSpeed(255); //Define maximum velocity
  motor1.run(FORWARD); //rotate the motor clockwise
  motor2.setSpeed(255); //Define maximum velocity
  motor2.run(FORWARD); //rotate the motor clockwise
}
void back()
{
  motor1.setSpeed(255); 
  motor1.run(BACKWARD); 
  motor2.setSpeed(255); 
  motor2.run(BACKWARD); 
}
void left()
{
  motor1.setSpeed(255); 
  motor1.run(FORWARD); 
  motor2.setSpeed(0);
  motor2.run(RELEASE); 
}
void right()
{
  motor1.setSpeed(0);
  motor1.run(RELEASE); 
  motor2.setSpeed(255);  
  motor2.run(FORWARD); 
}
void aheadLeft()
{
  motor1.setSpeed(255); 
  motor1.run(FORWARD); 
  motor2.setSpeed(125); 
  motor2.run(FORWARD); 
}
void aheadRight()
{
  motor1.setSpeed(125); 
  motor1.run(FORWARD); 
  motor2.setSpeed(255); 
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
