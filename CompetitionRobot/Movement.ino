/*
 * Functions controlling robot movement
 * Written by: Andrew Lin (yl656), David Burgstahler (dfb93) 
 */
#define wall_cycle 44

/**
 * Robot turns until it detects new line to follow
 */
void finishTurn()
{
  while (readLeftSensor() == 0 || readRightSensor() == 0)
    ; //robot exits line (breaks loop when both sensors are dark)
  while (readLeftSensor() == 1 || readRightSensor() == 1)
    ; //robot enters line again (both line sensors light)
  stopMovement();
}

/**
 * Robot leave an intersection
 */
void leaveIntersection()
{
  forward();
  while (readLeftmostSensor() == 0 || readRightmostSensor() == 0)
    ;
  delay(20); //changed
  //stopMovement();
}

/**
 * (OLDER VERSION) Robot leave an intersection
 */
void forwardUntilOffIntersection()
{
  while (readLeftmostSensor() == 0 && readRightmostSensor() == 0)
  {
    trackLine();
  }
}

/**
 * Robot moves forward until it reaches an intersection, or 
 * until it detects a robot or a wall.
 * returns 1 if action completes (found intersection ), 0 if aborted (robot or wall detected)
 */
int forwardAndStop()
{
  int i = 0;
  int j = 0;
  while (readLeftmostSensor() == 1 || readRightmostSensor() == 1)
  {
    trackLine();
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
      }
      i = 0;
    }
    else
    {
      i++;
    }
    //don't crash into walls
    if (j == wall_cycle)
    {
      while (readForwardWallClose())
      {
        stopMovement();
        delay(10);
      }
      j = 0;
    }
    else
    {
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

/**
 * Checks that the robot is on the line. 
 * -Robot is left of the line: go forward
 * -Robot is on right side of the line: turn left
 * -Robot is on the left side of the line: turn right
 * -Robot is non on the line: go backwards
 */
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

//write servos to go backwards
void backward()
{
  writeLeft(0);
  writeRight(0);
}

//write servos to move robot fowward
void forward()
{
  writeLeft(180);
  writeRight(180);
}

//write servos to turn robotleft
void turnLeft()
{
  writeLeft(20);   //0
  writeRight(160); //180
}

//write servos to turn robot right
void turnRight()
{
  writeLeft(160); //180
  writeRight(20); //0
}

//stop the robot from moving
void stopMovement()
{
  writeLeft(90);
  writeRight(90);
}
