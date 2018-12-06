/*
 * Functions that read sensor inputs
 * Written by: Andrew Lin (yl656), Gregory Kaiser (ghk48), David Burgstahler (dfb93) 
 */

/* read forward-facing sensor
 * Returns 1 for wall detected and 0 for no wall detected
 */
int readForwardWallSensor()
{
  //111
  digitalWrite(mux0, HIGH);
  digitalWrite(mux1, HIGH);
  digitalWrite(mux2, HIGH);
  delay(muxReadDelay);
  int val = 0;
  for (int i = 0; i < 5; i++)
  {
    val += analogRead(muxRead); //frontWallSensor
  }
  val = val / 5;
  if (val > 130)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

/* read right-facing sensor. 
 *  Returns 1 for wall detected and 0 for no wall detected
 */
int readRightWallSensor()
{
  //100
  digitalWrite(mux0, LOW);
  digitalWrite(mux1, LOW);
  digitalWrite(mux2, HIGH);
  delay(muxReadDelay);
  int val = 0;
  for (int i = 0; i < 10; i++)
  {
    val += analogRead(muxRead); //rightWallSensor
  }
  val = val / 10;
  if (val > 160)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

int readLeftWallSensor()
{
  //delay(5);
  //101
  digitalWrite(mux0, HIGH);
  digitalWrite(mux1, LOW);
  digitalWrite(mux2, HIGH);
  delay(muxReadDelay);
  int val = 0;
  for (int i = 0; i < 5; i++)
  {
    val += analogRead(muxRead); //leftWallSensor
  }
  val = val / 5;
  if (val > 150)
  {
    //digitalWrite(leftWallLED, HIGH);
    return 1;
  }
  else
  {
    //digitalWrite(leftWallLED, LOW);
    return 0;
  }
}
/* returns 0 if white detected, 1 if black */
int readLeftSensor()
{
  digitalWrite(mux0, LOW);
  digitalWrite(mux1, LOW);
  digitalWrite(mux2, LOW);
  delay(muxReadDelay);
  int val = analogRead(muxRead);
  return val > 700 ? 1 : 0; //400,5,600
}
int readRightSensor()
{
  digitalWrite(mux0, HIGH);
  digitalWrite(mux1, LOW);
  digitalWrite(mux2, LOW);
  delay(muxReadDelay);
  int val = analogRead(muxRead);
  return val > 650 ? 1 : 0; //350,450,550
}
int readLeftmostSensor()
{
  digitalWrite(mux0, HIGH);
  digitalWrite(mux1, HIGH);
  digitalWrite(mux2, LOW);
  delay(muxReadDelay);
  int val = analogRead(muxRead);
  return val > 760 ? 1 : 0;
}
int readRightmostSensor()
{
  digitalWrite(mux0, LOW);
  digitalWrite(mux1, HIGH);
  digitalWrite(mux2, LOW);
  delay(muxReadDelay);
  int val = analogRead(muxRead);
  return val > 780 ? 1 : 0;
}

/* read forward-facing sensor with high threshold (for avoiding crashes)
 * Returns 1 for wall detected and 0 for no wall detected
 */
int readForwardWallClose()
{
  //delay(5);
  digitalWrite(mux0, HIGH);
  digitalWrite(mux1, HIGH);
  digitalWrite(mux2, HIGH);
  delay(muxReadDelay);
  int val = 0;
  for (int i = 0; i < 2; i++)
  {
    val += analogRead(muxRead);
  }
  val = val / 2;
  if (val > 280)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}
