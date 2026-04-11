#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define NTHREAD 3
#define STACK_SIZE 4096

int sem = 1;

void
worker(void *arg)
{
  int id = (int)(uint64)arg;

  printf("worker %d: waiting for lock\n", id);
  if(sem_wait(&sem) < 0){
    printf("worker %d: sem_wait failed\n", id);
    exit(1);
  }

  printf("worker %d: entered critical section\n", id);
  pause(50);
  printf("worker %d: leaving critical section\n", id);

  if(sem_post(&sem) < 0){
    printf("worker %d: sem_post failed\n", id);
    exit(1);
  }

  exit(0);
}

int
main(void)
{
  int i;
  int pid[NTHREAD];
  void *stack[NTHREAD];

  printf("semtest: semaphore initialized to 1\n");

  for(i = 0; i < NTHREAD; i++){
    stack[i] = malloc(STACK_SIZE);
    if(stack[i] == 0){
      printf("semtest: malloc failed\n");
      exit(1);
    }

    pid[i] = clone(worker, (void *)(uint64)i, stack[i] + STACK_SIZE);
    if(pid[i] < 0){
      printf("semtest: clone failed\n");
      exit(1);
    }
  }

  for(i = 0; i < NTHREAD; i++)
    wait(0);

  for(i = 0; i < NTHREAD; i++)
    free(stack[i]);

  printf("semtest: done\n");
  exit(0);
}
