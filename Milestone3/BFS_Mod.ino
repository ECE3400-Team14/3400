/* TODO: For Dijkstra: 
 *  -add weights to nodes
 *  -allow parent swapping
 *    -try going to node
 *    -see that a node there aready exists
 *    -compare path lengths from current path and the one of the node already there
 *    -if new path is lower, replace other node's parent with this node and update distance
 *  -User priority node queue to order "frontier" by path length
 * 
 */

#define forward_dist 1//distance weight of forward
#define turn_dist 2//distance weight of turn

//TODO: Condense data structure memory with byte sharing
struct Node {
  byte position;//x,y coordinates of the node
  //unsigned char move;//move used to get here
  //byte orientation;//orientation of the robot at this point
  byte move_and_or; //[XXXXMMOO]
  struct Node* parent;//the parent of this node
  //int dist;
  //byte dist;//distance to this node
};


void setMove(struct Node* v, char m) {
  if (m == 'f') {
    bitSet(v->move_and_or,2);
    bitClear(v->move_and_or,3);
  }
   else if (m == 'l') {
    bitClear(v->move_and_or,2);
    bitSet(v->move_and_or,3);
  }
  else if (m == 'r') {
    bitSet(v->move_and_or,2);
    bitSet(v->move_and_or,3);
  }
  else {//'s'
    bitClear(v->move_and_or,2);
    bitClear(v->move_and_or,3);
  }
}

char getMove(struct Node* v) {
  byte mov = (v->move_and_or >> 2) & 0b11;
  if (mov == 1) return 'f';
  else if (mov == 2) return 'l';
  else if (mov == 3) return 'r';
  else return 's';
}

void setOrientation(struct Node* v, byte o) {
  v->move_and_or = (v->move_and_or & (0b11111100)) | o;//is this right?
}

byte getOrientation(struct Node* v) {
  return v->move_and_or & 0b11;
}

/*
 * Returns the node representing move between [v] and neighboring position [pos]
 */
//Node getMoveNode(Node* v, byte pos) {
// byte start_o = getOrientation(p);
// byte end_o = 0;//init
// if(getX(v->position) - getX(pos) == 1) {
//  end_o = 0;//go north
// }
// else if(getY(v->position) - getY(pos) == -1) {
//  end_o = 1;//go east
// }
// else if(getX(v->position) - getX(pos) == -1) {
//  end_o = 2;//go south
// }
// else if(getY(v->position) - getY(pos) == 1) {
//  end_o = 3;//go west
// }
// //TODO (what info is necessary? Just Distance?
//  
//}

/*
 * get the distance between adjacent nodes [p] to [v]
 */
byte getDist(Node* p,Node* v) {
 byte calc_turn_dist = 0;//measure of distance of turn
 byte rotation = (getOrientation(p) - getOrientation(v) + 4) % 4;//calcuate rotation
 if (rotation == 1 || rotation == 3) calc_turn_dist = turn_dist;//requires one turn
 else if (rotation == 2) calc_turn_dist = 2*turn_dist;//requires two turns
 return forward_dist + calc_turn_dist;//add up one forward and number of turns needed
}

QueueList<Node*>* nodeQueue;//array of node (addresses) (TODO: Make Priority Queue with LinkedList)

LinkedList<Node*>* priorityQueue;//queue where elements are added based on priority (stores all elements as well in distance order?)

//NEW: Push node based on distance to it. also updates visitedNodes?
//void pushPriority(struct Node &v) {
//  for (int i = 0; i < priorityQueue->size; i++) {
//    struct Node v_i* = priorityQueue->get(i);
//    //if distance to new node is less, add it in this place in the queue
//    if(v->dist < v_i-> dist) {
//      priorityQueue.set(i,v);
//      break;//exit
//    }
//  }
//}

void initNodeQueue() {
  nodeQueue = new QueueList<Node*>;
//  nodeQueue.setPrinter(Serial);
}

//call when done with the tree (after generating path to next node)
void clearNodeQueue() {
 delete nodeQueue;
}

const int v_max_size = (rowLength+1)*colLength;//(TODO: Calculate theoretical max size)
struct Node visitedNodes[v_max_size];//array for storing node data 
byte v_index = 0;//index where to enter new nodes

/* 
 * Reset list of visited nodes by algorithm
 */
void initVisitedNodes() {
  v_index = 0;
}

/*
 * add a new node 
 */
void addVisitedNode(struct Node v) {
  if(v_index == v_max_size) {
    //???? FAIL!
  }
  else {
    visitedNodes[v_index] = v;
    v_index++;
  }
}

/*
 * Get address of node just added 
 * Requires visitedNodes is not empty
 */
struct Node* getTopAddr() {
  return &visitedNodes[v_index-1];
}

/*
 * check if a node with position [pos] is already in the visited list
 */
bool visitedContains(byte pos) {
  for(int i = 0; i < v_index; i++) {
    if (visitedNodes[i].position == pos) return true;
  }
  return false;
}

/*
 * If a node with [pos] exists in visitedNodes, return it's address. Else NULL (TODO: use instead of visitedContains)
 */
Node* getNodeByPos(byte pos) {
  for(int i = 0; i < v_index; i++) {
    if (visitedNodes[i].position == pos) return &visitedNodes[i];
  }
  return NULL;
}


//struct Queue graph_visited;//a queue of visited coordinates in nodeQueue;

//find the closest frontier node to start_pos

//todo: change to return stack of nodes?
//  procedure bfs-iterative(G,v):
//       let Q be a queue
//       Q.push(v)
//       while Q is not empty
//          v = Q.pop()
//           if v is a frontier node, construct the path to v
//           otherwise (have not found path to frontier yet):
//              if v is not labeled as discovered:
//                label v as discovered
//                  for all possible moves from v in the maze do 
//                    Q.push(w)
void bfs_mod(int start_pos) {

  initNodeQueue();//initialize the queue (memory leaks?)
  //initialize_bfs();//initialize queue to store coordinates visited by the algorithm
  initVisitedNodes();//reset visited list
  struct Node start;
  start.position = start_pos;
  start.parent = NULL;
//  start.move = 's';
//  start.orientation = orientation;
  setMove(&start, 's');
  setOrientation(&start, orientation);
  addVisitedNode(start);
  nodeQueue->push(getTopAddr());

  //search for the frontier
  while (!nodeQueue->isEmpty()) {
    struct Node* v = nodeQueue->pop();//get next node to look at
    int pos = v->position;
    //if this node hasn't been explored yet, return path to node
    if(!isExplored(getX(pos),getY(pos) ) ){
      constructMovements(v);
      return;
    }
    //if the node is not in the frontier, keep looking
    else 
        //if forward is an option
        if(canGoForward(getX(pos),getY(pos),getOrientation(v))) {
          Serial.println("CanGoForward");
          //int next_orientation = v->orientation;//orientation at destination
          int next_orientation = getOrientation(v);
          int next_pos = nextCoor(getX(pos),getY(pos),next_orientation);//coordinate of destination
          if(!visitedContains(next_pos)) {//if next coordinate is not visited 
            //add Node with next coordinate and 'f'
            struct Node next_v;
            next_v.position = next_pos;//update node position
            //next_v.move = 'f';
            setMove(&next_v, 'f');
            //next_v.orientation = next_orientation;
            setOrientation(&next_v,next_orientation);
            next_v.parent = v;
            //put next_v in visited array
            addVisitedNode(next_v);//store this node in memory
            nodeQueue->push(getTopAddr());//add address of node to queue
          }
          //else(use getNodeByPos(next_pos) != NULL)
          //next_v = getNodeByPos(next_pos)
          //new_dist = v.dist + getDist(v,next_v)
          //if new_dist < next_v.dist
          //  next_v.parent = v
          //  next_v.dist = new_dist
          //  set move and orientation to new values
          //  resort priority queue
          
        }
        //if left is an option (turn left with not wall)
        if(canGoLeft(getX(pos),getY(pos),getOrientation(v))) {
          Serial.println("CanGoLeft");
          //int next_orientation = (v->orientation == 0) ? 3 : v->orientation - 1;//orientation after turning left
          int next_orientation = (getOrientation(v) == 0) ? 3 : getOrientation(v) - 1;//orientation after turning left
          int next_pos = nextCoor(getX(pos),getY(pos),next_orientation);//coordinate of front destination
          if(!visitedContains(next_pos)) {//if next coordinate is not visited
            //add Node with this coordinate and 'l'
            struct Node next_v;
            next_v.position = pos;//keep the same position
            //next_v.move = 'l';
            setMove(&next_v, 'l');
            //next_v.orientation = next_orientation;
            setOrientation(&next_v, next_orientation);
            next_v.parent = v;
            //nodeQueue->push(next_v);
            addVisitedNode(next_v);//store this node in memory
            nodeQueue->push(getTopAddr());
          }
        }

        //if right is an option (turn right with not wall)
        if(canGoRight(getX(pos),getY(pos),getOrientation(v)) ) {
          Serial.println("CanGoRight");
          //int next_orientation = (v->orientation == 3) ? 0 : v->orientation + 1;
          int next_orientation = (getOrientation(v) == 3) ? 0 :getOrientation(v) + 1;
          int next_pos = nextCoor(getX(pos),getY(pos),next_orientation);
          if(!visitedContains(next_pos)) {//if next coordinate is not visited
            //add Node with this coordinate and 'r'
            Serial.println("new node");
            struct Node next_v;
            next_v.position = pos;//keep the same position
            //next_v.move = 'r';
            setMove(&next_v, 'r');
            //next_v.orientation = next_orientation;
            setOrientation(&next_v,next_orientation);
            next_v.parent = v;
            //nodeQueue->push(next_v);
            addVisitedNode(next_v);//store this node in memory
            nodeQueue->push(getTopAddr());
          }
        }
        //backwards
        if(canGoBackwards(getX(pos),getY(pos),getOrientation(v)) && !canGoLeft(getX(pos),getY(pos),getOrientation(v)) && !canGoRight(getX(pos),getY(pos),getOrientation(v)) ) {
          Serial.println("CanGoBackwards");
          //int next_orientation = (v->orientation == 3) ? 0 : v->orientation + 1;
          int next_orientation_r = (getOrientation(v) == 3) ? 0 : getOrientation(v) + 1;
          int next_orientation = (next_orientation_r == 3) ? 0 : next_orientation_r + 1;
          int next_pos = nextCoor(getX(pos),getY(pos),next_orientation);
          Serial.println("Data:");
          Serial.println(next_orientation);
          Serial.println(next_pos);
          Serial.println(!visitedContains(next_pos));
          if(!visitedContains(next_pos)) {//if next coordinate is not visited
            //add Node with this coordinate and 'r'
            struct Node next_v;
            next_v.position = pos;//keep the same position
            //next_v.move = 'r';//made 'r' from 't' to preserve move priority heirarchy
            setMove(&next_v, 'r');
            //next_v.orientation = next_orientation;
            setOrientation(&next_v,next_orientation_r);
            next_v.parent = v;
            //nodeQueue->push(next_v);
            addVisitedNode(next_v);//store this node in memory
            nodeQueue->push(getTopAddr());
          }
        }
        

    
  }
  //do stuff to construct movement path
  
  //have found nothing new
  constructMovements(&start);//is this necessary?
  return;
}

//add the moves from v to the stack
void constructMovements(struct Node* v) {
 while (v != NULL) {
    //char next_move = v->move;
    char next_move = getMove(v);
    Serial.print(next_move);//debug
    Serial.print(" to ");
    Serial.print(getX(v->position));
    Serial.print(",");
    Serial.println(getY(v->position));
    movementStack.push(next_move);
    v = v->parent;
 }
  clearNodeQueue();//clearMemory
}






