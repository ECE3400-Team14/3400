/*ROBOT SETTINGS*/
const bool debug = false;
const bool transmit = true;
const bool treasure = false;

const int debug_mode = 0;
/* 0: Line Sensors, Wall Sensors, Robot Detection
 * 1: Radio (transmit circle)
 * 2: Treasures (just read treasures)
 */

//Maze Size
#define rowLength 5 //y
#define colLength 4 //x


 /////////////*************////////////////////////

/* Code for running the 3400 Robot
 * This file contains global fields, as well as logic for searching algorithms and debugging the robot.
 * 
 * Written by: David Burgstahler (dfb93), Gregory Kaiser (ghk48), Andrew Lin (yl656), and Michaelangelo Rodriguez Ayala (mr2242)
 */
//#include <QueueArray.h>
//#include <QueueList.h>
#include <LinkedList.h>
#include <StackArray.h>
#include <Servo.h>
#include "printf.h"
Servo left;
Servo right;
#define buttonPin 0       //pin assigned to start button (NOT CURRENTLY IN USE)
//int rightWallSensor = A5; //read right wall sensor data
//int rightWallLED = 7;     //TEMP
//int frontWallSensor = A4;
//int leftWallSensor = A1;
//int frontWallLED = 8;
#define mux0 2        //line sensor mux input 0
#define mux1 4        //line sensor mux input 1
#define mux2 7        //TODO: line sensor mux input 2
int muxRead = A3;     //line sensor input
int muxReadDelay = 6; //ms delay before reading from the mux to handle some switching issues
int fft_cycle = 10;   //number of movement cycles between FFT detections (see forwardAndStop())
int fft_mux_pin = 6;  //pin for selecting Audio/IR signal
const int fft_intersection_cycles = 3;

//fft settings
bool fft_detect = false;  //starting state of fft
bool has_started = false; //false: wait for audio signal

bool left_start = false; //indicates if robot has left start

int detected_robot = -1;        //the approximate coordinates of a detected robot (if found), -1 by default
const bool enable_abort = true; //enables/disables movement aborts
bool robot_detected = false;    //true if robot was detected

bool finished_search = false; //indicates when a search is finished
bool start_dir = false;       // false: prioritize [left] over [right]. true: prioritize [right] over [left]
//maze data

const byte mazeSize = rowLength * colLength;

//starting position
#define start_orientation 2
#define start_x 0
#define start_y 0
byte orientation = start_orientation; //0=north, 1=east, 2=south, 3=west
byte x = start_x;
byte y = start_y;

byte prev_x = -1; //previous x coordinate (-1 default)
byte prev_y = -1; //previous y coordinate (-1 by default)

StackArray<char> movementStack; //stack of movements to follow

void setup()
{
  left.attach(5);  //left servo pin 5
  right.attach(3); //right servo pin 3
  stopMovement();
  Serial.begin(9600);
  pinMode(buttonPin, INPUT);
//  pinMode(rightWallSensor, INPUT);
//  pinMode(frontWallSensor, INPUT);
  //  pinMode(rightWallLED, OUTPUT);
  //  pinMode(frontWallLED, OUTPUT);
  pinMode(mux0, OUTPUT);
  pinMode(mux1, OUTPUT);
  pinMode(mux2, OUTPUT);
  pinMode(fft_mux_pin, OUTPUT);
  //Serial.println("Wait for button");

  //uncomment for start button
  //while(digitalRead(buttonPin)==LOW);

  //fpga setup:
  if(treasure) {
    if(debug) Serial.println("Setting up Camera...");
    camera_setup();
    setupComm();//serial comm setup
  }

  analogRead(1); //initialize analog

  //Serial.println("B");
  fft_setup(); //ADDED
  if (debug)
    Serial.println("FFT");

  digitalWrite(fft_mux_pin, LOW);

  //
  while (has_started == false)
  {
    fft_analyze();
    //Serial.println("Waiting");
    delay(10);
  }
  digitalWrite(fft_mux_pin, HIGH);
  fft_analyze(); //reset

  //initMaze(); (removed to save memory)
  printf_begin();
  radioSetup();

  //stack setup
  //initialize_search();
  //append_frontier(getPosition(x, y));
  //delay(10000);//delay start
}

void loop()
{
  if (!debug)
  {
    if (!finished_search)
    {
      dijkstra_search();
    }
    else
    {
      //send the entire maze
      sendFullMaze();
      //switch to right-left search
      start_dir = !start_dir;
      finished_search = false;
    }
  }
  //debug
  else
  {
    //sensors
    if (debug_mode == 0)
    {
      //forwardAndStop();
      //backwardsAndStop();
      //stopMovement();
      // troubleshooting code block:
      int leftmost = readLeftmostSensor();
      Serial.print("LL:");
      Serial.print(leftmost);
      Serial.print("|");
      int left = readLeftSensor();
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
      //    int hasRightWall = readRightWallSensor();
      //    int hasFrontWall = readForwardWallSensor();
      //    int hasLeftWall = readLeftWallSensor();
      //    Serial.print("LW:");
      //    Serial.print(hasLeftWall);
      //    Serial.print(" ");
      //    Serial.print("FW:");
      //    Serial.print(hasFrontWall);
      //    Serial.print(" ");
      //    Serial.print("RW:");
      //    Serial.print(hasRightWall);
      Serial.print(" ");
      Serial.print("CloseWall:");
      Serial.print(readForwardWallClose());
      updateMaze();
      Serial.print(" | ");
      Serial.print("Left:");
      Serial.print(canGoLeft(x, y, orientation));
      Serial.print(" ");
      Serial.print("FW:");
      Serial.print(canGoForward(x, y, orientation));
      Serial.print(" ");
      Serial.print("Right:");
      Serial.print(canGoRight(x, y, orientation));
      Serial.print(" ");

      //Serial.println();
      Serial.print(", ");
      fft_analyze();
      if (fft_detect)
      {
        detected_robot = nextCoor(x, y, orientation); //avoid this spot next time
        robot_detected = true;
      }
      else
      {
        robot_detected = false;
      }
      Serial.println();
      Serial.println();
    }
    //radio
    else if (debug_mode == 1)
    {
      //orientation code:
      updateCoor();
      Serial.print("Orientation:");
      Serial.println(orientation);
      Serial.print(x);
      Serial.print(", ");
      Serial.print(y);
      Serial.println();
      orientation = (orientation == 0) ? 3 : orientation - 1;
      updateMaze();
      sendMaze(x, y);
      delay(1000);
    }
    //fpga/camera
    else if(debug_mode == 2) {
      delay(500);
      readShape();
    }
  }
}

void rightWallFollowing()
{
  int hasRightWall = readRightWallSensor();
  int hasFrontWall = readForwardWallSensor();
  int hasLeftWall = readLeftWallSensor();
  updateMaze();
  if (transmit)
  {
    sendMaze(x, y);
  }
  if (hasRightWall == 1 && hasFrontWall == 0)
  {
    leaveIntersection();
    forwardAndStop();
    updateCoor();
  }
  else if (hasRightWall == 1 && hasFrontWall == 1 /*&&hasLeftWall==0*/)
  {
    turnLeft();
    finishTurn();
    orientation = (orientation == 0) ? 3 : orientation - 1;
    //
    //updateCoor();
  }
  //   else if (hasRightWall==1&&hasFrontWall==1&&hasLeftWall==1){
  //    turnLeft();
  //    finishTurn();
  //    orientation = (orientation == 0) ? 3 : orientation - 1;
  //    turnLeft();
  //    finishTurn();
  //    orientation = (orientation == 0) ? 3 : orientation - 1;
  //    //
  //    //updateCoor();
  //   }
  else if (hasRightWall == 0)
  {
    turnRight();
    finishTurn();
    orientation = (orientation == 3) ? 0 : (orientation + 1);
    forwardAndStop();
    updateCoor();
  }
}

//updates current coordinates of robot
void updateCoor()
{
  if (orientation == 0)
  {
    x = (x == 0) ? 0 : x - 1;
  }
  else if (orientation == 1)
  {
    y += 1;
  }
  else if (orientation == 2)
  {
    x += 1;
  }
  else
  {
    y = (y == 0) ? 0 : y - 1;
  }
}

//using three wall sensors
void updateMaze()
{
  //  turnLeft();
  //  finishTurn();
  //  orientation = (orientation-1)%4;
  int hasFrontWall = readForwardWallSensor();
  int hasRightWall = readRightWallSensor();
  int hasLeftWall = readLeftWallSensor();
  //  if(!isExplored(x,y)) {
  if (orientation == 0)
  {
    setNorthWall(x, y, hasFrontWall);
    setEastWall(x, y, hasRightWall);
    setWestWall(x, y, hasLeftWall);
    //robot starts with wall behind it
    if (!left_start && x == start_x && y == start_y)
    {
      setSouthWall(x, y, 1);
      left_start = true;
    }
  }
  else if (orientation == 1)
  {
    setEastWall(x, y, hasFrontWall);
    setSouthWall(x, y, hasRightWall);
    setNorthWall(x, y, hasLeftWall);
    if (!left_start && x == start_x && y == start_y)
    {
      setWestWall(x, y, 1); //robot starts with wall behind it
      left_start = true;
    }
    //setWestWall(x,y,0);
  }
  else if (orientation == 2)
  {
    setSouthWall(x, y, hasFrontWall);
    setWestWall(x, y, hasRightWall);
    setEastWall(x, y, hasLeftWall);
    //setNorthWall(x,y,0);
    if (!left_start && x == start_x && y == start_y)
    {
      setNorthWall(x, y, 1); //robot starts with wall behind it
      left_start = true;
    }
  }
  else
  {
    setWestWall(x, y, hasFrontWall);
    setNorthWall(x, y, hasRightWall);
    setSouthWall(x, y, hasLeftWall);
    //setEastWall(x,y,0);
    if (!left_start && x == start_x && y == start_y)
    {
      setEastWall(x, y, 1); //robot starts with wall behind it
      left_start = true;
    }
  }
  setExplored(x, y, 1);

  if(treasure && hasRightWall) {
    readShape();//analyze wall for treasure
  }
  //  }
  //checking wal data [TODO: return value indicating mismatched wall read]
  //  else {
  //    if (orientation == 0)
  //    {
  //      bool check = true;
  //      if(hasFrontWall != getNorthWall(x,y)) {
  //        setNorthWall(x, y, hasFrontWall);
  //        check = false;
  //      }
  //      if(hasRightWall != getEastWall(x,y)) {
  //        setEastWall(x,y,hasEastWall);
  //        check = false;
  //      }
  //      setEastWall(x, y, hasRightWall);
  //      setWestWall(x, y, hasLeftWall);
  //      //robot starts with wall behind it
  //      if (!left_start && x == start_x && y == start_y) {
  //        setSouthWall(x, y, 1);
  //        left_start = true;
  //      }
  //    }
  //    else if (orientation == 1)
  //    {
  //      setEastWall(x, y, hasFrontWall);
  //      setSouthWall(x, y, hasRightWall);
  //      setNorthWall(x, y, hasLeftWall);
  //      if (!left_start && x == start_x && y == start_y) {
  //        setWestWall(x, y, 1); //robot starts with wall behind it
  //        left_start = true;
  //      }
  //    //setWestWall(x,y,0);
  //    }
  //    else if (orientation == 2)
  //    {
  //      setSouthWall(x, y, hasFrontWall);
  //      setWestWall(x, y, hasRightWall);
  //      setEastWall(x, y, hasLeftWall);
  //      //setNorthWall(x,y,0);
  //      if (!left_start && x == start_x && y == start_y) {
  //       setNorthWall(x, y, 1); //robot starts with wall behind it
  //        left_start = true;
  //      }
  //    }
  //    else
  //    {
  //      setWestWall(x, y, hasFrontWall);
  //      setNorthWall(x, y, hasRightWall);
  //      setSouthWall(x, y, hasLeftWall);
  //      //setEastWall(x,y,0);
  //      if (!left_start && x == start_x && y == start_y) {
  //        setEastWall(x, y, 1); //robot starts with wall behind it
  //        left_start = true;
  //      }
  //    }
  //  }
}

int fft_at_intersection()
{
  for (int i = 0; i < fft_intersection_cycles; i++)
  {
    fft_analyze();
    if (fft_detect)
      break;
  }
}

/* Runs Dijkstra's algorithm to move between squares
 *  Adapted from Lecture 17, Slide 14
 * 
 * At each square, this algorithm performs a search for the next closest frontier square that is accessible
 * from all visited squares. 
 * 
 * Once this square is found, it generates the path requiring the fewest movements (1 movement = move between squares or a turn)
 * through the explored squares and executes this path
 * 
 */
void dijkstra_search()
{
  //updateMaze(); //analyze walls, set square as explored
  //  if (transmit && hasMoved())
  //  {
  //    sendMaze();
  //  }                            //send new maze data
  dijkstra(getPosition(x, y)); //find the closest frontier square and create a path to it
  moveToNextUnexplored();      //perform set of actions gererated by dijkstra
}

/*
 * Returns true if the robot has moved from its previous location since the last search, or if no previous move is recorded, 
 * false otherwise.
 */
bool hasMoved()
{
  return x != prev_x || y != prev_y;
}

/* 
 * Sends all of the data in the maze
 * 
 */
void sendFullMaze()
{
  for (int i = 0; i < mazeSize; i++)
  {
    sendMaze(getX(i), getY(i));
  }
}

/* (UNIMPLEMENTED) A basic DFS/BFS search algorith*/
void search()
{
  //get next frontier position
  int next_pos = next_position();

  //move to next_pos
  char move = get_next_move(x, y, getX(next_pos), getY(next_pos), orientation);
  performAction(move);

  //now at new position

  //get wall/treasure information about the new position
  updateMaze();

  //update frontier
  updateSearch();
}

void bad_search()
{
  int hasRightWall = readRightWallSensor();
  int hasFrontWall = readForwardWallSensor();
  int hasLeftWall = readLeftWallSensor();
  updateMaze();
  if (transmit)
  {
    sendMaze(x, y);
  }

  if (hasFrontWall == 0)
  {
    forwardAndStop();
    updateCoor();
  }
  else if (hasLeftWall == 0)
  {
    turnLeft();
    finishTurn();
    orientation = (orientation == 0) ? 3 : orientation - 1;
    forwardAndStop();
    updateCoor();
  }
  else if (hasRightWall == 0)
  {
    turnRight();
    finishTurn();
    orientation = (orientation == 3) ? 0 : (orientation + 1);
    forwardAndStop();
    updateCoor();
  }
  else
  {
    //do nothing
    stopMovement();
  }
  //   else if (hasRightWall==1&&hasFrontWall==1&&hasLeftWall==1){
  //    turnLeft();
  //    finishTurn();
  //    orientation = (orientation == 0) ? 3 : orientation - 1;
  //    turnLeft();
  //    finishTurn();
  //    orientation = (orientation == 0) ? 3 : orientation - 1;
  //    //
  //    //updateCoor();
  //   }
}
