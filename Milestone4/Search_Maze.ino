/* 
 * Functions that handle search decision making
 * 
 */

const bool avoid_robot = true;//avoid the space where robot was last in next search

//once moves to next node have been created, run all of the moves in the movement stack
void moveToNextUnexplored()
{
  //set previous coordinates to current coordinates before move
  while (!movementStack.isEmpty())
  {
    char move = movementStack.pop();
    //    Serial.print("Move: ");
    //    Serial.println(move);
    if (performAction(move) == 0)
    {
      //abort
      clearMovementStack();
    }
  }
}

void clearMovementStack()
{
  while (!movementStack.isEmpty())
  {
    movementStack.pop();
  }
}

//f: forward
//l: turn left
//r: turn right
//t: turn 180 deg. and forward (don't use)
//s: stop
//returns 1 if action was performed successfully, 0 if action was aborted
int performAction(char m)
{
  prev_x = x;
  prev_y = y;
  //TODO: if going to run into wall, return
  //TODO: also have robot detection here(exit if there is a robot in the path)
  if (m == 'f')
  {
    //Serial.println("Going Forward");
    // if (enable_abort && checkForObstacle())
    //   return 0;
    leaveIntersection();
    if (forwardAndStop())
    {
      updateCoor();
    }
    else if (enable_abort)
    {
      detected_robot = nextCoor(x,y,orientation);//avoid this spot next time
      robot_detected = true;
      abortMove();
      return 0;
    }
  }
  else if (m == 'l')
  {
    //Serial.println("Going Left");
    turnLeft();
    finishTurn();
    orientation = (orientation == 0) ? 3 : orientation - 1;
    // if (enable_abort && checkForObstacle())
    //   return 0;
    if (forwardAndStop())
    {
      updateCoor();
    }
    else if (enable_abort)
    {
      detected_robot = nextCoor(x,y,orientation);//avoid this spot next time
      robot_detected = true;
      abortMove();
      return 0;
    }
  }
  else if (m == 'r')
  {
    //Serial.println("Going Right");
    turnRight();
    finishTurn();
    orientation = (orientation == 3) ? 0 : (orientation + 1);
    // if (enable_abort && checkForObstacle())
    //   return 0;
    if (forwardAndStop())
    {
      updateCoor();
    }
    else if (enable_abort)
    {
      detected_robot = nextCoor(x,y,orientation);//avoid this spot next time
      robot_detected = true;
      abortMove();
      return 0;
    }
  }
  else if (m == 't')
  {
    //Serial.println("Turning");
    turnLeft();
    finishTurn();
    orientation = (orientation == 0) ? 3 : orientation - 1;
    turnLeft();
    finishTurn();
    orientation = (orientation == 0) ? 3 : orientation - 1;
    // if (enable_abort && checkForObstacle())
    //   return 0;
    // forwardAndStop();
    // updateCoor();
    if (forwardAndStop())
    {
      updateCoor();
    }
    else if (enable_abort)
    {
      detected_robot = nextCoor(x,y,orientation);//avoid this spot next time
      robot_detected = true;
      abortMove();
      return 0;
    }
  }
  else
  {
    stopMovement();
  }
  robot_detected = false;//no robot encountered
  return 1;
}

bool checkForObstacle()
{
  fft_at_intersection();
  return (fft_detect);
}

// int toNextSquare()
// {
//   if (forwardAndStop())
//   {
//     updateCoor();
//     return 1;
//   }
//   //aborted movement
//   else
//   {
//     turnAround();
//     forwardAndStop();
//     orientation = (orientation == 0) ? 3 : orientation - 1;
//     orientation = (orientation == 0) ? 3 : orientation - 1;
//   }
// }
/**
 * Turn around and go back to where you came from (to where there is not wall/robot)
 */
void abortMove()
{
  turnAround();
  orientation = (orientation == 0) ? 3 : orientation - 1;
  orientation = (orientation == 0) ? 3 : orientation - 1;
  if (forwardAndStop() == 0)
  {
    return abortMove(); //try this again
  }
}

//adds new stuff to the frontier at x,y [Order: R,L,F].
void updateSearch()
{
  int hasFrontWall = readForwardWallSensor();
  int hasRightWall = readRightWallSensor();
  int hasLeftWall = readLeftWallSensor();
  int xn = 0;
  int yn = 0;

  //check right square
  if (hasRightWall == 0)
  {
    int xn = 0;
    int yn = 0;
    if (orientation == 0)
    { //north: east
      xn = x;
      yn = y + 1;
    }
    else if (orientation == 1)
    { //east: south
      xn = x + 1;
      yn = y;
    }
    else if (orientation == 2)
    { //south: west
      xn = x;
      yn = y - 1;
    }
    else
    { //west: north
      xn = x - 1;
      yn = y;
    }
    if (has_visited(getPosition(xn, yn)) == false)
      append_frontier(getPosition(xn, yn));
  }

  //check left square
  if (hasLeftWall == 0)
  {
    xn = 0;
    yn = 0;
    if (orientation == 0)
    { //facing north, update west
      xn = x;
      yn = y - 1;
    }
    else if (orientation == 1)
    { //facing east, update north
      xn = x - 1;
      yn = y;
    }
    else if (orientation == 2)
    { //facing south, update east
      xn = x;
      yn = y + 1;
    }
    else
    { //facing east, update south
      xn = x + 1;
      yn = y;
    }
    if (has_visited(getPosition(xn, yn)) == false)
      append_frontier(getPosition(xn, yn));
    //add left space coordinates to frontier
  }

  //check forward square
  if (hasFrontWall == 0)
  {
    xn = 0; //init
    yn = 0; //init
    if (orientation == 0)
    { //facing north
      xn = x - 1;
      yn = y;
    }
    else if (orientation == 1)
    { //facing east
      xn = x;
      yn = y + 1;
    }
    else if (orientation == 2)
    { //facing south
      xn = x + 1;
      yn = y;
    }
    else
    { //facing west
      xn = x;
      yn = y - 1;
    }
    if (has_visited(getPosition(xn, yn)) == false)
      append_frontier(getPosition(xn, yn));
  }
}

//returns the next unexplored position in the maze to move to
int next_position()
{
  while (!frontier_empty())
  {
    int next = next_frontier();
    if (!isExplored(getX(next), getY(next)))
    {
      return next;
    }
  }
  return getPosition(x, y);
}

//use character list to construct moves? ["f","l","r","t"...]
//f: forward
//l: turn left and forward
//r: turn right and forward
//t: turn 180 deg. and forward
//s: stop

//move from x,y to new_x, new_y
//TODO: return array of chars generated using BFS
char get_next_move(int x, int y, int new_x, int new_y, int orientation)
{
  //check if there is a wall between them
  if (x - new_x == 1 && y == new_y)
  {
    //Go North, if there is no wall
    if (getNorthWall(x, y) == 0)
    {
      if (orientation == 0)
        return 'f';
      else if (orientation == 1)
        return 'l';
      else if (orientation == 2)
        return 't';
      else if (orientation == 3)
        return 'r';
      else
        return 's';
    }
  }
  else if (y - new_y == -1 && x == new_x)
  {
    //go east, if possible
    if (getEastWall(x, y) == 0)
    {
      if (orientation == 0)
        return 'r';
      else if (orientation == 1)
        return 'f';
      else if (orientation == 2)
        return 'l';
      else if (orientation == 3)
        return 't';
      else
        return 's';
    }
  }
  else if (x - new_x == 1 && y == new_y)
  {
    //Go South
    if (getSouthWall(x, y) == 0)
    {
      if (orientation == 0)
        return 't';
      else if (orientation == 1)
        return 'r';
      else if (orientation == 2)
        return 'f';
      else if (orientation == 3)
        return 'l';
      else
        return 's';
    }
  }
  else if (y - new_y == 1 && x == new_x)
  {
    //Go West
    if (getSouthWall(x, y) == 0)
    {
      if (orientation == 0)
        return 'l';
      else if (orientation == 1)
        return 't';
      else if (orientation == 2)
        return 'r';
      else if (orientation == 3)
        return 'f';
      else
        return 's';
    }
  }
  //TODO: BFS
  return 's';
}

/*  Returns true if there is no forward wall 
 *  in orientation [orientation] at x,y, false otherwise
 *  
 *  Requires: x,y is explored
 */
bool canGoForward(int x, int y, int orientation)
{
  if(avoid_robot && robot_detected) {
    if(nextCoor(x,y,orientation) == detected_robot) return false;
  }
  if (orientation == 0 && getNorthWall(x, y) == 0)
  {
    return true;
  }
  else if (orientation == 1 && getEastWall(x, y) == 0)
  {
    return true;
  }
  else if (orientation == 2 && getSouthWall(x, y) == 0)
  {
    return true;
  }
  else if (orientation == 3 && getWestWall(x, y) == 0)
  {
    return true;
  }
  return false;
}

/*  Returns true if there is no left wall 
 *  in orientation [orientation] at x,y, false otherwise
 *  
 *  Requires: x,y is explored
 */
bool canGoLeft(int x, int y, int orientation)
{
  if(avoid_robot && robot_detected) {
    int next_orientation = (orientation == 0) ? 3 : orientation - 1;
    if(nextCoor(x,y,next_orientation) == detected_robot) return false;
  }
  if (orientation == 0 && getWestWall(x, y) == 0)
  {
    return true;
  }
  else if (orientation == 1 && getNorthWall(x, y) == 0)
  {
    return true;
  }
  else if (orientation == 2 && getEastWall(x, y) == 0)
  {
    return true;
  }
  else if (orientation == 3 && getSouthWall(x, y) == 0)
  {
    return true;
  }
  return false;
}

/*  Returns true if there is no right wall 
 *  in orientation [orientation] at x,y, false otherwise
 *  
 *  Requires: x,y is explored
 */
bool canGoRight(int x, int y, int orientation)
{
  if(avoid_robot && robot_detected) {
    int next_orientation = (orientation == 3) ? 0 : orientation + 1;
    if(nextCoor(x,y,next_orientation) == detected_robot) return false;
  }
  if (orientation == 0 && getEastWall(x, y) == 0)
  {
    return true;
  }
  else if (orientation == 1 && getSouthWall(x, y) == 0)
  {
    return true;
  }
  else if (orientation == 2 && getWestWall(x, y) == 0)
  {
    return true;
  }
  else if (orientation == 3 && getNorthWall(x, y) == 0)
  {
    return true;
  }
  return false;
}

bool canGoBackwards(int x, int y, int orientation)
{
  if(avoid_robot && robot_detected) {
    int next_orientation = (orientation == 3) ? 0 : orientation + 1;
    next_orientation = (next_orientation == 3) ? 0 : next_orientation + 1;
    if(nextCoor(x,y,next_orientation) == detected_robot) return false;
  }
  if (orientation == 0 && getSouthWall(x, y) == 0)
  {
    return true;
  }
  else if (orientation == 1 && getWestWall(x, y) == 0)
  {
    return true;
  }
  else if (orientation == 2 && getNorthWall(x, y) == 0)
  {
    return true;
  }
  else if (orientation == 3 && getEastWall(x, y) == 0)
  {
    return true;
  }
  return false;
}

//at x,y, facing [orientation], the next square coordinate
int nextCoor(int x, int y, int orientation)
{
  byte xn;
  byte yn;
  if (orientation == 0)
  {
    xn = (x == 0) ? 0 : x - 1;
    yn = y;
  }
  else if (orientation == 1)
  {
    xn = x;
    yn = (y == colLength - 1) ? y : y + 1; //changed
  }
  else if (orientation == 2)
  {
    xn = (x == rowLength - 1) ? x : x + 1; //changed
    yn = y;
  }
  else
  {
    xn = x;
    yn = (y == 0) ? 0 : y - 1;
  }
  return getPosition(xn, yn);
}
