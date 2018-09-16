#include <Servo.h>
Servo left;
Servo right;
int buttonPin = 8;

/*Buffer variables*/
int buffer_size = 4;
int* leftmostSensorBuffer = new int [buffer_size];
int* leftSensorBuffer = new int [buffer_size];
int* rightSensorBuffer = new int [buffer_size];
int* rightmostSensorBuffer = new int [buffer_size];
int lmsbPointer = 0;
int lsbPointer = 0;
int rsbPointer = 0;
int rmsbPointer = 0;

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
  //setBufferDefault(leftmostSensorBuffer, 1); setBufferDefault(rightmostSensorBuffer, 1);
  while (readLeftmostSensor() == 1 || readRightmostSensor() == 1){
    trackLine();
  }
  turnLeft();
  //setBufferDefault(leftSensorBuffer, 0); setBufferDefault(rightSensorBuffer, 0);
  while (readLeftSensor() == 0 && readRightSensor() == 0);
  while (readLeftSensor() == 1 || readRightSensor() == 1);
  stopMovement();
}
void forwardAndRight(){
  //setBufferDefault(leftmostSensorBuffer, 1); setBufferDefault(rightmostSensorBuffer, 1);
  while (readLeftmostSensor() == 1 || readRightmostSensor() == 1){
    trackLine();
  }
  turnRight();
  //setBufferDefault(leftSensorBuffer, 0); setBufferDefault(rightSensorBuffer, 0);
  while (readLeftSensor() == 0 && readRightSensor() == 0);
  while (readLeftSensor() == 1 || readRightSensor() == 1);
  stopMovement();
}
void forwardAndStop(){
  //setBufferDefault(leftmostSensorBuffer, 1); setBufferDefault(rightmostSensorBuffer, 1);
  while (readLeftmostSensor() == 1 || readRightmostSensor() == 1){
    trackLine();
  }
  stopMovement();
}
void trackLine(){
    //setBufferDefault(leftSensorBuffer, 0); setBufferDefault(rightSensorBuffer, 0);
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
  //update_buffer(leftSensorBuffer, lsbPointer, val);
  //val = average_buffer(leftSensorBuffer);
  Serial.print(val);
  Serial.print(" ");
  return val > 800? 1:0;
  }
int readRightSensor(){
  int val = analogRead(1);
  //update_buffer(rightSensorBuffer, rsbPointer, val);
  //val = average_buffer(leftSensorBuffer);
  Serial.print(val);
  Serial.print(" ");
  return val > 800? 1:0;
  }
int readLeftmostSensor(){
  int val = analogRead(2);
  //update_buffer(leftmostSensorBuffer, lmsbPointer, val);
  //val = average_buffer(leftSensorBuffer);
  Serial.print(val);
  Serial.print(" ");
  return val > 600? 1:0;
  }
int readRightmostSensor(){
  int val = analogRead(3);
  //update_buffer(rightmostSensorBuffer, rmsbPointer, val);
  //val = average_buffer(leftSensorBuffer);
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

/* filter functions */
void update_buffer(int buffer[], int &p, int val) {
  buffer[p] = val;
  p = (p = buffer_size-1) ? 0 : (p+1);
}

void setBufferDefault(int buffer[], int val) {
  for(int i = 0; i < buffer_size; i++) {
      buffer[i] = val*1023;
  }
}

int average_buffer(int buffer[]) {
  int sum = 0;
  for(int i = 0; i < buffer_size; i++) {
      sum +=buffer[i];
  }
  return sum/buffer_size;
}






