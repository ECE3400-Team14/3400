struct Node {
  byte position;//x,y coordinates of the node
  char move;//move used to get here
  byte orientation;//orientation of the robot at this point
  struct Node* parent;//the parent of this node
};

QueueArray<Node>* nodeQueue;

void initNodeQueue() {
  nodeQueue = new QueueArray<Node>;
}

//call when done with the tree (after generating path to next node)
void clearNodeQueue() {
 delete nodeQueue;
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
  initialize_bfs();//initialize queue to store coordinates visited by the algorithm
  struct Node start;
  start.position = start_pos;
  start.move = 's';
  start.parent = NULL;
  start.orientation = orientation;
  nodeQueue->push(start);
  
  bfs_append_visited(start_pos);
  while (!nodeQueue->isEmpty()) {
    struct Node v = nodeQueue->pop();
    int pos = v.position;
    //if this node hasn't been explored yet, return path to node
    if(!isExplored(getX(pos),getY(pos) ) ){
      constructMovements(&v);
      return;
    }
    //if the node is not in the frontier, keep looking
    else 
      if(!bfs_has_visited(pos)) {
        bfs_append_visited(pos);
      }
        //if forward is an option
        if(canGoForward(x,y,v.orientation)) {
          int next_orientation = v.orientation;
          int next_pos = nextCoor(getX(pos),getY(pos),next_orientation);
          if(!bfs_has_visited(next_pos)) {//if next coordinate is not visited
            //add Node with next coordinate and 'f'
            struct Node next_v;
            next_v.position = next_pos;//update node position
            next_v.move = 'f';
            next_v.orientation = next_orientation;
            next_v.parent = &v;
            nodeQueue->push(next_v);
          }
          
        }
        //if left is an option (turn left with not wall)
        if(canGoLeft(x,y,v.orientation)) {
          int next_orientation = (v.orientation == 0) ? 3 : v.orientation - 1;
          int next_pos = nextCoor(getX(pos),getY(pos),next_orientation);
          if(!bfs_has_visited(pos)) {//if next coordinate is not visited
            //add Node with this coordinate and 'l'
            struct Node next_v;
            next_v.position = pos;//keep the same position
            next_v.move = 'l';
            next_v.orientation = next_orientation;
            next_v.parent = &v;
            nodeQueue->push(next_v);
          }
        }

        //if right is an option (turn right with not wall)
        if(canGoRight(x,y,v.orientation)) {
          int next_orientation = (v.orientation == 3) ? 0 : v.orientation + 1;
          int next_pos = nextCoor(getX(pos),getY(pos),next_orientation);
          if(!bfs_has_visited(pos)) {//if next coordinate is not visited
            //add Node with this coordinate and 'r'
            struct Node next_v;
            next_v.position = pos;//keep the same position
            next_v.move = 'r';
            next_v.orientation = next_orientation;
            next_v.parent = &v;
            nodeQueue->push(next_v);
          }
        }

    
  }
  //do stuff to construct movement path
  
  //have found nothing new
  constructMovements(&start);
  return;
}

//add the moves from v to the stack
void constructMovements(struct Node* v) {
 movementStack = new StackArray<char>();
 while (v != NULL) {
    char next_move = v->move;
    movementStack->push(next_move);
    v = v->parent;
 }
  clearNodeQueue();//clearMemory
}






