/*
 * This file is responsible for the reading of sensor input from all of the eyes and ears
 * of the robot. It contains wall sensor input, line sensor input, and the audio
 * and IR inputs, which rely upon Signal_Processing to return valid values.
 */

 /* read forward-facing sensor
 * Returns 1 for wall detected and 0 for no wall detected
 */
int readForwardWallSensor() {
  int val = analogRead(frontWallSensor);
  //Serial.print(val);
  //Serial.print(" ");
  if (val>180){
    digitalWrite(frontWallLED, HIGH);
    return 1;
  }
  else{
    digitalWrite(frontWallLED, LOW);
    return 0;
  }
}

/* read right-facing sensor. 
 *  Returns 1 for wall detected and 0 for no wall detected
 */
int readRightWallSensor() {
  
  int val = analogRead(rightWallSensor);
  //Serial.print(val);
  //Serial.print(" ");
  if (val>180){
    digitalWrite(rightWallLED, HIGH);
    return 1;
  }
  else{
    digitalWrite(rightWallLED, LOW);
    return 0;
  }
}
  
/* returns 0 if white detected, 1 if black */
int readLeftSensor(){
  digitalWrite(mux0, LOW);
  digitalWrite(mux1, LOW);
  delay(muxReadDelay);
  int val = analogRead(muxRead);
  //Serial.print(val);
  //Serial.print(" ");
  return val > 357? 1:0;
  }
int readRightSensor(){
  digitalWrite(mux0, HIGH);
  digitalWrite(mux1, LOW);
  delay(muxReadDelay);
  int val = analogRead(muxRead);
  //Serial.print(val);
  //Serial.print(" ");
  return val > 297? 1:0;
  }
int readLeftmostSensor(){
  digitalWrite(mux0, HIGH);
  digitalWrite(mux1, HIGH);
  delay(muxReadDelay);
  int val = analogRead(muxRead);
  //Serial.print(val);
  //Serial.print(" ");
  return val > 700? 1:0;
  }
int readRightmostSensor(){
  digitalWrite(mux0, LOW);
  digitalWrite(mux1, HIGH);
  delay(muxReadDelay);
  int val = analogRead(muxRead);
  //Serial.print(val);
//  Serial.print("\n");
  return val > 570? 1:0;
  }
