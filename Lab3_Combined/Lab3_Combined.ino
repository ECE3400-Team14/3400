#include <Servo.h>
#include "printf.h"
Servo left;
Servo right;
int buttonPin = 0;
int rightWallSensor = A5;
int rightWallLED = 7;//TEMP
int frontWallSensor = A4;
int frontWallLED = 8;
int mux0=2;
int mux1=4;
int muxRead = A3;
int muxReadDelay = 6; //ms delay before reading from the mux to handle some switching issues
int fft_cycle = 10;
int fft_mux_pin = 6;

bool fft_detect = false;
bool has_started = false;

int orientation = 0; //0=north, 1=east, 2=south, 3=west
int x = 1;
int y = 2;

bool debug = false;

void setup() {
  left.attach(5);//left servo pin 5
  right.attach(3);//right servo pin 3
  stopMovement();
  Serial.begin(9600);
  pinMode(buttonPin, INPUT);
  pinMode(rightWallSensor, INPUT);
  pinMode(frontWallSensor, INPUT);
  pinMode(rightWallLED, OUTPUT);
  pinMode(frontWallLED, OUTPUT);
  pinMode(mux0, OUTPUT);
  pinMode(mux1, OUTPUT);
  pinMode(fft_mux_pin, OUTPUT);
  Serial.println("Wait for button");

  //uncomment for start button
  //while(digitalRead(buttonPin)==LOW);
  analogRead(1);//initialize analog
  
  //Serial.println("Button Pressed");
  fft_setup();//ADDED
  Serial.println("FFT Setup Complete");

   digitalWrite(fft_mux_pin, LOW);

  
  while(has_started == false) {
     fft_analyze();
     Serial.println("Waiting");
  }
  digitalWrite(fft_mux_pin, HIGH);
  
  initMaze();
  printf_begin();
  radioSetup();
}

void loop() {
  if (!debug) {
   int hasRightWall = readRightWallSensor();
   int hasFrontWall = readForwardWallSensor();
   updateMaze();
   sendMaze();
   if (hasRightWall==1&&hasFrontWall==0) {
    forwardAndStop();
    updateCoor();
   }
   else if (hasRightWall==1&&hasFrontWall==1){
    turnLeft();
    finishTurn();
    orientation = (orientation == 0) ? 3 : orientation - 1; 
    //
    //updateCoor();
   }
   else if (hasRightWall==0){
    turnRight();
    finishTurn();
    orientation = (orientation == 3) ? 0 : (orientation+1);
    forwardAndStop();
    updateCoor();
   }
  }
  //debug
  else {
   // troubleshooting code block:
   int leftmost = readLeftmostSensor();
   Serial.print("LL:");
   Serial.print(leftmost);
   Serial.print("|");
   int left = readLeftSensor ();
   Serial.print("L:");
   Serial.print(left);
   Serial.print("|");
   int right = readRightSensor();
   Serial.print("R:");
   Serial.print(right);
   Serial.print("|");
   int rightmost = readRightmostSensor();
   Serial.print("RR:");
   Serial.print(rightmost);
   Serial.print("|");
   int hasRightWall = readRightWallSensor(); 
   int hasFrontWall = readForwardWallSensor();
   Serial.print("RW:");
   Serial.print(hasRightWall);
   Serial.print(" ");
   Serial.print("FW:");
   Serial.print(hasFrontWall);
   //Serial.println();
   Serial.print(", ");
   fft_analyze();
   Serial.println();
   //orientation code:
//   updateCoor();
//   Serial.print("Orientation:");
//   Serial.println(orientation);
//   Serial.print(x);
//   Serial.print(", ");
//   Serial.print(y);
//   Serial.println();
//   orientation = (orientation == 0) ? 3 : orientation - 1; 
//   updateMaze();
//   sendMaze();
//   delay(1000);
  }
}
void updateCoor(){
  if (orientation == 0){
    x = (x == 0) ? 0 : x - 1;
  }else if (orientation == 1){
    y += 1;
  }else if (orientation == 2){
    x += 1;
  }else{
    y = (y == 0) ? 0 : y - 1;
    //x -= 1;
  }
}
//temporary solution since only 2 wall sensors
void updateMaze(){
//  turnLeft();
//  finishTurn();
//  orientation = (orientation-1)%4;
  int hasFrontWall = readForwardWallSensor();
  int hasRightWall = readRightWallSensor();
  if (orientation == 0){
    setNorthWall(x, y, hasFrontWall);
    setEastWall(x, y, hasRightWall);
    //setWestWall(x,y,0);
  }else if (orientation == 1){
    setEastWall(x, y, hasFrontWall);
    setSouthWall(x, y, hasRightWall);
    //setNorthWall(x,y,0);
  }else if (orientation == 2){
    setSouthWall(x, y, hasFrontWall);
    setWestWall(x, y, hasRightWall);
    //setEastWall(x,y,0);
  }else{
    setWestWall(x, y, hasFrontWall);
    setNorthWall(x, y, hasRightWall);
    //setSouthWall(x,y,0);
  }
//  turnRight();
//  finishTurn();
//  orientation = (orientation+1)%4;
//  hasRightWall = readRightWallSensor();
//  if (orientation == 0){
//    if (hasRightWall) setEastWall(x, y, 1);
//  }else if (orientation == 1){
//    if (hasRightWall) setSouthWall(x, y, 1);
//  }else if (orientation == 2){
//    if (hasRightWall) setWestWall(x, y, 1);
//  }else{
//    if (hasRightWall) setNorthWall(x, y, 1);
//  }
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
      if (i == fft_cycle) {
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
  int val = 0;
  for (int i = 0; i < 5; i++) {
    val += analogRead(frontWallSensor);
  }
  val = val / 5;
  if (val>180){
    digitalWrite(frontWallLED, HIGH);
    return 1;
  }
  else{
    digitalWrite(frontWallLED, LOW);
    return 0;
  }
//  delay(5);
//  int val = analogRead(frontWallSensor);
//  //Serial.print(val);
//  //Serial.print(" ");
//  if (val>180){
//    digitalWrite(frontWallLED, HIGH);
//    return 1;
//  }
//  else{
//    digitalWrite(frontWallLED, LOW);
//    return 0;
//  }
}

/* read right-facing sensor. 
 *  Returns 1 for wall detected and 0 for no wall detected
 */
int readRightWallSensor() {

  int val = 0;
  for (int i = 0; i < 5; i++) {
    val += analogRead(rightWallSensor);
  }
  val = val / 5;
  if (val>180){
    digitalWrite(rightWallLED, HIGH);
    return 1;
  }
  else{
    digitalWrite(rightWallLED, LOW);
    return 0;
  }
//  delay(5);
//  int val = analogRead(rightWallSensor);
//  //Serial.print(val);
//  //Serial.print(" ");
//  if (val>180){
//    digitalWrite(rightWallLED, HIGH);
//    return 1;
//  }
//  else{
//    digitalWrite(rightWallLED, LOW);
//    return 0;
//  }
}
  
/* returns 0 if white detected, 1 if black */
int readLeftSensor(){
  digitalWrite(mux0, LOW);
  digitalWrite(mux1, LOW);
  delay(muxReadDelay);
  int val = analogRead(muxRead);
  //Serial.print(val);
  //Serial.print(" ");
  return val > 357? 1:0;//357
  }
int readRightSensor(){
  digitalWrite(mux0, HIGH);
  digitalWrite(mux1, LOW);
  delay(muxReadDelay);
  int val = analogRead(muxRead);
  //Serial.print(val);
  //Serial.print(" ");
  return val > 297? 1:0;//297
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
