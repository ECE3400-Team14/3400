#include <Servo.h>
Servo left;
Servo right;
int buttonPin = 7;
int rightWallSensor = A5;
int rightWallLED = 6;
int frontWallSensor = A4;
int frontWallLED = 10;
int mux0=8;
int mux1=9;
int muxRead = A3;
int muxReadDelay = 6; //ms delay before reading from the mux to handle some switching issues

bool fft_detect = false;
void setup() {
  left.attach(5);
  right.attach(3);
  stopMovement();
  Serial.begin(9600);
  pinMode(buttonPin, INPUT);
   pinMode(rightWallSensor, INPUT);
  pinMode(frontWallSensor, INPUT);
  pinMode(rightWallLED, OUTPUT);
  pinMode(frontWallLED, OUTPUT);
  pinMode(mux0, OUTPUT);
  pinMode(mux1, OUTPUT);
  Serial.println("Wait for button");
  while(digitalRead(buttonPin)==LOW);
  //Serial.println("Button Pressed");
  fft_setup();//ADDED
  Serial.println("FFT Setup Complete");
  
}

void loop() {
//  forwardAndLeft();
//  
//  forwardAndRight();
//
//  forwardAndRight();
// 
//  forwardAndRight();
// 
//  forwardAndRight();
//  
//  forwardAndLeft();
// 
//  forwardAndLeft();
//  
//  forwardAndLeft();
// 
Serial.println("Loop");
 
   int hasRightWall = readRightWallSensor();
   int hasFrontWall = readForwardWallSensor();
    /*
   while(true) {
    fft_analyze();
    Serial.println(fft_detect);
    
   }*/
   if (hasRightWall==1&&hasFrontWall==0) {
    forwardAndStop();
   }
   else if (hasRightWall==1&&hasFrontWall==1){
    turnLeft();
    finishTurn();
   }
   else if (hasRightWall==0){
    turnRight();
    finishTurn();
    forwardAndStop();
   }
// troubleshooting code block:
//   int leftmost = readLeftmostSensor();
//   int left = readLeftSensor();
//   int right = readRightSensor();
//   int rightmost = readRightmostSensor();
//   Serial.print("|");
//   int hasRightWall = readRightWallSensor(); 
//   int hasFrontWall = readForwardWallSensor();
//   Serial.print(hasRightWall);
//   Serial.print(" ");
//   Serial.print(hasFrontWall);
//   Serial.println();
}

void finishTurn(){
  while (readLeftSensor() == 0 || readRightSensor() == 0);//robot exits line (breaks loop when both sensors are dark)
  while (readLeftSensor() == 1 && readRightSensor() == 1);//robot enters line again (both line sensors light)
  stopMovement();
}
void forwardUntilOffIntersection() {
  while (readLeftmostSensor() == 0 && readRightmostSensor() == 0){
    trackLine();
  }
}

void forwardAndStop(){
    int i = 0;
    while (readLeftmostSensor() == 1 || readRightmostSensor() == 1){
      //Serial.println("BEGIN");
      trackLine();
      //perform FFT every __ number of cycles
      if (i == 5) {
        Serial.println("Running fft");
        /*
        stopMovement();
        while(true) {
          fft_analyze();
          Serial.println(fft_detect);
        }*/
        fft_analyze();
        while (fft_detect/**/) {
          stopMovement();
          fft_analyze();
        } //Added
        i = 0;
      }
      else { i++; }
    }
  stopMovement();
}

void forwardAndLeft(){
  while (readLeftmostSensor() == 1 || readRightmostSensor() == 1){ //This statement detects an intersection (both outer sensors go light to break loop)
    trackLine();
  }
  turnLeft();
  while (readLeftSensor() == 0 && readRightSensor() == 0);//robot exits line (both line sensors dark)
  while (readLeftSensor() == 1 || readRightSensor() == 1);//robot enters line again (both line sensors light)
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

/* read forward-facing sensor
 * Returns 1 for wall detected and 0 for no wall detected
 */
int readForwardWallSensor() {
  int val = analogRead(frontWallSensor);
  //Serial.print(val);
  //Serial.print(" ");
  if (val>180){
    digitalWrite(frontWallLED, HIGH);
    return 1;
  }
  else{
    digitalWrite(frontWallLED, LOW);
    return 0;
  }
}

/* read right-facing sensor. 
 *  Returns 1 for wall detected and 0 for no wall detected
 */
int readRightWallSensor() {
  
  int val = analogRead(rightWallSensor);
  //Serial.print(val);
  //Serial.print(" ");
  if (val>180){
    digitalWrite(rightWallLED, HIGH);
    return 1;
  }
  else{
    digitalWrite(rightWallLED, LOW);
    return 0;
  }
}
  
/* returns 0 if white detected, 1 if black */
int readLeftSensor(){
  digitalWrite(mux0, LOW);
  digitalWrite(mux1, LOW);
  delay(muxReadDelay);
  int val = analogRead(muxRead);
  //Serial.print(val);
  //Serial.print(" ");
  return val > 357? 1:0;
  }
int readRightSensor(){
  digitalWrite(mux0, HIGH);
  digitalWrite(mux1, LOW);
  delay(muxReadDelay);
  int val = analogRead(muxRead);
  //Serial.print(val);
  //Serial.print(" ");
  return val > 297? 1:0;
  }
int readLeftmostSensor(){
  digitalWrite(mux0, HIGH);
  digitalWrite(mux1, HIGH);
  delay(muxReadDelay);
  int val = analogRead(muxRead);
  //Serial.print(val);
  //Serial.print(" ");
  return val > 700? 1:0;
  }
int readRightmostSensor(){
  digitalWrite(mux0, LOW);
  digitalWrite(mux1, HIGH);
  delay(muxReadDelay);
  int val = analogRead(muxRead);
  //Serial.print(val);
//  Serial.print("\n");
  return val > 570? 1:0;
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

