#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <assert.h>

#define MUT_INIT(x) assert(pthread_mutex_init(x, NULL) == 0)

#define N 1
#define SECONDS 2

pthread_mutex_t orders[N];
pthread_mutex_t bikes[N];

void*
veteran(void* ptr)
{
  int r = *(int*)ptr;

  printf("[Veteran] Trying bike %d...\n", r);
  pthread_mutex_lock(&bikes[r]);
  
  sleep(SECONDS);

  printf("[Veteran] Trying order %d...\n", r);
  if(pthread_mutex_trylock(&orders[r]) != 0){
    printf("[Veteran] Both resources are locked, releasing bike %d\n", r);
    pthread_mutex_unlock(&bikes[r]);
    return NULL;
  };


  printf("[Veteran] Successfully delivered order %d.\nFreeing resources...\n", r);
  pthread_mutex_unlock(&bikes[r]);
  pthread_mutex_unlock(&orders[r]);
  pthread_exit(NULL);
}

void*
novice(void* ptr)
{
  int r = *(int*)ptr;

  printf("[Novice] Trying order %d...\n", r);
  pthread_mutex_lock(&orders[r]);
  
  sleep(SECONDS);

  printf("[Novice] Trying bike %d...\n", r);
  if(pthread_mutex_trylock(&bikes[r]) != 0){
    printf("[Novice] Trying order %d...\n", r);
    pthread_mutex_unlock(&orders[r]);
    return NULL;
  };

  printf("[Novice] Successfully delivered order %d.\nFreeing resources...\n", r);
  pthread_mutex_unlock(&orders[r]);
  pthread_mutex_unlock(&bikes[r]);
  pthread_exit(NULL);
}

int
main(int argc, char* argv[]){
  srand(time(NULL));
  
  puts("Initializing mutexes...");
  for(int i = 0; i < N; i++){
    MUT_INIT(&orders[i]);
    MUT_INIT(&bikes[i]);
  }


  pthread_t veterans[N];
  pthread_t novices[N];
  int id_vet[N];
  int id_nov[N];
  int rc = 0;
  
  puts("Creating threads...");
  for(int i = 0; i < N; i++){
    id_vet[i] = rand() % N;
    id_nov[i] = rand() % N;
    
    pthread_create(&veterans[i], NULL, veteran, &id_vet[i]);
    pthread_create(&novices[i], NULL, novice, &id_nov[i]);
  }


  for(int i = 0; i < N; i++){
    pthread_join(veterans[i], NULL);
    pthread_join(novices[i], NULL);
  }
  return EXIT_SUCCESS;
}
