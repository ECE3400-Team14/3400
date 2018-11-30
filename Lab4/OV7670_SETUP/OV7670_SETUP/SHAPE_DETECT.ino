byte shape_data;

const bool print_data = false;

#define wait_time 500
#define switch_length 2
#define sig 2//signal pin 2
#define data A3//data pin A3
#define max_val_count 64//maximum number of validation tries before exiting

void setupComm() {
  pinMode(sig, OUTPUT);
  pinMode(data, INPUT);
  digitalWrite(sig, LOW);
}

void readShape() {
  shape_data = 0;//reset shape data

//  switchSig();
//  int data = readData();
//  Serial.print(data);
  
  if(validate()) {
    int shape1 = get_shape();
    if(!data_break()) return readShape();
    int shape2 = get_shape();
    if(!data_break()) return readShape();
    if(shape1 == shape2) {
      shape_data = shape1;
      analyzeShape();
    }
    else {
      return readShape();
    }
  }
  else {
    Serial.print("Validation Failed");
  }
  Serial.println();
  //switchSig();//reset
  
  //delay(switch_length);
  
}

int data_break() {
  if(print_data) Serial.print("Break");
  switchSig();
  int data = readData();
  if(print_data) Serial.print(data);
  if(data == 1) return 1;
  else return 0;
}

int get_shape() {
  if(print_data) Serial.print("GS");
  int shape_data = 0;
  for(int i = 3; i >= 0; i--) {
     switchSig();
     int data = readData();
     if(print_data) Serial.print(data);
     bitWrite(shape_data, i,data);
  }
  return shape_data;
}

bool validate() {
  int try_count = 0;
  bool valid = false;
  int c = 0;
  if(print_data) Serial.print("Validating");
  while (!valid && try_count < max_val_count) {
    switchSig();
    int data = readData();
    if(print_data) Serial.print(data);
    if(c < 5 && data == 1) c = 0;
    else if ( (c < 5 && data == 0) || (c == 5 && data == 1)) c++;
    if(c == 6) {
      valid = true;
      return true;
    }
    try_count++;
  }
  return false;
}


byte readData() {
  return digitalRead(data);
}

void switchSig() {
  digitalWrite(sig, HIGH);
  delay(switch_length);
  digitalWrite(sig, LOW);
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

