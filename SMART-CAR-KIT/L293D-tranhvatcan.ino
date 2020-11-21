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
//Servo SG90 cắm vao giắc 3 chân: SERVO1 (TRÊN L293D)


#define ECHO_PIN A0          //noi chan A0 (Uno)
#define TRIG_PIN A1          //Noi chan A1 (Uno)

#define MAX_DISTANCE 200 
#define MAX_SPEED 235        // cài đặt tốc độ động cơ DC motors
#define MAX_SPEED_OFFSET 20  

AF_DCMotor motor1(2);        //motor - phai- noi pin 2 của L293D
AF_DCMotor motor2(3);        //motor - trai- noi pin 3 của L293D

Servo myservo;

NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE); 
boolean goesForward=false;
int distance = 100;
int speedSet = 0;
char command; 


int dugme=1;
void setup() 
{   
   
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
    
    Serial.begin(9600);  //Set the baud rate to your Bluetooth module.
}
void loop(){
      long duration, distance;
           while(dugme==1)
           { 
               int distanceR = 0;
               int distanceL =  0;
               delay(40);
               if(distance<=24)
               {
                    moveStop();
                    delay(100);
                    moveBackward();
                    delay(300);
                    moveStop();
                    delay(200);
                    distanceR = lookRight();
                    delay(200);
                    distanceL = lookLeft();
                    delay(200);
                    if(distanceR>=distanceL)
                    {
                      turnRight();
                      moveStop();
                    }else
                    {
                      turnLeft();
                      moveStop();
                    }
               }else
               {
                forward();
               }
               distance = readPing();
           }
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
void stopAvoiding()
{
  dugme=0;
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
