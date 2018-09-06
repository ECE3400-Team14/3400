#include <Servo.h>
Servo left;
Servo right;
void setup() {
  // put your setup code here, to run once:
  left.attach(3);
  right.attach(5);
}

void loop() {
  // put your main code here, to run repeatedly:
  forwardOneGrid();
  right90();
  forwardOneGrid();
  left90();
  forwardOneGrid();
  left90();
  forwardOneGrid();
  left90();
  forwardOneGrid();
  left90();
  forwardOneGrid();
  right90();
  forwardOneGrid();
  right90();
  forwardOneGrid();
}
void forwardOneGrid(){
  forward();
  delay(1700);
  stopMovement();
  }
void left90(){
  turnLeft();
  delay(670);
  stopMovement();
  }
void right90(){
  turnRight();
  delay(670);
  stopMovement();
  }
void forward(){
  left.write(180);
  right.write(0);
  }
void backward(){
  left.write(0);
  right.write(180);
  }
void turnLeft(){
  left.write(0);
  right.write(0);
  }
void turnRight(){
  left.write(180);
  right.write(180);
  }
void stopMovement(){
  left.write(90);
  right.write(90);
  }
