/*
 * Functions for storing, setting, and getting maze data
 */

unsigned int mazeData[6];//index = x + rowLength*y, 0,0 bottom left, 2*3 for now
const int rowLength = 2;
const int colLength = 3;

/*
 * x[3:0]
 * y[7:4]
 * N[8]
 * E[9]
 * S[10]
 * W[11]
 * color[12]
 * shape[14:13]
 * explored[15]
 */
void initMaze(){
  for (int xn = 0; xn < rowLength; xn++){
    for (int yn = 0; yn < colLength; yn++){
      int coordinates = 0;
      coordinates = coordinates | (xn | (yn << 4));
      mazeData[xn+rowLength*yn] = coordinates;
      }
    }
  }

int getPosition(int x, int y) { return x+rowLength*y;}

int getX(int c) {return c%rowLength;}
int getY(int c) {return c/rowLength;}

//returns true if there is a known wall between (x1,y1) and (x2,y2)
//Requres (x1,y1) and (x2,y2) are one square apart
//bool has_wall(int x1, int y1, int x2, int y2) {
//  return (getNorthWall(x1,y1) | getSouthWall(x2,y2) )
//  | ( getEastWall(x1,y1) | getWestWall(x2,y2) )
//  | ( getSouthWall(x1,y1) | getNorthWall(x2,y2) )
//  | ( getWestWall(x1,y1) | getEastWall(x2,y2) );
//}

//need getter functions
int getNorthWall(int x, int y){
  return (mazeData[x+rowLength*y] &= 1<<8)>>8;
  }
int getEastWall(int x, int y){
  return (mazeData[x+rowLength*y] &= 1<<9)>>9;
  }
int getSouthWall(int x, int y){
  return (mazeData[x+rowLength*y] &= 1<<10)>>10;
  }
int getWestWall(int x, int y){
  return (mazeData[x+rowLength*y] &= 1<<11)>>11;
  }

int isExplored(int x, int y) {
  return bitRead(mazeData[x+rowLength*y], 15);
}

void setNorthWall(int x, int y, int valid){
  if(valid) bitSet(mazeData[x+rowLength*y], 8);
  else bitClear(mazeData[x+rowLength*y], 8);
  }
void setEastWall(int x, int y, int valid){
  if(valid) bitSet(mazeData[x+rowLength*y], 9);
  else bitClear(mazeData[x+rowLength*y], 9);
  
  }
void setSouthWall(int x, int y, int valid){
  if(valid) bitSet(mazeData[x+rowLength*y], 10);
  else {bitClear(mazeData[x+rowLength*y], 10);}
  
  }
void setWestWall(int x, int y, int valid){
  if(valid) bitSet(mazeData[x+rowLength*y], 11);
  else bitClear(mazeData[x+rowLength*y], 11);
  
  }
  //0 is red, 1 is blue
void setTreasureColor(int x, int y, int color){
  if(color) bitSet(mazeData[x+rowLength*y], 12);
  else bitClear(mazeData[x+rowLength*y], 12);
  }
  // shape according to standard
void setTreasureShape(int x, int y, int shape){
  mazeData[x+rowLength*y] |= shape << 13;
  }

void setExplored(int x, int y, int valid) {
  if(valid) bitSet(mazeData[x+rowLength*y], 15);
  else bitClear(mazeData[x+rowLength*y], 15);
}


