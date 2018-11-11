/*
 * Functions controlling robot movement
 * 
 */

 void finishTurn(){
  while (readLeftSensor() == 0 || readRightSensor() == 0);//robot exits line (breaks loop when both sensors are dark)
  while (readLeftSensor() == 1 || readRightSensor() == 1);//robot enters line again (both line sensors light)
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
        fft_analyze();
        while (fft_detect) {
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
  writeLeft(20);//0
  writeRight(160);//180
  }
void turnRight(){
  writeLeft(160);//180
  writeRight(20);//0
  }
void stopMovement(){
  writeLeft(90);
  writeRight(90);
  }
