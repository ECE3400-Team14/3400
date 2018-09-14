#include <Servo.h>
Servo left;
Servo right;
int buttonPin = 8;
void setup() {
  left.attach(5);
  right.attach(3);
  stopMovement();
  Serial.begin(9600);
  pinMode(buttonPin, INPUT);
  while (digitalRead(buttonPin) == LOW);
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
  turnLeft();
  while (readLeftSensor() == 0 && readRightSensor() == 0);
  while (readLeftSensor() == 1 || readRightSensor() == 1);
  stopMovement();
}
void forwardAndRight(){
  while (readLeftmostSensor() == 1 || readRightmostSensor() == 1){
    trackLine();
  }
  turnRight();
  while (readLeftSensor() == 0 && readRightSensor() == 0);
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
      writeRight(180);
      writeLeft(90);
    }
    else if (readLeftSensor() == 1 && readRightSensor() == 0){
      writeLeft(180);
      writeRight(90);
    }else
      backward();
  }
/* returns 0 if white detected, 1 if black */
int readLeftSensor(){
  int val = analogRead(0);
  Serial.print(val);
  Serial.print(" ");
  return val > 800? 1:0;
  }
int readRightSensor(){
  int val = analogRead(1);
  Serial.print(val);
  Serial.print(" ");
  return val > 800? 1:0;
  }
int readLeftmostSensor(){
  int val = analogRead(2);
  Serial.print(val);
  Serial.print(" ");
  return val > 600? 1:0;
  }
int readRightmostSensor(){
  int val = analogRead(3);
  Serial.print(val);
  Serial.print("\n");
  return val > 600? 1:0;
  }
/* writing 180 to both functions would go forward, 0 would go backward*/
void writeLeft(int speed){
    left.write(speed);
  }
void writeRight(int speed){
    right.write(180-speed);
  }

void backward(){
  writeLeft(0);
  writeRight(0);
  }
void forward(){
  writeLeft(180);
  writeRight(180);
  }
void turnLeft(){
  writeLeft(0);
  writeRight(180);
  }
void turnRight(){
  writeLeft(180);
  writeRight(0);
  }
void stopMovement(){
  writeLeft(90);
  writeRight(90);
  }
