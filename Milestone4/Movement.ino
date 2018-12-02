/*
 * Functions controlling robot movement
 * 
 */
#define wall_cycle 44

void finishTurn()
{
  while (readLeftSensor() == 0 || readRightSensor() == 0)
    ; //robot exits line (breaks loop when both sensors are dark)
  while (readLeftSensor() == 1 || readRightSensor() == 1)
    ; //robot enters line again (both line sensors light)
  stopMovement();
}

void leaveIntersection()
{
  forward();
  while (readLeftmostSensor() == 0 || readRightmostSensor() == 0)
    ;
  delay(20); //changed
  //stopMovement();
}

void forwardUntilOffIntersection()
{
  while (readLeftmostSensor() == 0 && readRightmostSensor() == 0)
  {
    trackLine();
  }
}

/**
 * returns 1 if action completes, 0 if aborted (robot or wall)
 */
int forwardAndStop()
{
  //pre-check
  fft_analyze();
  if(enable_abort && fft_detect) {
    stopMovement;
    return 0;
  }
  int i = 0;
  int j = 0;
  while (readLeftmostSensor() == 1 || readRightmostSensor() == 1)
  {

    //Serial.println("BEGIN");
    trackLine();
    //perform FFT every __ number of cycles

    //every [fft_cycle] cycles, check for robot or close wall
    if (i == fft_cycle)
    {
      if (debug)
        Serial.println("Running fft");
      fft_analyze();
      if (enable_abort && (fft_detect /*|| readForwardWallClose())*/))
      {
        //abort
        stopMovement();
        return 0;
        // backwardsAndStop();
        // while (fft_detect)
        // {
        //   delay(20);
        //   fft_analyze();
        // } //Added
        // leaveIntersection();
        // //delay(20);
      }

      i = 0;
    }
    else
    {
      i++;
    }
    //don't crash into walls (hopefully)
    if(j == wall_cycle) {
      while(readForwardWallClose()) {
        stopMovement();
        delay(10);
      }
      j = 0;
    }
    else {
      j++;
    }
  }
  stopMovement();
  return 1;
}

/**
 * Robot performs 180 degree turn at any point on the line
 */
void turnAround()
{
  turnLeft();
  delay(750); //delay subject to change
  while (readLeftSensor() == 1 || readRightSensor() == 1)
    ;
  stopMovement();
}

void backwardsAndStop()
{
  int i = 0;
  while (readLeftmostSensor() == 1 && readRightmostSensor() == 1)
  {
    trackLineBackwards();
  }
  stopMovement();
}

void trackLineBackwards()
{
  if (readLeftSensor() == 0 && readRightSensor() == 0)
    backward();
  else if (readLeftSensor() == 0 && readRightSensor() == 1)
  {
    while (readLeftSensor() == 0)
      forward();
    writeRight(0);
    writeLeft(90);
  }
  else if (readLeftSensor() == 1 && readRightSensor() == 0)
  {
    while (readRightSensor() == 0)
      forward();
    writeLeft(0);
    writeRight(90);
  }
  else
    backward();
}

void forwardAndLeft()
{
  while (readLeftmostSensor() == 1 || readRightmostSensor() == 1)
  { //This statement detects an intersection (both outer sensors go light to break loop)
    trackLine();
  }
  turnLeft();
  while (readLeftSensor() == 0 && readRightSensor() == 0)
    ; //robot exits line (both line sensors dark)
  while (readLeftSensor() == 1 || readRightSensor() == 1)
    ; //robot enters line again (both line sensors light)
  stopMovement();
}

void forwardAndRight()
{
  while (readLeftmostSensor() == 1 || readRightmostSensor() == 1)
  {
    trackLine();
  }
  turnRight();
  while (readLeftSensor() == 0 && readRightSensor() == 0)
    ;
  while (readLeftSensor() == 1 || readRightSensor() == 1)
    ;
  stopMovement();
}

void trackLine()
{
  if (readLeftSensor() == 0 && readRightSensor() == 0)
    forward();
  else if (readLeftSensor() == 0 && readRightSensor() == 1)
  {
    writeRight(180);
    writeLeft(90);
  }
  else if (readLeftSensor() == 1 && readRightSensor() == 0)
  {
    writeLeft(180);
    writeRight(90);
  }
  else
    backward();
}

/* writing 180 to both functions would go forward, 0 would go backward*/
void writeLeft(int speed)
{
  left.write(speed);
}
void writeRight(int speed)
{
  right.write(180 - speed);
}

void backward()
{
  writeLeft(0);
  writeRight(0);
}
void forward()
{
  writeLeft(180);
  writeRight(180);
}
void turnLeft()
{
  writeLeft(20);   //0
  writeRight(160); //180
}
void turnRight()
{
  writeLeft(160); //180
  writeRight(20); //0
}
void stopMovement()
{
  writeLeft(90);
  writeRight(90);
}
