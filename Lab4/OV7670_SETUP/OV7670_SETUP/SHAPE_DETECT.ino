byte shape_data;

#define wait_time 100
#define switch_length 500
#define sig 2//signal pin 2
#define data A3//data pin A3

void setupComm() {
  pinMode(sig, OUTPUT);
  pinMode(data, INPUT);
  digitalWrite(sig, HIGH);
}

void readShape() {
  shape_data = 0;//reset shape data
  //delay(wait_time);
  //check data line
//  if(readData()) {
//    //shape detected
//    //fill data [SSC]
//    for(int i = 0; i < 3; i++) {
//      switchSig();
//      bitWrite(shape_data, i,readData());
//    }
//    switchSig();//reset
//    analyzeShape();
//  }
//  else {
//    Serial.print("No Shape Detected");
//  }

  switchSig();
  int data = readData();
  Serial.print(data);
  
  
//  validate();
//  for(int i = 3; i > 0; i--) {
//  switchSig();
//     bitWrite(shape_data, i,readData());
//  }
//  //switchSig();//reset
//  analyzeShape();
//  delay(switch_length);
//  Serial.println();
}

void validate() {
  bool valid = false;
  int c = 0;
  while (!valid) {
    switchSig();
    int data = readData();
    Serial.print(data);
    if(c < 5 && data == 1) c = 0;
    else if (c == 5 && data == 0) c = 0;
    else c++;
    if(c == 6) valid = true;
  }
}


byte readData() {
//  int val = analogRead(data);
//  if(val > 500) return 1;
//  return 0;
  return digitalRead(data);
}

void switchSig() {
  digitalWrite(sig, LOW);
  delay(switch_length);
  digitalWrite(sig, HIGH);
  delay(switch_length);
}

void analyzeShape() {
  byte color = shape_data & 0b11;
  byte shape = (shape_data >> 2) & 0b11;
  if(color == 2) Serial.print("Blue ");
  else if (color == 1) Serial.print("Red ");
  else {
    Serial.print("No Shape Detected");
    return;
  }
  switch (shape) {
    case 1 : Serial.print("Diamond ");
             break;
    case 2 : Serial.print("Triangle ");
             break;
    case 3 : Serial.print("Square ");
             break;
    default : Serial.print("<Unknown Shape> ");
              break;
  }
  Serial.print("Detected");
}

