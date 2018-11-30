//robot settings
const bool debug = false;
const bool transmit = true;

const bool debug1 = false;
const bool debug2 = true;

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
int rightWallSensor = A5; //read right wall sensor data
int rightWallLED = 7;     //TEMP
int frontWallSensor = A4;
int leftWallSensor = A1;
int frontWallLED = 8;
#define mux0 2        //line sensor mux input 0
#define mux1 4        //line sensor mux input 2
int muxRead = A3;     //line sensor input
int muxReadDelay = 6; //ms delay before reading from the mux to handle some switching issues
int fft_cycle = 10;   //number of movement cycles between FFT detections (see forwardAndStop())
int fft_mux_pin = 6;  //pin for selecting Audio/IR signal
const int fft_intersection_cycles = 3;

//fft settings
bool fft_detect = false;  //starting state of fft
bool has_started = false; //false: wait for audio signal

int detected_robot = -1;        //the approximate coordinates of a detected robot (if found), -1 by default
const bool enable_abort = true; //enables/disables movement aborts
bool robot_detected = false;//true if robot was detected
//maze data

#define rowLength 9 //y
#define colLength 9 //x

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
  //  if (debug)
  //  {
  //
  //  } //TEST that this works without Serial.begin, test D0 and D1
  pinMode(buttonPin, INPUT);
  pinMode(rightWallSensor, INPUT);
  pinMode(frontWallSensor, INPUT);
  pinMode(rightWallLED, OUTPUT);
  pinMode(frontWallLED, OUTPUT);
  pinMode(mux0, OUTPUT);
  pinMode(mux1, OUTPUT);
  pinMode(fft_mux_pin, OUTPUT);
  //Serial.println("Wait for button");

  //uncomment for start button
  //while(digitalRead(buttonPin)==LOW);

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
    //search();
    //rightWallFollowing();
    dijkstra_search();
  }
  //debug
  else
  {
  if(debug1) {
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
    Serial.println();
    Serial.println();
  }
  else if (debug2) {
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
           sendMaze();
           delay(1000);
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
    sendMaze();
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
  if (orientation == 0)
  {
    setNorthWall(x, y, hasFrontWall);
    setEastWall(x, y, hasRightWall);
    setWestWall(x, y, hasLeftWall);
    //robot starts with wall behind it
    if (x == start_x && y == start_y)
      setSouthWall(x, y, 1);
  }
  else if (orientation == 1)
  {
    setEastWall(x, y, hasFrontWall);
    setSouthWall(x, y, hasRightWall);
    setNorthWall(x, y, hasLeftWall);
    if (x == start_x && y == start_y)
      setWestWall(x, y, 1); //robot starts with wall behind it
    //setWestWall(x,y,0);
  }
  else if (orientation == 2)
  {
    setSouthWall(x, y, hasFrontWall);
    setWestWall(x, y, hasRightWall);
    setEastWall(x, y, hasLeftWall);
    //setNorthWall(x,y,0);
    if (x == start_x && y == start_y)
      setNorthWall(x, y, 1); //robot starts with wall behind it
  }
  else
  {
    setWestWall(x, y, hasFrontWall);
    setNorthWall(x, y, hasRightWall);
    setSouthWall(x, y, hasLeftWall);
    //setEastWall(x,y,0);
    if (x == start_x && y == start_y)
      setEastWall(x, y, 1); //robot starts with wall behind it
  }
  setExplored(x, y, 1);

  //check for robot
  //  fft_at_intersection();
  //  if(fft_detect) {
  //    //Serial.println("detect");
  //    detected_robot = nextCoor(x,y,orientation);//set the forward coordinate as the approximate spot of a robot
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

/* Is it BFS? Is it Dijkstra? It's kinda both and neither
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
  updateMaze(); //analyze walls, set square as explored
  if (transmit /*&& hasMoved()*/)
  {
    sendMaze();
  }                            //send new maze data
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
    sendMaze();
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
