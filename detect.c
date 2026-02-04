/*
 * detect.c
 *
 * Deadlock detection and recovery strategy for the delivery system
 * Threads can cause deadlock but the system detects cycles and recovers
 * by aborting one participant
 */

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <assert.h>

#define MUT_INIT(x) assert(pthread_mutex_init(x, NULL) == 0)

// Color support toggle: colors may not work on Windows Powershell or cmd
// Comment the line below to toggle colors off
#define COLOR
#ifdef COLOR
	#define C(x) x
#else
	#define C(x) ""
#endif

#define CLR_RESET   "\033[0m"
#define CLR_RED     "\033[31m"
#define CLR_GREEN   "\033[32m"
#define CLR_ORANGE  "\033[33m"
#define CLR_BLUE    "\033[34m"
#define CLR_MAGENTA "\033[35m"
#define CLR_CYAN    "\033[36m"
#define CLR_GRAY    "\033[90m"

#define N 10
#define SECONDS 2

pthread_mutex_t state_lock;

typedef struct ThreadArgs{
	int id;
	int restaurant;
	int type; // 0 = vet, 1 = nov
} ThreadArgs;

int bike_owner[N];
int order_owner[N];

int waiting_bike[2*N];
int waiting_order[2*N];

void*
veteran(void* ptr)
{

}

void*
novice(void* ptr)
{
}

int
main(int argc, char* argv[]){

	for(int i = 0; i < N; i++){
		bike_owner[i] = -1;
		order_owner[i] = -1;
	}

	for(int i = 0; i < 2*N; i++){
		waiting_bike[i] = -1;
		waiting_order[i] = -1;
	}

	MUT_INIT(&state_lock);
  
  pthread_t veterans[N];
  pthread_t novices[N];
 
	ThreadArgs vet_args[N];
	ThreadArgs nov_args[N];

  puts("Creating threads...");
  for(int i = 0; i < N; i++){
		vet_args[i].id = i;
		vet_args[i].restaurant = rand() % N;
		vet_args[i].type = 0;

		nov_args[i].id = i;
		nov_args[i].restaurant = rand() % N;
		nov_args[i].type = 1;

    pthread_create(&veterans[i], NULL, veteran, &vet_args[i]);
    pthread_create(&novices[i], NULL, novice, &nov_args[i]);
  }


  for(int i = 0; i < N; i++){
    pthread_join(veterans[i], NULL);
    pthread_join(novices[i], NULL);
  }
  return EXIT_SUCCESS;
}
