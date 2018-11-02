#include <Wire.h>

#define OV7670_I2C_ADDRESS 0x21/*TODO: write this in hex (eg. 0xAB) */


///////// Main Program //////////////
void setup() {
  Wire.begin();
  Serial.begin(9600);
  Serial.println("The arduino is on");
  delay(100);
  // TODO: READ KEY REGISTERS
  read_key_registers();
  delay(100);
  // TODO: WRITE KEY REGISTERS
  OV7670_write_register(0x12, 0x80);//reset all registers COM7
  delay(100);
  OV7670_write_register(0x0C, 0x8);//enable scaling COM3
  delay(100);
  OV7670_write_register(0x11, 0x40);//use external clock
  delay(100);
  //OV7670_write_register(0x3E, 0x4);//enable manual scaling
  //OV7670_write_register(0x12, 0xE);//set camera pixel format and enable color bar test
  OV7670_write_register(0x12, 0xC);//set camera pixel format and disable color bar test
  delay(100);
  OV7670_write_register(0x14, 0x0);//automated gain ceiling of 2x
  delay(100);
  OV7670_write_register(0x40, 0xD0);//COM15 set for RGB 565 11010000 (208)
  delay(100);
  OV7670_write_register(0x70, 0x0);//enable color bar test xsc
  delay(100);
  OV7670_write_register(0x71, 0x80);//enable color bar test (second bit) ysc
  delay(100);
  //OV7670_write_register(0x42, 0x4);//COM17 enable DSP color bar
  delay(100);
  read_key_registers();
  delay(100);
  set_color_matrix();
  delay(500);
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