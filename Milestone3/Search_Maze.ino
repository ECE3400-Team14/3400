/* 
 * Functions that handle search decision making
 * 
 */


//f: forward
//l: turn left and forward
//r: turn right and forward
//t: turn 180 deg. and forward
//s: stop
//TODO: take in array of actions
void performAction(char m) {
  if(m == 'f') {
    forwardAndStop();
    updateCoor();
  }
  else if (m == 'l') {
    turnLeft();
    finishTurn();
    orientation = (orientation == 0) ? 3 : orientation - 1;
    forwardAndStop();
    updateCoor();
  }
  else if (m = 'r') {
    turnRight();
    finishTurn();
    orientation = (orientation == 3) ? 0 : (orientation+1);
    forwardAndStop();
    updateCoor();
  }
  else if (m = 't') {
    turnLeft();
    finishTurn();
    orientation = (orientation == 0) ? 3 : orientation - 1;
    turnLeft();
    finishTurn();
    orientation = (orientation == 0) ? 3 : orientation - 1;
    forwardAndStop();
    updateCoor();
  }
  else {
    stopMovement();
  }
  
}

//updates current coordinates of robot
void updateCoor(){
  if (orientation == 0){
    x = (x == 0) ? 0 : x - 1;
  }else if (orientation == 1){
    y += 1;
  }else if (orientation == 2){
    x += 1;
  }else{
    y = (y == 0) ? 0 : y - 1;
  }
}

//adds new stuff to the frontier at x,y [Order: R,L,F]. 
void updateSearch() {
  int hasFrontWall = readForwardWallSensor();
  int hasRightWall = readRightWallSensor();
  int hasLeftWall = readLeftWallSensor();
  int xn = 0;
  int yn = 0;

//check right square
  if(hasRightWall == 0) {
    int xn = 0;
    int yn = 0;
    if (orientation == 0){//north: east
      xn = x;
      yn = y+1;
    }else if (orientation == 1){//east: south
      xn = x+1;
      yn = y;
    }else if (orientation == 2){//south: west
      xn = x;
      yn = y-1;
    }else{//west: north
     xn = x-1;
     yn = y;
    }
    if (has_visited(getPosition(xn, yn)) ==false)
        append_frontier(getPosition(xn, yn));
  }

  //check left square
  if(hasLeftWall == 0) {
    xn = 0;
    yn = 0;
    if (orientation == 0) {//facing north, update west
      xn = x;
      yn = y-1;
    }else if (orientation == 1){//facing east, update north
      xn = x-1;
      yn = y;
    }else if (orientation == 2){//facing south, update east
      xn = x;
      yn = y+1;
    }else{//facing east, update south
     xn = x+1;
     yn = y;
    }
    if (has_visited(getPosition(xn, yn)) ==false)
        append_frontier(getPosition(xn, yn));
    //add left space coordinates to frontier
  }
  
  //check forward square
  if(hasFrontWall == 0) {
    xn = 0;//init
    yn = 0;//init
    if (orientation == 0){//facing north
      xn = x-1;
      yn = y;
    }else if (orientation == 1){//facing east
      xn = x;
      yn = y + 1;
    }else if (orientation == 2){//facing south
      xn = x+1;
      yn = y;
    }else{//facing west
     xn = x;
     yn = y-1;
    }
    if (has_visited(getPosition(xn, yn)) ==false)
        append_frontier(getPosition(xn, yn));
  }
  
}

//returns the next unexplored position in the maze to move to
int next_position() {
  while (!frontier_empty()) {
    int next = next_frontier();
    if(!isExplored(getX(next), getY(next))) {
      return next;
    }
  }
  return getPosition(x,y);
}

//use character list to construct moves? ["f","l","r","t"...]
//f: forward
//l: turn left and forward
//r: turn right and forward
//t: turn 180 deg. and forward
//s: stop

//move from x,y to new_x, new_y 
//TODO: return array of chars generated using BFS
char get_next_move(int x, int y, int new_x, int new_y, int orientation) {
  //check if there is a wall between them
  if(x-new_x == 1) {
    //Go North, if there is no wall
    if(getNorthWall(x,y) == 0) {
      if(orientation == 0) return 'f';
      else if(orientation == 1) return 'l';
      else if(orientation == 2) return 't';
      else if(orientation == 3) return 'r';
      else return 's';
    }
    
  }
  else if(y - new_y == -1) {
    //go east, if possible
    if(getEastWall(x,y) == 0) {
      if(orientation == 0) return 'r';
      else if(orientation == 1) return 'f';
      else if(orientation == 2) return 'l';
      else if(orientation == 3) return 't';
      else return 's';
    }
  }
  else if (x - new_x == 1) {
    //Go South
    if(getSouthWall(x,y) == 0) {
      if(orientation == 0) return 't';
      else if(orientation == 1) return 'r';
      else if(orientation == 2) return 'f';
      else if(orientation == 3) return 'l';
      else return 's';
    }
  }
  else if (y - new_y == 1) {
    //Go West
    if(getSouthWall(x,y) == 0) {
      if(orientation == 0) return 'l';
      else if(orientation == 1) return 't';
      else if(orientation == 2) return 'r';
      else if(orientation == 3) return 'f';
      else return 's';
    }
  }
  //TODO: BFS
  return 's';
}





//using three wall sensors
void updateMaze(){
//  turnLeft();
//  finishTurn();
//  orientation = (orientation-1)%4;
  int hasFrontWall = readForwardWallSensor();
  int hasRightWall = readRightWallSensor();
  int hasLeftWall = readLeftWallSensor();
  if (orientation == 0){
    setNorthWall(x, y, hasFrontWall);
    setEastWall(x, y, hasRightWall);
    setWestWall(x,y, hasLeftWall);
    //setWestWall(x,y,0);
  }else if (orientation == 1){
    setEastWall(x, y, hasFrontWall);
    setSouthWall(x, y, hasRightWall);
    setNorthWall(x,y, hasLeftWall);
    //setNorthWall(x,y,0);
  }else if (orientation == 2){
    setSouthWall(x, y, hasFrontWall);
    setWestWall(x, y, hasRightWall);
    setEastWall(x,y, hasLeftWall);
    //setEastWall(x,y,0);
  }else{
    setWestWall(x, y, hasFrontWall);
    setNorthWall(x, y, hasRightWall);
    setSouthWall(x,y, hasLeftWall);
    //setSouthWall(x,y,0);
  }
  setExplored(x,y,1);
}
