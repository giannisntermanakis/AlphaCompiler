#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../headerfiles/stack.h"



void expandStack(struct Stack* stack){
    int* p = (int*) malloc((stack->TOTALSIZE + stack->SIZE)* sizeof(int));

    
    if(stack){
        p = realloc(stack , stack->TOTALSIZE+stack->SIZE);
    }
    stack->array = p;
    stack->TOTALSIZE += stack->SIZE;


}



struct Stack* createStack()
{
	struct Stack* stack = (struct Stack*)malloc(sizeof(struct Stack));
	stack->SIZE = 10;
  	stack->TOTALSIZE = 0 ;
	stack->top = -1;


   stack->array = (int*)malloc(stack->SIZE * sizeof(int));
   stack->TOTALSIZE += stack->SIZE ;
	return stack;
}

int isFull(struct Stack* stack)
{
	return stack->top == stack->SIZE - 1;
}

int isEmpty(struct Stack* stack)
{
	return stack->top == -1;
}

void push(struct Stack* stack, int item)
{
	if (isFull(stack)){
		expandStack(stack);
   }
   ++stack->top ;
	stack->array[stack->top] = item;
}

int pop(struct Stack* stack)
{
   int i;
	if (isEmpty(stack)){
		return -1;
   }

   i = stack->array[stack->top--] ;
	return i;
}

int top(struct Stack* stack)
{
	if (isEmpty(stack))
		return -1;
	return stack->array[stack->top];
}