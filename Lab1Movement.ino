#include <Servo.h>
Servo left;
Servo right;
void setup() {
  left.attach(3);
  right.attach(5);
}

void loop() {
  forwardOneBlcok();
  right90();
  forwardOneBlcok();
  left90();
  forwardOneBlcok();
  left90();
  forwardOneBlcok();
  left90();
  forwardOneBlcok();
  left90();
  forwardOneBlcok();
  right90();
  forwardOneBlcok();
  right90();
  forwardOneBlcok();
}
void forwardOneBlcok(){
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
