#include <AFMotor.h>
#include <NewPing.h>
#include <Servo.h>

//   4 chân của cảm biến siêu âm: GND,ECHO,TRIG,VCC
// Chúng ta: 1 đầu bẹ 4 dây (cắm vào cảm biến siêu âm); 1 đầu tuốt dây (cắm vào Lỗ trên Arduino, Sau đó cắm chồng board L293D lên)
// SIÊU ÂM hc-04 --------Arduino
//       GND         -    GND           
//       ECHO        -    A1
//       TRIG        -    A2
//       VCC         -    5V

/************************************************/
AF_DCMotor motor1(2);        //motor - phai- noi pin 2 của L293D
AF_DCMotor motor2(3);        //motor - trai- noi pin 3 của L293D

Servo myservo;              //Servo SG90 cắm vao giắc 3 chân: SERVO2 (TRÊN L293D)

/***********************IRcode Definition  *************************/
#include "UCNEC.h"
int32_t temp = 0;
UCNEC myIR(10);             //Cắm vào SERVO1 trên L293D

#define F 16718055
#define B 16732845
#define L 16713975
#define R 16734885
#define S 16719075
#define track       16717545           //line-tracking routing press [*]
#define ultra       16718310           //Ultrasonics press [0]
#define ble         16715250           //Bluetooth run press [#]

/*************ĐỊNH NGHĨA CHÂN CHO CẢM BIẾN SIÊU ÂM****************/
#define ECHO_PIN A0          //noi chan A1 (Uno)
#define TRIG_PIN A1          //Noi chan A2 (Uno)

#define MAX_DISTANCE 200 
#define MAX_SPEED 235        // cài đặt tốc độ động cơ DC motors
#define MAX_SPEED_OFFSET 20  

NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE); 
boolean goesForward=false;
int distance = 100;
int speedSet = 0;
char command; 

int lechA = +50;          //bù lệch tốc độ bánh trái
int lechB = 0;            //bù lệch tốc độ bánh phải

/*************************Define Line Track pins************************************/
const int SensorLeft   = A5;       //Left sensor input (A5)
const int SensorMiddle = A4;       //Midd sensor input (A4)
const int SensorRight  = A3;       //Right sensor input(A3)
int SL;    //Status of Left line track sensor
int SM;    //Status of Midd line track sensor
int SR;    //Status of Righ line track sensor

unsigned char Dift=0; //center: Dift=0; lech trai: Dift = 1; lech phai:Dift = 2;

void setup() {  
   
    myservo.attach(9);         //tương ứng SERVO2; NẾU CẮM SERVO1 - KHAI  myservo.attach(10);
    myservo.write(95);         //Chỉnh lại góc quay SG90: 85,95,105,... sao cho cảm biến siêu âm thẳng với xe
    delay(1000);
    distance = readPing();
    delay(100);
    distance = readPing();
    delay(100);
    distance = readPing();
    delay(100);
    distance = readPing();
    delay(100);
    
    myIR.begin();
    
    pinMode(SensorLeft,  INPUT);    //Init left sensor
    pinMode(SensorMiddle, INPUT);   //Init Middle sensor
    pinMode(SensorRight, INPUT);    //Init Right sensor

}
void loop(){
      if(myIR.available())
      {
            temp =  myIR.read();
      }
             
      if (temp == F)      {go(160,160);} 
      else if (temp == B) {back(160,160);} 
      else if (temp == L) {left(200,200);} 
      else if (temp == R) {right(200,200);} 
      else if (temp == S) {Stop();temp=0;}
      else if (temp == track) 
      {
              Dift=0;
              while(temp == track)
              {
                      int distance = sonar.ping_cm();

                if (distance == 0) {
                        distance = 30;
                }
                
                if(distance <=20) {
                            Stop();
                            delay(100);
                        right(150,150);
                        delay(150);
                            Stop();
                            delay(100);
                        go(200,200);
                        delay(800);
                            Stop();
                            delay(300);
                        left(200,200);
                        delay(210);
                            Stop();
                            delay(300);
                        go(180,200);
                        delay(700);
                            Stop();
                            delay(100);
                        do{
                            left(40,160);
                            SL = digitalRead(SensorLeft);
                            SM = digitalRead(SensorMiddle);
                            SR = digitalRead(SensorRight);
                        }while((SL==LOW)&&(SM==LOW)&&(SR==LOW));
                            
                            Stop();
                            delay(100);
                        do{
                            right(160,40);
                            SL = digitalRead(SensorLeft);
                            SM = digitalRead(SensorMiddle);
                            SR = digitalRead(SensorRight);
                        }while((SL==LOW)&&(SM==LOW)&&(SR==LOW));
                        
                            //delay(500);
                            //go(160);
                            //delay(700);
                            //right(250,250);
                            //delay(500);
                }
          
                /****************************************/
                //temp =  myIR.read();
                
                SL = digitalRead(SensorLeft);
                SM = digitalRead(SensorMiddle);
                SR = digitalRead(SensorRight);
                
                //right fast 20 200
                if (SM == HIGH && SR == HIGH && SL == LOW)
                {
                      go(160,40);delay(100);
                }
                if (SM == HIGH && SR == LOW && SL == HIGH)
                {
                      go(40,160);delay(100);
              
                }
                if (SM == HIGH && SR == LOW && SL == LOW)
                {
                      go(160,160);
                }
                if(SM == LOW && SR == LOW && SL == HIGH)
                {
                      go(80,200);delay(100);
                }
                if(SM == LOW && SR == HIGH && SL == LOW)
                {
                      go(200,80);delay(100);
                }
                else
                {
                      Stop(); delay(100);
                }
        
             }
        }
        else 
        {
            Stop();
            temp=0;
        }
      
}
void go(int a, int b)
{
  motor1.setSpeed(b+lechB); //Define maximum velocity
  motor1.run(FORWARD); //rotate the motor clockwise
  motor2.setSpeed(a+lechA); //Define maximum velocity
  motor2.run(FORWARD); //rotate the motor clockwise
}
void back(int a, int b)
{
  motor1.setSpeed(b+lechB); 
  motor1.run(BACKWARD); 
  motor2.setSpeed(a+lechA); 
  motor2.run(BACKWARD); 
}
void left(int a, int b)
{
  motor1.setSpeed(b+lechB); 
  motor1.run(FORWARD); 
  motor2.setSpeed(a+lechA);
  motor2.run(BACKWARD); 
}
void right(int a, int b)
{
  motor1.setSpeed(b+lechB);
  motor1.run(BACKWARD); 
  motor2.setSpeed(a+lechA);  
  motor2.run(FORWARD); 
}

void Stop()
{
  motor1.setSpeed(0);
  motor2.run(RELEASE); 
  motor2.setSpeed(0);
  motor2.run(RELEASE); 
}
void stopAvoiding()
{
  //dugme=0;
  motor1.setSpeed(0);
  motor2.run(RELEASE); 
  motor2.setSpeed(0);
  motor2.run(RELEASE); 
}

int lookRight()
{
    myservo.write(50); 
    delay(500);
    int distance = readPing();
    delay(100);
    myservo.write(105); 
    return distance;
}
int lookLeft()
{
    myservo.write(160); 
    delay(500);
    int distance = readPing();
    delay(100);
    myservo.write(105); 
    return distance;
    delay(100);
}
int readPing() { 
  delay(70);
  int cm = sonar.ping_cm();
  if(cm==0)
  {
    cm = 250;
  }
  return cm;
}
void moveStop() {
  motor1.run(RELEASE); 
  motor2.run(RELEASE);
  } 
void moveForward() {
 if(!goesForward)
  {
    goesForward=true;
    motor1.run(FORWARD);      
    motor2.run(FORWARD);   
   for (speedSet = 0; speedSet < MAX_SPEED; speedSet +=2) // slowly bring the speed up to avoid loading down the batteries too quickly
   {
    motor1.setSpeed(speedSet);
    motor2.setSpeed(speedSet+MAX_SPEED_OFFSET);
    delay(5);
   }
  }
}
void moveBackward() {
    goesForward=false;
    motor1.run(BACKWARD);      
    motor2.run(BACKWARD);  
  for (speedSet = 0; speedSet < MAX_SPEED; speedSet +=2) // slowly bring the speed up to avoid loading down the batteries too quickly
  {
    motor1.setSpeed(speedSet);
    motor2.setSpeed(speedSet+MAX_SPEED_OFFSET);
    delay(5);
  }
}  
void turnRight() {
  motor2.run(FORWARD);
  motor1.run(BACKWARD);     
  delay(300);
  motor2.run(FORWARD);      
  motor1.run(FORWARD);      
} 
void turnLeft() {
  motor2.run(BACKWARD);     
  motor1.run(FORWARD);     
  delay(300);
  motor2.run(FORWARD);     
  motor1.run(FORWARD);
}    
