#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include "semaphore.h"

#include "./include/stack.h"

/// Consts
int const MAX_SIZE = 8;
unsigned int MAX_THREADS;
/// Global variables
Stack stack;
Stack stack_final;
sem_t sem_sentinel;
sem_t sem_consumer;
pthread_barrier_t thread_barrier;
unsigned int size;
unsigned int size2;
unsigned int size3;
unsigned int size4;

void print_sudoku(unsigned int sudoku[]) {
	for (unsigned int i = 0; i < size4; ++i) {
		if (i%size2 == 0) printf("\n");
		printf("%u ", sudoku[i]);
	}
	printf("\n\n");
	fflush(stdout);
}

bool valid_in_row(unsigned int* array, unsigned int x, int index){
	for(int i = 0; i < size2; i++){
		if(array[(index/size2)*size2 + i] == x)
			return false;
	}
	return true;
}

bool valid_in_col(unsigned int* array, unsigned int x, int index){
	for(int i = 0; i < size2; i++){
		if(array[index%size2 + i*size2] == x)
			return false;
	}
	return true;
}

bool valid_in_sq(unsigned int* array, unsigned int x, int index){
	for(int i = 0; i < size2; i++){
		if(array[((index/size3)*size3) + ((index%size2)/size)*size + i%size + (i/size)*size2] == x)
			return false;
	}
	return true;
}

void* solve_sudoku(void* args){
	//Declarations
	Data *data;
	unsigned int ind;
	unsigned int start_line;
	bool found;
	bool next;
	int sem_value;

	// Copy sudoku
	unsigned int *sudoku_copy;

	while (1) {
		sem_getvalue(&sem_sentinel, &sem_value);
		if (stack.size == 0 && sem_value == 0) {
			printf("Done with threads\n");
			fflush(stdout);
			pthread_barrier_wait(&thread_barrier);
			break;
		}
		// Semaphores
		sem_wait(&sem_consumer);
		sem_post(&sem_sentinel);

		/// Variables/Constraints
		data = stackpop(&stack);
		ind = data->start_ind;
		start_line = (ind/size2);
		found = true;
		next = true;

		// Copy sudoku
		sudoku_copy = malloc(sizeof(unsigned int)*size4);
		memcpy(sudoku_copy, data->state, sizeof(unsigned int)*size4); 

		/// Sudoku recursive loop
		while(ind/size2 >= start_line && ind/size2 <= start_line) {
			if (data->state[ind]==0) {
				// When an empty space is found, test up from the last number recorded on the copy
				found = false;
				for (unsigned int i = sudoku_copy[ind]+1; i < size2+1; ++i) {
					if (valid_in_sq(sudoku_copy, i, ind) &
						valid_in_col(sudoku_copy, i, ind) &
						valid_in_row(sudoku_copy, i, ind)) {
							found = true;
							sudoku_copy[ind] = i;
							break;
					}
				}
				next = found;
			}
			if(found){
				if (ind == size4-1) {
					// Stack the solutionon the solutions stack
					Data *aux_data = malloc(sizeof(Data));
					unsigned int *aux_sudoku = malloc(sizeof(unsigned int)*size4);
					memcpy(aux_sudoku, sudoku_copy, sizeof(unsigned int)*size4);
					aux_data->state = aux_sudoku;
					aux_data->start_ind = size4;
					stackpush(&stack_final, aux_data);
					next = false;
				} else if ((ind+1)/size2 > start_line) { // TODO: Memory constraints/ Stacking Criteria
					// Stack the partial sudoku on the stack
					printf("OH SHIT WHERE DID MY MEMORY GO. Stack Size: %u\n", stack.size);
					Data *aux_data = malloc(sizeof(Data));
					unsigned int *aux_sudoku = malloc(sizeof(unsigned int)*size4);
					memcpy(aux_sudoku, sudoku_copy, sizeof(unsigned int)*size4);
					aux_data->state = aux_sudoku;
					aux_data->start_ind = ind+1;
					stackpush(&stack, aux_data);
					next=false;
					sem_post(&sem_consumer);

				}
			}
			if (next) {
				++ind;
			} else {
				// Restore to 0 only if the space was empty on the original
				if (data->state[ind] == 0) sudoku_copy[ind] = 0;
				--ind;
			}
		}
		// Free memory
		free(data->state);
		free(sudoku_copy);
		sem_wait(&sem_sentinel);
	}
	return 0;
}


int main(int argc, char const *argv[])
{
	// Constraints
	assert(size <= MAX_SIZE);

	// Size
	assert(scanf("%d", &size) == 1);
	size2 = size*size;
	size3 = size2*size;
	size4 = size3*size;

	/// Inis
	sem_init(&sem_sentinel, 0, 0);
	sem_init(&sem_consumer, 0, 1);
	pthread_barrier_init(&thread_barrier, NULL, 2);
	stackinit(&stack);
	stackinit(&stack_final);
	unsigned int initial_sudoku[size4];
	MAX_THREADS = atoi(argv[1]);

	// Sudoku Scan
	for (unsigned int i = 0u; i<size4; ++i) {
		assert(scanf("%u",&initial_sudoku[i]) == 1);
	}

	// Create/Place Sudoku on Stack
	Data *data = malloc(sizeof(Data));
	data->start_ind = 0u;
	data->state = initial_sudoku;
	stackpush(&stack, data);
	
	// Create and Assign threads
	pthread_t threads[MAX_THREADS];
	for (unsigned int i = 0; i<MAX_THREADS; ++i){
		pthread_create(&threads[i], NULL, solve_sudoku, NULL);
	}
	printf("Main thread done\n");
	fflush(stdout);
	pthread_barrier_wait(&thread_barrier);
	/// DEBUG ///
	printf("Max_threads %i\n", MAX_THREADS);
	printf("Size %i\n", size);
	printf("Initial Sudoku:\n");
	print_sudoku(initial_sudoku);
	printf("One Line Solutions\n");
	unsigned int stacksize = stack_final.size;
	printf("%u Solutions\n", stacksize);
	for (unsigned int i = 0; i < stacksize; ++i) {
		print_sudoku(stackpop(&stack_final)->state);
	}

	/// END DEBUG ///
	printf("%s\n", "Done");
	return 0;
}