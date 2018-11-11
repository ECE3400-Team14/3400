/* 
 * Functions for creating and updating stacks/queues
 * 
 */
/*Depth First Search

Professor's algorithm:
(in main)
n = state(init) (starting position)
frontier.append(n)
while(frontier not empty)
  n = pull state from frontier 
  append n to visited (and/or mark as explored)
  //NOGOAL//if n = goal, return solution 
  for all actions in n (where the robot can go from n, in order (f,l,r)*
    n’ = a(n) (evaluation of the action)*
      if n’ not visited (could be getter from maze data)
        append n’ to frontier


Wikipedia Algorithm:
1 procedure DFS-iterative(G,v):
2      let S be a stack
3      S.push(v)
4      while S is not empty
5          v = S.pop()
6          if v is not labeled as discovered:
7              label v as discovered
8              for all edges from v to w in G.adjacentEdges(v) do 
9                  S.push(w)

*/

//Stack-related-functions
#define maze_size 6//TODO: sync across files

struct Stack {
  byte* elements;//the elements of the stack
  byte top;//the pointer to the top of the stack
};

struct Stack frontier;
struct Stack visited;

//initialize a stack
void init_stack(struct Stack s) {
  s.elements = new byte[maze_size];
  s.top = 0;
}

//push an element to the stack
void push_stack(int v, struct Stack s) {
  if ( s.top < maze_size) {
    s.elements[s.top] = v;
    s.top++;
  }
}

//pop an element from a stack
int pop_stack(struct Stack s) {
  if ( s.top > 0) {
    s.top = s.top - 1;
    return s.elements[s.top];
  }
  else return 0;
}

//get current number of elements in stack
int stack_size(struct Stack s) {
  return s.top;
}

//return true if stack contains v
bool stack_contains(int v, struct Stack s) {
  if (s.top == 0) return false;
  for(int i = 0; i < s.top; i++) {
    if (s.elements[i] == v) return true;
  }
  return false;
}

///DFS helpers/////

//initialize state

void initialize_search() {
  init_stack(frontier);
  init_stack(visited);
}

//append to frontier
void append_frontier(int v) {
  push_stack(v, frontier);
}

//pop from frontier
int next_frontier() {
  return pop_stack(frontier);
}

//append to visited
void append_visited(int v) {
  push_stack(v, visited);
}

//has visiteds
bool has_visited(int v) {
  return stack_contains(v, visited);
}

bool frontier_empty() {
  return stack_size(frontier) == 0;
}


//////QUEUE//////////
struct Queue {
  byte* elements;//the elements of the stack
  byte first;//the pointer to the top of the stack
  byte last;
  byte size;
};

struct Queue bfs_frontier;
struct Queue bfs_visited;

void init_queue(struct Queue q) {
   q.elements = new byte[maze_size];
   q.first = 0;
   q.last = 0;
   q.size = 0;
}

void push_queue(int v, struct Queue q) {
  if(q.size < maze_size) {
    q.elements[q.last] = v;
    q.last = (q.last == maze_size - 1) ? 0 : q.last + 1;
    q.size = q.size + 1;
  }
}

int pop_queue(struct Queue q) {
  if(q.size > 0) {
    int first = q.elements[q.first];
    q.first = (q.first == maze_size - 1) ? 0 : q.first + 1;
    q.size = q.size - 1;
    return first;
  }
}

int queue_size(struct Queue q) {
  return q.size;
}

bool queue_contains(int v, struct Queue q) {
  for(int i = 0; i < q.size; i++) {
    if(q.elements[(q.first + i) % maze_size] == v) return true;
  }
  return false;
}

//BFS Helper Functions//

void initialize_bfs() {
  init_queue(bfs_frontier);
  init_queue(bfs_visited);
}

//append to frontier
void bfs_append_frontier(int v) {
  push_queue(v, bfs_frontier);
}

//pop from frontier
int bfs_next_frontier() {
  return pop_queue(bfs_frontier);
}

//append to visited
void bfs_append_visited(int v) {
  push_queue(v, bfs_visited);
}

//has visiteds
bool bfs_has_visited(int v) {
  return queue_contains(v, bfs_visited);
}

bool bfs_frontier_empty() {
  return queue_size(bfs_frontier) == 0;
}






