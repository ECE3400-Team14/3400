/* Code for running the 3400 Robot
 * This file contains global fields, as well as logic for searching algorithms and debugging the robot.
 * 
 * Written by: David Burgstahler (dfb93), Gregory Kaiser (ghk48), Andrew Lin (yl656), and Michaelangelo Rodriguez Ayala (mr2242)
 */

/*ROBOT SETTINGS*/
const bool debug = false;
const bool transmit = true; //true: transmits maze data via. radio
const bool treasure = true; //true: detects treasures

const int debug_mode = 2;
/* 0: Line Sensors, Wall Sensors, Robot Detection
 * 1: Radio (transmit circle)
 * 2: Treasures (just read treasures)
 */

//Maze Size
#define rowLength 9 //maximum y + 1
#define colLength 9 //maximum x + 1

/////////////*************////////////////////////

#include <LinkedList.h>
#include <StackArray.h>
#include <Servo.h>
#include "printf.h"
Servo left;
Servo right;
#define buttonPin 0   //pin assigned to start button (NOT CURRENTLY IN USE)
#define mux0 2        //line sensor mux input 0
#define mux1 4        //line sensor mux input 1
#define mux2 7        //line sensor mux input 2
int muxRead = A3;     //line/wall sensor mux input
int muxReadDelay = 6; //ms delay before reading from the mux to handle some switching issues
int fft_cycle = 12;   //number of movement cycles between FFT detections (see forwardAndStop())
int fft_mux_pin = 6;  //pin for selecting Audio/IR signal

//fft settings
bool fft_detect = false;  //starting state of fft
bool has_started = false; //false: wait for audio signal

bool left_start = false; //indicates if robot has left start

int detected_robot = -1;        //the approximate coordinates of a detected robot (if found), -1 by default
const bool enable_abort = true; //enables/disables movement aborts
bool robot_detected = false;    //true if robot was detected

volatile bool finished_search = false; //indicates when a search is finished
bool start_dir = false;                // false: prioritize [left] over [right]. true: prioritize [right] over [left]

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
  pinMode(mux0, OUTPUT);
  pinMode(mux1, OUTPUT);
  pinMode(mux2, OUTPUT);
  pinMode(fft_mux_pin, OUTPUT);

  //fpga setup:
  if (treasure)
  {
    if (debug)
      Serial.println("Setting up Camera...");
    camera_setup();
    setupComm(); //serial comm setup
  }

  analogRead(1); //initialize analog

  fft_setup();
  if (debug)
    Serial.println("FFT");

  digitalWrite(fft_mux_pin, LOW);

  //wait for start signal
  if (!debug || debug_mode == 0)
  {
    while (has_started == false)
    {
      fft_analyze();
      //Serial.println("Waiting");
      delay(5);
    }
  }
  else
  {
    has_started == true;
  }
  digitalWrite(fft_mux_pin, HIGH);
  fft_analyze(); //reset fft

  initMaze();
  printf_begin();

  //setup radio
  if (transmit)
  {
    radioSetup();
  }

  //init start position:
  updateMaze();
  if (transmit)
  {
    sendMaze(x, y);
  }
}

//LOOP
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
      if (transmit)
      {
        sendFullMaze();
      }
      resetSearch();          //reset explored
      start_dir = !start_dir; //switch to right-left search
      finished_search = false;
    }
  }
  //debug
  else
  {
    //test sensors
    if (debug_mode == 0)
    {
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
      if (treasure)
      {
        readShape();
      }

      updateMaze();
      if (transmit)
      {
        sendMaze(x, y);
      }
      delay(1000);
    }
    //fpga/camera
    else if (debug_mode == 2)
    {
      delay(500);
      if (treasure)
      {
        readShape();
      }
      //Serial.print("Loop");
      Serial.println();
    }
  }
}

/* Runs Dijkstra's algorithm to move between squares
 *  Adapted from Lecture 17, Slide 14
 * 
 * At each square, this algorithm performs a search for the next closest frontier square that is accessible
 * from all visited squares. 
 * 
 * Once this square is found, it generates the path requiring the fewest movements (1 movement = move between squares)
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
  int hasFrontWall = readForwardWallSensor();
  int hasRightWall = readRightWallSensor();
  int hasLeftWall = readLeftWallSensor();

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
    if (!left_start && x == start_x && y == start_y)
    {
      setEastWall(x, y, 1); //robot starts with wall behind it
      left_start = true;
    }
  }
  setExplored(x, y, 1);

  if (treasure && hasRightWall && getShape(x, y) == 0)
  {
    readShape(); //analyze wall for treasure
  }
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
 */
void sendFullMaze()
{
  for (int i = 0; i < mazeSize; i++)
  {
    sendMaze(getX(i), getY(i));
  }
}

//make all coordinate unexplored:
void resetSearch()
{
  for (int i = 0; i < mazeSize; i++)
  {
    setExplored(getX(i), getY(i), 0);
  }
}

//an OLD Wall-following Search Algorithms (not used in competition)/////
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
  }
  else if (hasRightWall == 0)
  {
    turnRight();
    finishTurn();
    orientation = (orientation == 3) ? 0 : (orientation + 1);
    forwardAndStop();
    updateCoor();
  }
}
