/* Code for running the 3400 Robot
 * This file contains global fields, as well as logic for searching algorithms and debugging the robot.
 * 
 * Written by: David Burgstahler (dfb93), Gregory Kaiser (ghk48), Andrew Lin (yl656), and Michaelangelo Rodriguez Ayala (mr2242)
 */
#include <QueueArray.h>
#include <StackArray.h>
#include <Servo.h>
#include "printf.h"
Servo left;
Servo right;
int buttonPin = 0;//pin assigned to start button (NOT CURRENTLY IN USE)
int rightWallSensor = A5;//read right wall sensor data                                
int rightWallLED = 7;//TEMP
int frontWallSensor = A4;                                                                                          
int leftWallSensor = A1;
int frontWallLED = 8;
int mux0=2;//line sensor mux input 0
int mux1=4;//line sensor mux input 2
int muxRead = A3;//line sensor input
int muxReadDelay = 6; //ms delay before reading from the mux to handle some switching issues
int fft_cycle = 5;//number of movement cycles between FFT detections (see forwardAndStop())
int fft_mux_pin = 6;//pin for selecting Audio/IR signal

//fft settings
bool fft_detect = false;//starting state of fft
bool has_started = false;//false: wait for audio signal

//maze data
#define rowLength 2
#define colLength 3
const int mazeSize = rowLength*colLength;

//starting position
int orientation = 0; //0=north, 1=east, 2=south, 3=west
int x = 1;
int y = 2;

StackArray<char>* movementStack;//stack of movements to follow

//robot settings
bool debug = false;
bool transmit = true;

void setup() {
  left.attach(5);//left servo pin 5
  right.attach(3);//right servo pin 3
  stopMovement();
  if(debug) {Serial.begin(9600);}//TEST that this works without Serial.begin, test D0 and D1
  pinMode(buttonPin, INPUT);
  pinMode(rightWallSensor, INPUT);
  pinMode(frontWallSensor, INPUT);
  pinMode(rightWallLED, OUTPUT);
  pinMode(frontWallLED, OUTPUT);
  pinMode(mux0, OUTPUT);
  pinMode(mux1, OUTPUT);
  pinMode(fft_mux_pin, OUTPUT);
  Serial.println("Wait for button");

  //uncomment for start button
  //while(digitalRead(buttonPin)==LOW);
  
  analogRead(1);//initialize analog
  
  //Serial.println("Button Pressed");
  fft_setup();//ADDED
  Serial.println("FFT Setup Complete");

   digitalWrite(fft_mux_pin, LOW);

//  
  while(has_started == false) {
     fft_analyze();
     Serial.println("Waiting");
  }
  digitalWrite(fft_mux_pin, HIGH);
  
  initMaze();
  printf_begin();
  radioSetup();

  //stack setup
  initialize_search();
  append_frontier(getPosition(x,y));
}

void loop() {
  if (!debug) {
    search();
  }
  //debug
  else {
   // troubleshooting code block:
   int leftmost = readLeftmostSensor();
   Serial.print("LL:");
   Serial.print(leftmost);
   Serial.print("|");
   int left = readLeftSensor ();
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
   int hasRightWall = readRightWallSensor(); 
   int hasFrontWall = readForwardWallSensor();
   int hasLeftWall = readLeftWallSensor();
   Serial.print("LW:");
   Serial.print(hasLeftWall);
   Serial.print(" ");
   Serial.print("FW:");
   Serial.print(hasFrontWall);
   Serial.print(" ");
   Serial.print("RW:");
   Serial.print(hasRightWall);
   //Serial.println();
   Serial.print(", ");
   fft_analyze();
   Serial.println();
   //orientation code:
//   updateCoor();
//   Serial.print("Orientation:");
//   Serial.println(orientation);
//   Serial.print(x);
//   Serial.print(", ");
//   Serial.print(y);
//   Serial.println();
//   orientation = (orientation == 0) ? 3 : orientation - 1; 
//   updateMaze();
//   sendMaze();
//   delay(1000);
  }
}

void rightWallFollowing() {
  int hasRightWall = readRightWallSensor();
   int hasFrontWall = readForwardWallSensor();
   int hasLeftWall = readLeftWallSensor();
   updateMaze();
   if(transmit) { sendMaze(); }
   if (hasRightWall==1&&hasFrontWall==0) {
    forwardAndStop();
    updateCoor();
   }
   else if (hasRightWall==1&&hasFrontWall==1/*&&hasLeftWall==0*/){
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
   else if (hasRightWall==0){
    turnRight();
    finishTurn();
    orientation = (orientation == 3) ? 0 : (orientation+1);
    forwardAndStop();
    updateCoor();
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
void bfs_mod_search() {
  updateMaze();//analyze walls, set square as explored
  bfs_mod(getPosition(x,y));//find the closest frontier square and create a path to it
  moveToNextUnexplored();//perform set of actions gererated by bfs_mod
}

/* (UNIMPLEMENTED) A basic DFS/BFS search algorith*/
void search() {
    //get next frontier position
    int next_pos = next_position();

    //move to next_pos
    char move = get_next_move(x,y,getX(next_pos), getY(next_pos), orientation);
    performAction(move);

    //now at new position
    
    //get wall/treasure information about the new position
    updateMaze();
    
    //update frontier
    updateSearch();
}

void bad_search() {
  int hasRightWall = readRightWallSensor();
   int hasFrontWall = readForwardWallSensor();
   int hasLeftWall = readLeftWallSensor();
   updateMaze();
   if(transmit) { sendMaze(); }
   
   if (hasFrontWall==0) {
    forwardAndStop();
    updateCoor();
   }
   else if (hasLeftWall == 0) {
    turnLeft();
    finishTurn();
    orientation = (orientation == 0) ? 3 : orientation - 1;
    forwardAndStop();
    updateCoor();
   }
   else if (hasRightWall==0){
    turnRight();
    finishTurn();
    orientation = (orientation == 3) ? 0 : (orientation+1);
    forwardAndStop();
    updateCoor();
   }
   else {
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


