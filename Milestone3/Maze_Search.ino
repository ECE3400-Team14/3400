
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
  int* elements;//the elements of the stack
  int top;//the pointer to the top of the stack
};

struct Stack frontier;
struct Stack visited;

//initialize a stack
void init_stack(struct Stack s) {
  s.elements = new int[maze_size];
  s.top = 0;
}

//push an element to the stack
void push_stack(int v, struct Stack s) {
  if ( s.top < maze_size - 1) {
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
bool stack_contains(int v, Stack s) {
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
bool has_visted(int v) {
  return stack_contains(v, visited);
}








