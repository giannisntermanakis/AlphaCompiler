
/*void expand_stack(void) ;

int isempty() ;
   
int isfull() ;

int pop() ;

int push(int data) ;
*/
struct Stack {
	int top;
	int SIZE;
  	int TOTALSIZE;
	int* array;
};

void expandStack(struct Stack* stack);


// function to create a stack of given SIZE. It initializes size of
// stack as 0
struct Stack* createStack();

// Stack is full when top is equal to the last index
int isFull(struct Stack* stack);

// Stack is empty when top is equal to -1
int isEmpty(struct Stack* stack);

// Function to add an item to stack. It increases top by 1
void push(struct Stack* stack, int item);

// Function to remove an item from stack. It decreases top by 1
int pop(struct Stack* stack);

// Function to return the top from stack without removing it
int top(struct Stack* stack);