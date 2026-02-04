/*
 * trylock.c
 *
 * Deadlock prevention by using pthread_mutex_trylock() and 
 * relinquishing ownership if acquisition of the second resource
 * fails, basically employing a non-blocking resource acquisition
 * strategy that prevents circular wait.
 */

#include <stdlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <assert.h>
#include <errno.h>
#include <string.h>

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

#define N 5
#define MILLIS 500

pthread_mutex_t orders[N];
pthread_mutex_t bikes[N];

typedef struct ThreadArgs{
	int id;
	int restaurant;
} ThreadArgs;

void
sleep_ms(int ms){
	usleep(ms * 1000);
}

void*
veteran(void* ptr)
{
	ThreadArgs* args = (ThreadArgs*)ptr;
	int id = args->id;
	int r  = args->restaurant;

	printf("[Veteran %d] Trying bike %d...\n", id, r);
	pthread_mutex_lock(&bikes[r]);
	printf(C(CLR_CYAN) "[Veteran %d] Locked bike %d\n" C(CLR_RESET), id, r);

	sleep_ms((rand() % MILLIS) + MILLIS);

	printf("[Veteran %d] Trying order %d...\n", id, r);
	int s = pthread_mutex_trylock(&orders[r]);
	if(s == EBUSY){
		printf(C(CLR_ORANGE) "[Veteran %d] Both resources are locked, releasing bike %d...\n" C(CLR_RESET), id, r);
		pthread_mutex_unlock(&bikes[r]);
	}else if(s == 0){
		printf(C(CLR_CYAN) "[Veteran %d] Locked order %d. Delivering...\n" C(CLR_RESET), id, r);
		sleep_ms((rand() % MILLIS) + MILLIS);
		printf(C(CLR_GREEN) "[Veteran %d] Successfully delivered order %d. Freeing resources...\n" C(CLR_RESET), id, r);
		
		pthread_mutex_unlock(&bikes[r]);
		pthread_mutex_unlock(&orders[r]);
	}else{
		fprintf(stderr, "Error trying to lock mutex: %s\n", strerror(s));
	}

	return NULL;
}

void*
novice(void* ptr)
{
	ThreadArgs* args = (ThreadArgs*)ptr;
	int id = args->id;
	int r  = args->restaurant;

	printf("[Novice %d] Trying order %d...\n", id, r);
	pthread_mutex_lock(&orders[r]);
	printf(C(CLR_CYAN) "[Novice %d] Locked order %d\n" C(CLR_RESET), id, r);

	sleep_ms((rand() % MILLIS) + MILLIS);

	printf("[Novice %d] Trying bike %d...\n", id, r);
	int s = pthread_mutex_trylock(&bikes[r]);
	if(s == EBUSY){
		printf(C(CLR_ORANGE) "[Novice %d] Both resources are locked, releasing order %d...\n" C(CLR_RESET), id, r);
		pthread_mutex_unlock(&orders[r]);
	}else if(s == 0){
		printf(C(CLR_CYAN) "[Novice %d] Locked bike %d. Delivering...\n" C(CLR_RESET), id, r);
		sleep_ms((rand() % MILLIS) + MILLIS);
		printf(C(CLR_GREEN) "[Novice %d] Successfully delivered order %d. Freeing resources...\n" C(CLR_RESET), id, r);
	
		pthread_mutex_unlock(&orders[r]);
		pthread_mutex_unlock(&bikes[r]);
	}else{
		fprintf(stderr, "Error trying to lock mutex: %s\n", strerror(s));
	}

	return NULL;
}

int
main(int argc, char* argv[]){
	srand(time(NULL));

	printf("Initializing mutexes...");
	for(int i = 0; i < N; i++){
		MUT_INIT(&orders[i]);
		MUT_INIT(&bikes[i]);
	}

	pthread_t veterans[N];
	pthread_t novices[N];
	ThreadArgs vet_args[N];
	ThreadArgs nov_args[N];

	for(;;){
		system("clear");
		printf("================");
		printf("SIMULATION START");
		printf("================");
		
		printf("Creating threads...");
		for(int i = 0; i < N; i++){
			vet_args[i].id = i;
			vet_args[i].restaurant = rand() % N;
			nov_args[i].id = i;
			nov_args[i].restaurant = rand() % N;

			pthread_create(&veterans[i], NULL, veteran, &vet_args[i]);
			pthread_create(&novices[i], NULL, novice, &nov_args[i]);
		}

		for(int i = 0; i < N; i++){
			pthread_join(veterans[i], NULL);
			pthread_join(novices[i], NULL);
		}
		printf("==============\n");
		printf("SIMULATION END\n");
		printf("==============\n");
		sleep(5);
	}

	return EXIT_SUCCESS;
}
