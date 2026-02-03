#include <bits/pthreadtypes.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <assert.h>

#define LOG(x)\
              \

#define MUT_INIT(x) assert(pthread_mutex_init(x, NULL) == 0)

#define N 10
#define SECONDS 5



pthread_mutex_t orders[N];
pthread_mutex_t bikes[N];

void*
veteran(void* ptr)
{
  pthread_mutex_lock(&bikes[i]);
  sleep(SECONDS);
  pthread_mutex_trylock(&orders[i])
}

void*
novice(void* ptr)
{
  pthread_mutex_lock(&orders[i]);
  sleep(SECONDS);
  pthread_mutex_trylock(&bikes[i]);
}

int
main(int argc, char* argv[]){
  pthread_t veterans;
  pthread_t novices;

  //int id_vet = 1;
  //int id_nov = 2;

  puts("Initializing mutexes and threads...");
  int rc = 0;
  for(int i = 0; i < N; i++){
    MUT_INIT(&orders[i]);
    MUT_INIT(&bikes[i]);

    //pthread_create(&veterans, NULL, veteran, &id_vet[i]);
    //pthread_create(&novices, NULL, novice, &id_nov[i]);

    pthread_create(&veterans, NULL, veteran, NULL);
    pthread_create(&novices, NULL, novice, NULL);
  }
  return EXIT_SUCCESS;
}
