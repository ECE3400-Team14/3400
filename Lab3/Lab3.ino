#include <Servo.h>
//Servo Wheels
Servo left; Servo right;

//Input Pins
int buttonPin = 7;
int rightWallSensor = A5;
int frontWallSensor = A4;
//MUX constants
int mux0 = 8; //written to digitally to set MUX input
int mux1 = 9;
int muxRead = A3; //read from for analog input
int muxReadDelay = 6; //ms delay before reading from the mux to handle some switching issues

//Output Pins
int frontWallLED = 10; int rightWallLED = 6;

//Other variables
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
  while(digitalRead(buttonPin)==LOW);//waits for start button press
  //Serial.println("Button Pressed");
  fft_setup();
  Serial.println("FFT Setup Complete");
}

void loop() {
  wallFollowAndStop();
}

void wallFollowAndStop(){
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
}

void figureEight(){
  forwardAndLeft();
  forwardAndRight();
  forwardAndRight();
  forwardAndRight();
  forwardAndRight();
  forwardAndLeft();
  forwardAndLeft();
  forwardAndLeft();
}

void troubleshoot(){
   int leftmost = readLeftmostSensor();
   int left = readLeftSensor();
   int right = readRightSensor();
   int rightmost = readRightmostSensor();
   Serial.print("|");
   int hasRightWall = readRightWallSensor(); 
   int hasFrontWall = readForwardWallSensor();
   Serial.print(hasRightWall);
   Serial.print(" ");
   Serial.print(hasFrontWall);
   Serial.println();
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

