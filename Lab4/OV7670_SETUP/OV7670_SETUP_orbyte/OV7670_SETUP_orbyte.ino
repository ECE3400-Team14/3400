#include <Wire.h>

#define OV7670_I2C_ADDRESS 0x21/*TODO: write this in hex (eg. 0xAB) */


///////// Main Program //////////////
void setup() {
  Wire.begin();
  Serial.begin(9600);
  Serial.println("The arduino is on");

  // TODO: READ KEY REGISTERS
  read_key_registers();
  // TODO: WRITE KEY REGISTERS
  OV7670_write_register(0x12, 0x80);//reset all registers COM7

  byte prev = read_register_value(0x0C);
  byte toAdd = 0b00001000; //0x8
  byte toWrite = prev|toAdd;
  OV7670_write_register(0x0C, toWrite);//enable scaling COM3

  prev = read_register_value(0x11);
  toAdd = 0b01000000; //0x40
  toWrite = prev|toAdd;
  OV7670_write_register(0x11, toWrite); //use external clock
  
  prev = read_register_value(0x12);
  toAdd = 0b00001110; //0xE
  toWrite = prev|toAdd;
  OV7670_write_register(0x12, toWrite);//set camera pixel format and enable color bar test

  prev = read_register_value(0x14);
  toAdd = 0b00000000; //0x0
  toWrite = prev|toAdd;
  OV7670_write_register(0x14, 0x0);//automated gain ceiling of 2x

  prev = read_register_value(0x40);
  toAdd = 0b11010000; //0xD0 (0x10 would be 00010000)
  toWrite = prev|toAdd;
  OV7670_write_register(0x40, toWrite);//COM15 set for RGB 565 11010000 (208)

  prev = read_register_value(0x1E);
  toAdd = 0b00000000; //0x0
  toWrite = prev|toAdd;
  OV7670_write_register(0x1E, toWrite); //normal image mirror and flip
  
  //OV7670_write_register(0x70, 0x80);//enable color bar test xsc

  //OV7670_write_register(0x71, 0x0);//enable color bar test (second bit) ysc

  prev = read_register_value(0x42);
  toAdd = 0b00001000; //0x8
  toWrite = prev|toAdd;
  OV7670_write_register(0x42, toWrite);//COM17 enable DSP color bar

  read_key_registers();
 
  set_color_matrix();
 
  Serial.println("The arduino has set the color matrix");
}

void loop(){
 }


///////// Function Definition //////////////
void read_key_registers(){
  /*TODO: DEFINE THIS FUNCTION*/
  byte com7 = read_register_value(0x12); //COM7 1110
  byte com3 = read_register_value(0x0C); //COM3 1100
  byte clkrc = read_register_value(0x11); //CLKRC 10001
  //byte com14 = read_register_value(0x3E); //COM14 111110
  byte mvfp = read_register_value(0x1E); //MVFP 11110
  byte com9 = read_register_value(0x14); //COM9 10100
  byte com15 = read_register_value(0x40); //COM15 11010000
  byte scaling_xsc = read_register_value(0x70); //enabling the color bar test
  byte scaling_ysc = read_register_value(0x71); //enabling the color bar test
  byte com17 = read_register_value(0x42); //com17
  delay(100);
  Serial.print(com7);
  Serial.print(", ");
  Serial.print(com3);
  Serial.print(", ");
  Serial.print(clkrc);
  Serial.print(", ");
//  Serial.print(com14);
//  Serial.print(", ");
  Serial.print(mvfp);
  Serial.print(", ");
  Serial.print(com9);
  Serial.print(", ");
  Serial.print(com15);
  Serial.print(", ");
  Serial.print(scaling_ysc);
  Serial.print(", ");
  Serial.print(scaling_xsc);
  Serial.print(", ");
  Serial.print(com17);
  Serial.println();
  delay(100);
}

byte read_register_value(int register_address){
  byte data = 0;
  Wire.beginTransmission(OV7670_I2C_ADDRESS);
  Wire.write(register_address);
  Wire.endTransmission();
  Wire.requestFrom(OV7670_I2C_ADDRESS,1);
  while(Wire.available()<1);
  data = Wire.read();
  return data;
}

String OV7670_write(int start, const byte *pData, int size){
    int n,error;
    Wire.beginTransmission(OV7670_I2C_ADDRESS);
    n = Wire.write(start);
    if(n != 1){
      return "I2C ERROR WRITING START ADDRESS";   
    }
    n = Wire.write(pData, size);
    if(n != size){
      return "I2C ERROR WRITING DATA";
    }
    error = Wire.endTransmission(true);
    if(error != 0){
      return String(error);
    }
    return "no errors :)";
 }

String OV7670_write_register(int reg_address, byte data){
  return OV7670_write(reg_address, &data, 1);
 }

void set_color_matrix(){
    OV7670_write_register(0x4f, 0x80);
    OV7670_write_register(0x50, 0x80);
    OV7670_write_register(0x51, 0x00);
    OV7670_write_register(0x52, 0x22);
    OV7670_write_register(0x53, 0x5e);
    OV7670_write_register(0x54, 0x80);
    OV7670_write_register(0x56, 0x40);
    OV7670_write_register(0x58, 0x9e);
    OV7670_write_register(0x59, 0x88);
    OV7670_write_register(0x5a, 0x88);
    OV7670_write_register(0x5b, 0x44);
    OV7670_write_register(0x5c, 0x67);
    OV7670_write_register(0x5d, 0x49);
    OV7670_write_register(0x5e, 0x0e);
    OV7670_write_register(0x69, 0x00);
    OV7670_write_register(0x6a, 0x40);
    OV7670_write_register(0x6b, 0x0a);
    OV7670_write_register(0x6c, 0x0a);
    OV7670_write_register(0x6d, 0x55);
    OV7670_write_register(0x6e, 0x11);
    OV7670_write_register(0x6f, 0x9f);
    OV7670_write_register(0xb0, 0x84);
}
