byte shape_data;

#define wait_time 100
#define switch_length 2
#define sig 2//signal pin 2
#define data A3//data pin A3

void setupComm() {
  pinMode(sig, OUTPUT);
  pinMode(data, INPUT);
  digitalWrite(sig, HIGH);
}

void readShape() {
  shape_data = 0;//reset shape data
  delay(wait_time);
  //check data line
  if(readData()) {
    //shape detected
    //fill data [SSC]
    for(int i = 0; i < 3; i++) {
      switchSig();
      bitWrite(shape_data, i,readData());
    }
    switchSig();//reset
    analyzeShape();
  }
  else {
    Serial.print("No Shape Detected");
  }
  Serial.println();
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
}

void analyzeShape() {
  byte color = shape_data & 1;
  byte shape = (shape_data >> 1) & 0b11;
  if(color) Serial.print("Blue ");
  else Serial.print("Red ");

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

