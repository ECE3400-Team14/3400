/*
 * Functions that read sensor inputs
 * 
 */

/* read forward-facing sensor
 * Returns 1 for wall detected and 0 for no wall detected
 */
int readForwardWallSensor()
{
  //delay(5);
  int val = 0;
  for (int i = 0; i < 5; i++)
  {
    val += analogRead(frontWallSensor);
  }
  val = val / 5;
  if (val > 130)
  {
    digitalWrite(frontWallLED, HIGH);
    return 1;
  }
  else
  {
    digitalWrite(frontWallLED, LOW);
    return 0;
  }
  //  delay(5);
  //  int val = analogRead(frontWallSensor);
  //  //Serial.print(val);
  //  //Serial.print(" ");
  //  if (val>180){
  //    digitalWrite(frontWallLED, HIGH);
  //    return 1;
  //  }
  //  else{
  //    digitalWrite(frontWallLED, LOW);
  //    return 0;
  //  }
}

/* read right-facing sensor. 
 *  Returns 1 for wall detected and 0 for no wall detected
 */
int readRightWallSensor()
{
  //delay(5);
  int val = 0;
  for (int i = 0; i < 10; i++)
  {
    val += analogRead(rightWallSensor);
  }
  val = val / 10;
  if (val > 160)
  {
    digitalWrite(rightWallLED, HIGH);
    return 1;
  }
  else
  {
    digitalWrite(rightWallLED, LOW);
    return 0;
  }
  //  delay(5);
  //  int val = analogRead(rightWallSensor);
  //  //Serial.print(val);
  //  //Serial.print(" ");
  //  if (val>180){
  //    digitalWrite(rightWallLED, HIGH);
  //    return 1;
  //  }
  //  else{
  //    digitalWrite(rightWallLED, LOW);
  //    return 0;
  //  }
}

int readLeftWallSensor()
{
  //delay(5);
  int val = 0;
  for (int i = 0; i < 5; i++)
  {
    val += analogRead(leftWallSensor);
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
  delay(muxReadDelay);
  int val = analogRead(muxRead);
  //Serial.print(val);
  //Serial.print(" ");
  return val > 500 ? 1 : 0; //400
}
int readRightSensor()
{
  digitalWrite(mux0, HIGH);
  digitalWrite(mux1, LOW);
  delay(muxReadDelay);
  int val = analogRead(muxRead);
  //Serial.print(val);
  //Serial.print(" ");
  return val > 450 ? 1 : 0; //350
}
int readLeftmostSensor()
{
  digitalWrite(mux0, HIGH);
  digitalWrite(mux1, HIGH);
  delay(muxReadDelay);
  int val = analogRead(muxRead);
  //Serial.print(val);
  //Serial.print(" ");
  return val > 750 ? 1 : 0; //700
}
int readRightmostSensor()
{
  digitalWrite(mux0, LOW);
  digitalWrite(mux1, HIGH);
  delay(muxReadDelay);
  int val = analogRead(muxRead);
  //Serial.print(val);
  //  Serial.print("\n");
  return val > 700 ? 1 : 0; //570//630
}

/* read forward-facing sensor with high threshold (for avoiding crashes)
 * Returns 1 for wall detected and 0 for no wall detected
 */
int readForwardWallClose()
{
  //delay(5);
  int val = 0;
  for (int i = 0; i < 5; i++)
  {
    val += analogRead(frontWallSensor);
  }
  val = val / 5;
  if (val > 280)
  {
    //digitalWrite(frontWallLED, HIGH);
    return 1;
  }
  else
  {
    digitalWrite(frontWallLED, LOW);
    return 0;
  }
}
