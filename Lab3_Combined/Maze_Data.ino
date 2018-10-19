unsigned int mazeData[6];//index = x + rowLength*y, 0,0 bottom left, 2*3 for now
const int rowLength = 2;
const int colLength = 3;

void initMaze(){
  for (int x = 0; x < colLength; x++){
    for (int y = 0; y < rowLength; y++){
      int coordinates = x + y << 4;
      mazeData[x+rowLength*y] = coordinates;
      }
    }
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
  else bitClear(mazeData[x+rowLength*y], 10);
  
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
