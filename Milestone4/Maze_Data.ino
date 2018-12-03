/*
 * Functions for storing, setting, and getting maze data
 */

byte mazeData[mazeSize]; //index = x + rowLength*y

/*
 * N[0]
 * E[1]
 * S[2]
 * W[3]
 * color[4]
 * shape[5:6]
 * explored[7]
 */

//not necessary (DEPRICATED)
void initMaze()
{
  for (int xn = 0; xn < rowLength; xn++)
  {
    for (int yn = 0; yn < colLength; yn++)
    {
      mazeData[xn + rowLength * yn] = 0;
    }
  }
}

int getPosition(int x, int y) { return x + rowLength * y; }

int getX(int c) { return c % rowLength; }
int getY(int c) { return c / rowLength; }

//returns true if there is a known wall between (x1,y1) and (x2,y2)
//Requres (x1,y1) and (x2,y2) are one square apart
//bool has_wall(int x1, int y1, int x2, int y2) {
//  return (getNorthWall(x1,y1) | getSouthWall(x2,y2) )
//  | ( getEastWall(x1,y1) | getWestWall(x2,y2) )
//  | ( getSouthWall(x1,y1) | getNorthWall(x2,y2) )
//  | ( getWestWall(x1,y1) | getEastWall(x2,y2) );
//}

//need getter functions
int getNorthWall(int x, int y)
{
  return bitRead(mazeData[x + rowLength * y], 0);
}
int getEastWall(int x, int y)
{
  return bitRead(mazeData[x + rowLength * y], 1);
}
int getSouthWall(int x, int y)
{
  return bitRead(mazeData[x + rowLength * y], 2);
}
int getWestWall(int x, int y)
{
  return bitRead(mazeData[x + rowLength * y], 3);
}

int isExplored(int x, int y)
{
  return bitRead(mazeData[x + rowLength * y], 7);
}

int getShape(int x, int y)
{
  byte one = bitRead(mazeData[x + rowLength * y],5);
  byte two = bitRead(mazeData[x + rowLength * y],6) << 1;
  return one | two;
}

void setNorthWall(int x, int y, int valid)
{
  if (valid)
    bitSet(mazeData[x + rowLength * y], 0);
  else
    bitClear(mazeData[x + rowLength * y], 0);
}
void setEastWall(int x, int y, int valid)
{
  if (valid)
    bitSet(mazeData[x + rowLength * y], 1);
  else
    bitClear(mazeData[x + rowLength * y], 1);
}
void setSouthWall(int x, int y, int valid)
{
  if (valid)
    bitSet(mazeData[x + rowLength * y], 2);
  else
  {
    bitClear(mazeData[x + rowLength * y], 2);
  }
}
void setWestWall(int x, int y, int valid)
{
  if (valid)
    bitSet(mazeData[x + rowLength * y], 3);
  else
    bitClear(mazeData[x + rowLength * y], 3);
}
//0 is red, 1 is blue
void setTreasureColor(int x, int y, int color)
{
  if (color)
    bitSet(mazeData[x + rowLength * y], 4);
  else
    bitClear(mazeData[x + rowLength * y], 4);
}
// shape according to standard
void setTreasureShape(int x, int y, int shape)
{
  bitClear(mazeData[x + rowLength * y], 5);
  bitClear(mazeData[x + rowLength * y], 6);
  mazeData[x + rowLength * y] |= (shape << 5);
}

void setExplored(int x, int y, int valid)
{
  if (valid)
    bitSet(mazeData[x + rowLength * y], 7);
  else
    bitClear(mazeData[x + rowLength * y], 7);
}
