/* TODO: For Dijkstra: 
 *  -add weights to nodes
 *  -allow parent swapping
 *    -try going to node
 *    -see that a node there aready exists
 *    -compare path lengths from current path and the one of the node already there
 *    -if new path is lower, replace other node's parent with this node. 
 * 
 */

struct Node {
  byte position;//x,y coordinates of the node
  char move;//move used to get here
  byte orientation;//orientation of the robot at this point
  struct Node* parent;//the parent of this node
};

QueueList<Node*>* nodeQueue;//array of node (addresses) (TODO: Make Priority Queue with LinkedList)

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
  start.move = 's';
  start.parent = NULL;
  start.orientation = orientation;
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
        if(canGoForward(getX(pos),getY(pos),v->orientation)) {
          int next_orientation = v->orientation;//orientation at destination
          int next_pos = nextCoor(getX(pos),getY(pos),next_orientation);//coordinate of destination
          if(!visitedContains(next_pos)) {//if next coordinate is not visited 
            //add Node with next coordinate and 'f'
            struct Node next_v;
            next_v.position = next_pos;//update node position
            next_v.move = 'f';
            next_v.orientation = next_orientation;
            next_v.parent = v;
            //put next_v in visited array
            addVisitedNode(next_v);//store this node in memory
            nodeQueue->push(getTopAddr());//add address of node to queue
          }
          
        }
        //if left is an option (turn left with not wall)
        if(canGoLeft(getX(pos),getY(pos),v->orientation)) {
          int next_orientation = (v->orientation == 0) ? 3 : v->orientation - 1;//orientation after turning left
          int next_pos = nextCoor(getX(pos),getY(pos),next_orientation);//coordinate of front destination
          if(!visitedContains(next_pos)) {//if next coordinate is not visited
            //add Node with this coordinate and 'l'
            struct Node next_v;
            next_v.position = pos;//keep the same position
            next_v.move = 'l';
            next_v.orientation = next_orientation;
            next_v.parent = v;
            //nodeQueue->push(next_v);
            addVisitedNode(next_v);//store this node in memory
            nodeQueue->push(getTopAddr());
          }
        }

        //if right is an option (turn right with not wall)
        if(canGoRight(getX(pos),getY(pos),v->orientation)) {
          int next_orientation = (v->orientation == 3) ? 0 : v->orientation + 1;
          int next_pos = nextCoor(getX(pos),getY(pos),next_orientation);
          if(!visitedContains(next_pos)) {//if next coordinate is not visited
            //add Node with this coordinate and 'r'
            struct Node next_v;
            next_v.position = pos;//keep the same position
            next_v.move = 'r';
            next_v.orientation = next_orientation;
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
    char next_move = v->move;
    Serial.print(next_move);//debug
    Serial.print(" to ");
    Serial.println(getX(v->position));
    Serial.print(",");
    Serial.println(getY(v->position));
    movementStack.push(next_move);
    v = v->parent;
 }
  clearNodeQueue();//clearMemory
}






