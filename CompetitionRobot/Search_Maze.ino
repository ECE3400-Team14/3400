/* 
 * Functions that handle search decision making, as well as directing the 
 * robot through the maze
 * Written by: David Burgstahler (dfb93)
 */

const bool avoid_robot = true; //avoid the space where robot was last in next search

bool did_move = false; //true if coordinates changed after an abort

/**
 * once moves to next node have been created by [dijkstra()], run all of the moves in the movement stack
 * */
void moveToNextUnexplored()
{
  //set previous coordinates to current coordinates before move
  while (!movementStack.isEmpty())
  {
    char move = movementStack.pop();

    if (transmit && hasMoved())
    {
      sendMaze(x, y);
    } //send new maze data

    if (performAction(move) == 0)
    {
      //abort
      clearMovementStack();
    }

    //update maze data (TODO: do this here or before: This allows for corrections)
    //updateMaze(); //analyze walls, set square as explored
  }
}

void clearMovementStack()
{
  while (!movementStack.isEmpty())
  {
    movementStack.pop();
  }
}

/**
 * Direct robot to perform movement [m]:
 * f: forward
 * l: turn left then forward
 * r: turn right then forward
 * t: turn 180 deg. and forward (don't use)
 * s: stop
 * returns 1 if action was performed successfully, 0 if action was aborted
 */

int performAction(char m)
{
  did_move = false; //reset (for abort)
  prev_x = x;
  prev_y = y;
  if (m == 'f')
  {
    leaveIntersection();
    if (forwardAndStop())
    {
      updateCoor();
    }
    else if (enable_abort)
    {
      detected_robot = nextCoor(x, y, orientation); //avoid this spot next time
      robot_detected = true;
      abortMove();
      return 0;
    }
  }
  else if (m == 'l')
  {
    turnLeft();
    finishTurn();
    orientation = (orientation == 0) ? 3 : orientation - 1;

    if (treasure && !canGoRight(x, y, orientation) && getShape(x, y) == 0)
    {
      readShape(); //analyze wall for treasure
    }
    if (forwardAndStop())
    {
      updateCoor();
    }
    else if (enable_abort)
    {
      detected_robot = nextCoor(x, y, orientation); //avoid this spot next time
      robot_detected = true;
      abortMove();
      return 0;
    }
  }
  else if (m == 'r')
  {
    turnRight();
    finishTurn();
    orientation = (orientation == 3) ? 0 : (orientation + 1);
    if (forwardAndStop())
    {
      updateCoor();
    }
    else if (enable_abort)
    {
      detected_robot = nextCoor(x, y, orientation); //avoid this spot next time
      robot_detected = true;
      abortMove();
      return 0;
    }
  }
  else if (m == 't')
  {
    turnLeft();
    finishTurn();
    orientation = (orientation == 0) ? 3 : orientation - 1;

    if (treasure && !canGoRight(x, y, orientation) && getShape(x, y) == 0)
    {
      readShape(); //analyze wall for treasure
    }

    turnLeft();
    finishTurn();
    orientation = (orientation == 0) ? 3 : orientation - 1;
    if (forwardAndStop())
    {
      updateCoor();
    }
    else if (enable_abort)
    {
      detected_robot = nextCoor(x, y, orientation); //avoid this spot next time
      robot_detected = true;
      abortMove();
      return 0;
    }
  }
  else
  {
    stopMovement();
  }
  updateMaze();           //update maze at new location
  robot_detected = false; //no robot encountered
  return 1;
}

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
    did_move = !did_move; //indicate reversed course
    return abortMove();   //try this again
  }
  if (did_move)
  {
    updateCoor(); //ended up moving forward: update coordinates
    updateMaze();
  }
}

/*  Returns true if there is no forward wall 
 *  in orientation [orientation] at x,y, false otherwise
 *  
 *  Requires: x,y is explored
 */
bool canGoForward(int x, int y, int orientation)
{
  if (avoid_robot && robot_detected)
  {
    if (nextCoor(x, y, orientation) == detected_robot)
      return false;
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
  if (avoid_robot && robot_detected)
  {
    int next_orientation = (orientation == 0) ? 3 : orientation - 1;
    if (nextCoor(x, y, next_orientation) == detected_robot)
      return false;
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
  if (avoid_robot && robot_detected)
  {
    int next_orientation = (orientation == 3) ? 0 : orientation + 1;
    if (nextCoor(x, y, next_orientation) == detected_robot)
      return false;
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
  if (avoid_robot && robot_detected)
  {
    int next_orientation = (orientation == 3) ? 0 : orientation + 1;
    next_orientation = (next_orientation == 3) ? 0 : next_orientation + 1;
    if (nextCoor(x, y, next_orientation) == detected_robot)
      return false;
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
