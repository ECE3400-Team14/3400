#include <Servo.h>
Servo left;
Servo right;
void setup() {
  left.attach(3);
  right.attach(5);
  left.write(90);
  right.write(90);
  Serial.begin(9600);
}

void loop() {
  forwardAndLeft();
  forwardAndRight();
  forwardAndRight();
  forwardAndRight();
  forwardAndRight();
  forwardAndLeft();
  forwardAndLeft();
  forwardAndLeft();
}
void forwardAndLeft(){
  while (readLeftmostSensor() == 1 || readRightmostSensor() == 1){
    trackLine();
  }
  left.write(0);
  right.write(0);
  while (readLeftSensor() == 0 && readRightSensor() == 0);
  //while (readRightmostSensor() == 0 || readLeftmostSensor() == 0 || readRightSensor() == 0 || readLeftSensor() == 0);
  while (readLeftSensor() == 1 || readRightSensor() == 1);
  stopMovement();
}
void forwardAndRight(){
  while (readLeftmostSensor() == 1 || readRightmostSensor() == 1){
    trackLine();
  }
  left.write(180);
  right.write(180);
  while (readLeftSensor() == 0 && readRightSensor() == 0);
  //while (readRightmostSensor() == 0 || readLeftmostSensor() == 0 || readRightSensor() == 0 || readLeftSensor() == 0);
  while (readLeftSensor() == 1 || readRightSensor() == 1);
  stopMovement();
}
void forwardAndStop(){
  while (readLeftmostSensor() == 1 || readRightmostSensor() == 1){
    trackLine();
  }
  stopMovement();
}
void trackLine(){
    if (readLeftSensor() == 0 && readRightSensor() == 0)
      forward();
    else if (readLeftSensor() == 0 && readRightSensor() == 1){
      left.write(0);
      right.write(90);
    }
    else if (readLeftSensor() == 1 && readRightSensor() == 0){
      left.write(90);
      right.write(180);
    }else
      backward();
  }
/* returns 0 if white detected, 1 if black */
int readLeftSensor(){
  int val = analogRead(0);
  Serial.print(val);
  Serial.print(" ");
  return val > 650? 1:0;
  }
int readRightSensor(){
  int val = analogRead(1);
  Serial.print(val);
  Serial.print(" ");
  return val > 650? 1:0;
  }
int readLeftmostSensor(){
  int val = analogRead(2);
  Serial.print(val);
  Serial.print(" ");
  return val > 650? 1:0;
  }
int readRightmostSensor(){
  int val = analogRead(3);
  Serial.print(val);
  Serial.print("\n");
  return val > 650? 1:0;
  }
void backward(){
  left.write(180);
  right.write(0);
  }
void forward(){
  left.write(0);
  right.write(180);
  }
void turnLeft(){
  left.write(0);
  right.write(0);
  }
void turnRight(){
  left.write(180);
  right.write(180);
  }
void stopMovement(){
  left.write(90);
  right.write(90);
  }
