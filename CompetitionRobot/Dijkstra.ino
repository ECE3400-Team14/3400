/* Dijkstra's Algorithm:
 *  Does dijkstra's algorithm (inspired by Lecture 17, Slide 14, ECE 3400 2018)
 *  Written by: David Burgstahler (dfb93)
*/

#define forward_dist 2 //distance weight of forward
#define turn_dist 1    //distance weight of turn

//Node data structure for storing search tree nodes
struct Node
{
  byte position;       //x,y coordinates of the node
  byte move_and_or;    //[XXXMMMOO]
  struct Node *parent; //the parent of this node
  byte dist;           //distance from start to [position]
};

/*
 * Set the movement of a node witn movement [m]
 * 'f': forward
 * 'l': left then forward
 * 'r': right then forward
 * 't': turn 180 degrees then forward
 */
void setMove(struct Node *v, char m)
{
  if (m == 'f')
  {
    bitSet(v->move_and_or, 2);
    bitClear(v->move_and_or, 3);
    bitClear(v->move_and_or, 4);
  }
  else if (m == 'l')
  {
    bitClear(v->move_and_or, 2);
    bitSet(v->move_and_or, 3);
    bitClear(v->move_and_or, 4);
  }
  else if (m == 'r')
  {
    bitSet(v->move_and_or, 2);
    bitSet(v->move_and_or, 3);
    bitClear(v->move_and_or, 4);
  }
  else if (m == 't')
  {
    bitClear(v->move_and_or, 2);
    bitClear(v->move_and_or, 3);
    bitSet(v->move_and_or, 4);
  }
  else
  { //'s'
    bitClear(v->move_and_or, 2);
    bitClear(v->move_and_or, 3);
    bitClear(v->move_and_or, 4);
  }
}

/*
 * Get the move stored in node [v]
 * 'f': forward
 * 'l': left then forward
 * 'r': right then forward
 * 't': turn 180 degrees then forward
 */
char getMove(struct Node *v)
{
  byte mov = (v->move_and_or >> 2) & 0b111;
  if (mov == 1)
    return 'f';
  else if (mov == 2)
    return 'l';
  else if (mov == 3)
    return 'r';
  else if (mov == 4)
    return 't';
  else
    return 's';
}

/*
 * Set the orientation of the robot at node [v] to [o]:
 * 0: North (-x)
 * 1: East (+y)
 * 2: South (+x)
 * 3: West (-y)
 */
void setOrientation(struct Node *v, byte o)
{
  v->move_and_or = (v->move_and_or & (0b11111100)) | o; //is this right?
}

/*
 * Get the orientation of the robot at node [v]:
 * 0: North (-x)
 * 1: East (+y)
 * 2: South (+x)
 * 3: West (-y)
 */
byte getOrientation(struct Node *v)
{
  return v->move_and_or & 0b11;
}

/*
   get the distance between adjacent nodes [p] to [v]
*/
byte getDist(Node *p, Node *v)
{
  byte calc_turn_dist = 0;                                         //measure of distance of turn
  byte rotation = (getOrientation(p) - getOrientation(v) + 4) % 4; //calcuate rotation
  if (rotation == 1 || rotation == 3)
    calc_turn_dist = turn_dist; //requires one turn
  else if (rotation == 2)
    calc_turn_dist = 2 * turn_dist;     //requires two turns
  return forward_dist + calc_turn_dist; //add up one forward and number of turns needed
}

LinkedList<Node *> *priorityQueue; //queue where elements are added based on priority (stores all elements as well in distance order?)

//comparator function for sorting items in the priority queue
int compare(Node *&a, Node *&b)
{
  if (a->dist < b->dist)
    return -1;
  else if (a->dist == b->dist)
    return 0;
  else
    return 1;
}

void initQueue()
{
  priorityQueue = new LinkedList<Node *>;
}

//call when done with the tree (after generating path to next node)
void clearQueue()
{
  delete priorityQueue;
}

//Allocating memory for storing visited nodes
const int v_max_size = (rowLength)*colLength;
struct Node visitedNodes[v_max_size]; //array for storing node data
byte v_index = 0;                     //index where to enter new nodes

/*
   Reset list of visited nodes by algorithm
*/
void initVisitedNodes()
{
  v_index = 0;
}

/*
   add a new node
*/
void addVisitedNode(struct Node v)
{
  if (v_index == v_max_size)
  {
    //FAIL!
  }
  else
  {
    visitedNodes[v_index] = v;
    v_index++;
  }
}

/*
   Get address of node just added
   Requires visitedNodes is not empty
*/
struct Node *getTopAddr()
{
  return &visitedNodes[v_index - 1];
}

/*
   check if a node with position [pos] is already in the visited list
*/
bool visitedContains(byte pos)
{
  for (int i = 0; i < v_index; i++)
  {
    if (visitedNodes[i].position == pos)
      return true;
  }
  return false;
}

/*
   If a node with [pos] exists in visitedNodes, return it's address, NULL otherwise
*/
Node *getNodeByPos(byte pos)
{
  for (int i = 0; i < v_index; i++)
  {
    if (visitedNodes[i].position == pos)
      return &visitedNodes[i];
  }
  return NULL;
}

/*
 * 
 * procedure dijkstra(G, v):
 *        let Q be a queue
 *        Q.push(v)
 *        while Q is not empty
 *           sort Q by shortest distance
 *           v = Q.pop()
 *            if v is a frontier node, construct the path to v
 *            otherwise (have not found path to frontier yet):
 *               for all possible moves w from v in the maze do
 *                  if w has not been visited yet
 *                    Q.push(w)
 *                  else
 *                    if the path from v to w is shorter than the known distance to w
 *                      update w with path from v
 *                      Q.push(w)
 * 
*/
void dijkstra(int start_pos)
{

  initQueue();        //initialize the queue
  initVisitedNodes(); //reset visited list
  struct Node start;
  start.position = start_pos;
  start.parent = NULL;
  setMove(&start, 's');
  setOrientation(&start, orientation);
  start.dist = 0;
  addVisitedNode(start);
  priorityQueue->add(getTopAddr()); //add to front of priority queue

  //search for the frontier
  while (priorityQueue->size() > 0)
  {
    priorityQueue->sort(compare);              //sort queue
    struct Node *v = priorityQueue->remove(0); //get the first element in queue (smallest distance)

    int pos = v->position;
    //if this node hasn't been explored yet, return path to node
    if (!isExplored(getX(pos), getY(pos)))
    {
      clearQueue(); //clear frontier memory
      constructMovements(v);
      return;
    }
    //if the node is not in the frontier, keep looking
    else
    {
      //if forward is an option
      if (canGoForward(getX(pos), getY(pos), getOrientation(v)))
      {
        int next_orientation = getOrientation(v);
        int next_pos = nextCoor(getX(pos), getY(pos), next_orientation); //coordinate of destinatio
        struct Node *check_v = getNodeByPos(next_pos);                   //see if node with next_pos already exists
        if (check_v == NULL)
        { //if next coordinate is not visited
          //add Node with next coordinate and 'f'
          struct Node next_v;
          next_v.position = next_pos; //update node position
          setMove(&next_v, 'f');
          setOrientation(&next_v, next_orientation);
          next_v.parent = v;
          next_v.dist = v->dist + getDist(v, &next_v); //calc distance
          //put next_v in visited array
          addVisitedNode(next_v);           //store this node in memory
          priorityQueue->add(getTopAddr()); //add new item to queue
        }
        else
        {                                               //next_pos has path already with check_v
          int new_dist = v->dist + getDist(v, check_v); //test new distance through v to check_v
          if (new_dist < check_v->dist)
          {                      //if the new distance is less
            check_v->parent = v; //give check_v new parent
            check_v->dist = new_dist;
            setMove(check_v, 'f');
            setOrientation(check_v, next_orientation);
            //since check_v is in the priority Queue (assumed), its position will change later
          }
        }
      }
      if (!start_dir) //left > right
      {
        //if left is an option (turn left with not wall)
        if (canGoLeft(getX(pos), getY(pos), getOrientation(v)))
        {
          if (debug)
            Serial.println("CanGoLeft");
          int next_orientation = (getOrientation(v) == 0) ? 3 : getOrientation(v) - 1; //orientation after turning left
          int next_pos = nextCoor(getX(pos), getY(pos), next_orientation);             //coordinate of front destination
          struct Node *check_v = getNodeByPos(next_pos);
          if (check_v == NULL)
          { //if next coordinate is not visited
            //add Node with this coordinate and 'l'
            struct Node next_v;
            next_v.position = next_pos; //the new position
            setMove(&next_v, 'l');      //left to get to next position
            //next_v.orientation = next_orientation;
            setOrientation(&next_v, next_orientation);
            next_v.parent = v;
            next_v.dist = v->dist + getDist(v, &next_v); //calc distance
            addVisitedNode(next_v);                      //store this node in memory
            priorityQueue->add(getTopAddr());            //add new item to queue
          }
          else
          {                                               //next_pos has path already with check_v
            int new_dist = v->dist + getDist(v, check_v); //test new distance through v to check_v
            if (new_dist < check_v->dist)
            {                      //if the new distance is less
              check_v->parent = v; //give check_v new parent
              check_v->dist = new_dist;
              setMove(check_v, 'l');
              setOrientation(check_v, next_orientation);
              //since check_v is in the priority Queue (assumed), its position will change later
            }
          }
        }

        //if right is an option (turn right with not wall)
        if (canGoRight(getX(pos), getY(pos), getOrientation(v)))
        {
          int next_orientation = (getOrientation(v) == 3) ? 0 : getOrientation(v) + 1;
          int next_pos = nextCoor(getX(pos), getY(pos), next_orientation);
          struct Node *check_v = getNodeByPos(next_pos);
          if (check_v == NULL)
          { //if next coordinate is not visited
            //add Node with this coordinate and 'r'
            if (debug)
              Serial.println("new node");
            struct Node next_v;
            next_v.position = next_pos; //new position
            setMove(&next_v, 'r');
            setOrientation(&next_v, next_orientation);
            next_v.parent = v;
            next_v.dist = v->dist + getDist(v, &next_v); //calc distance
            addVisitedNode(next_v);                      //store this node in memory
            priorityQueue->add(getTopAddr());            //add new item to queue
          }
          else
          {                                               //next_pos has path already with check_v
            int new_dist = v->dist + getDist(v, check_v); //test new distance through v to check_v
            if (new_dist < check_v->dist)
            {                      //if the new distance is less
              check_v->parent = v; //give check_v new parent
              check_v->dist = new_dist;
              setMove(check_v, 'r');
              setOrientation(check_v, next_orientation);
              //since check_v is in the priority Queue (assumed), its position will change later
            }
          }
        }
      }
      else
      { //right > left
        if (canGoRight(getX(pos), getY(pos), getOrientation(v)))
        {
          int next_orientation = (getOrientation(v) == 3) ? 0 : getOrientation(v) + 1;
          int next_pos = nextCoor(getX(pos), getY(pos), next_orientation);
          struct Node *check_v = getNodeByPos(next_pos);
          if (check_v == NULL)
          { //if next coordinate is not visited
            //add Node with this coordinate and 'r'
            if (debug)
              Serial.println("new node");
            struct Node next_v;
            next_v.position = next_pos; //new position
            setMove(&next_v, 'r');
            setOrientation(&next_v, next_orientation);
            next_v.parent = v;
            next_v.dist = v->dist + getDist(v, &next_v); //calc distance
            addVisitedNode(next_v);                      //store this node in memory
            priorityQueue->add(getTopAddr());            //add new item to queue
          }
          else
          {                                               //next_pos has path already with check_v
            int new_dist = v->dist + getDist(v, check_v); //test new distance through v to check_v
            if (new_dist < check_v->dist)
            {                      //if the new distance is less
              check_v->parent = v; //give check_v new parent
              check_v->dist = new_dist;
              setMove(check_v, 'r');
              setOrientation(check_v, next_orientation);
              //since check_v is in the priority Queue (assumed), its position will change later
            }
          }
        }
        if (canGoLeft(getX(pos), getY(pos), getOrientation(v)))
        {
          if (debug)
            Serial.println("CanGoLeft");
          int next_orientation = (getOrientation(v) == 0) ? 3 : getOrientation(v) - 1; //orientation after turning left
          int next_pos = nextCoor(getX(pos), getY(pos), next_orientation);             //coordinate of front destination
          struct Node *check_v = getNodeByPos(next_pos);
          if (check_v == NULL)
          { //if next coordinate is not visited
            //add Node with this coordinate and 'l'
            struct Node next_v;
            next_v.position = next_pos; //the new position
            setMove(&next_v, 'l');      //left to get to next position]
            setOrientation(&next_v, next_orientation);
            next_v.parent = v;
            next_v.dist = v->dist + getDist(v, &next_v); //calc distance
            addVisitedNode(next_v);                      //store this node in memory
            priorityQueue->add(getTopAddr());            //add new item to queue
          }
          else
          {                                               //next_pos has path already with check_v
            int new_dist = v->dist + getDist(v, check_v); //test new distance through v to check_v
            if (new_dist < check_v->dist)
            {                      //if the new distance is less
              check_v->parent = v; //give check_v new parent
              check_v->dist = new_dist;
              setMove(check_v, 'l');
              setOrientation(check_v, next_orientation);
              //since check_v is in the priority Queue (assumed), its position will change later
            }
          }
        }
      }
      //backwards
      if (canGoBackwards(getX(pos), getY(pos), getOrientation(v)))
      {
        int next_orientation_r = (getOrientation(v) == 3) ? 0 : getOrientation(v) + 1;
        int next_orientation = (next_orientation_r == 3) ? 0 : next_orientation_r + 1;
        int next_pos = nextCoor(getX(pos), getY(pos), next_orientation);
        struct Node *check_v = getNodeByPos(next_pos);
        if (check_v == NULL)
        { //if next coordinate is not visited
          //add Node with this coordinate and 'r'
          struct Node next_v;
          next_v.position = next_pos; //new position
          setMove(&next_v, 't');      //turn around
          setOrientation(&next_v, next_orientation);
          next_v.parent = v;
          next_v.dist = v->dist + getDist(v, &next_v); //calc distance
          addVisitedNode(next_v);                      //store this node in memory
          priorityQueue->add(getTopAddr());
        }
        else
        {                                               //next_pos has path already with check_v
          int new_dist = v->dist + getDist(v, check_v); //test new distance through v to check_v
          if (new_dist < check_v->dist)
          {                      //if the new distance is less
            check_v->parent = v; //give check_v new parent
            check_v->dist = new_dist;
            setMove(check_v, 't');
            setOrientation(check_v, next_orientation);
            //since check_v is in the priority Queue (assumed), its position will change later
          }
        }
      }
    }
  }
  //have found nothing new
  if (!robot_detected)
    finished_search = true;
  clearQueue();               //clear frontier memory
  constructMovements(&start); //is this necessary?
  return;
}

//add the moves from v to the stack
void constructMovements(struct Node *v)
{
  while (v != NULL)
  {
    char next_move = getMove(v);
    movementStack.push(next_move);
    v = v->parent;
  }
}
