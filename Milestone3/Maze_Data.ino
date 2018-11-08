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

//need getter functions

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


