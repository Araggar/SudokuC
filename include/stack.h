#ifndef STACK_H
#define STACK_H 
#include <pthread.h>
#include <stdlib.h>

typedef struct StructData {
	unsigned int* state;
	unsigned int start_ind;
} Data;

typedef struct node_t
{
    Data * data;
    struct node_t * next;
} Node;

typedef struct stack_t
{
	Node * top;
	unsigned int size;
	pthread_mutex_t mutex;

} Stack;

void stackinit(Stack*);
void stackpush(Stack*, Data*);
Data * stackpop(Stack*);
void stackdispose(Stack*);

#endif
