#include "stack.h"

void stackinit(Stack * stack){
        stack->top = NULL;
		stack->size = 0;
		pthread_mutex_init(&stack->mutex, NULL);
}

void stackpush(Stack * stack, Data * data){	
		pthread_mutex_lock(&stack->mutex);
		Node* temp = malloc(sizeof(Node));
		temp->data = data;
		if(stack->size == 0){
			temp->next = NULL;
			stack->top = temp;
			stack->size += 1;
			pthread_mutex_unlock(&stack->mutex);
			return;
		}
		temp->next = stack->top;
        stack->top = temp;
		stack->size += 1;
		pthread_mutex_unlock(&stack->mutex);
}

Data * stackpop(Stack * stack){
	pthread_mutex_lock(&stack->mutex);
	if(!stack->size)
	{
		pthread_mutex_unlock(&stack->mutex);
		return NULL;
	}
	Node * temp;
	Data * data = stack->top->data;
	temp = stack->top;
	stack->top = stack->top->next;
	free(temp);
	stack->size += -1;
	pthread_mutex_unlock(&stack->mutex);
	return data;
}

void stackdispose(Stack * stack){
	pthread_mutex_lock(&stack->mutex);
    while(stack->size) {
       	stackpop(stack);
    }
	pthread_mutex_unlock(&stack->mutex);
}




