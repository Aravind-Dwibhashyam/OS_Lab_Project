#include "kernel/types.h"
#include "kernel/psinfo.h"
#include "user/user.h"

int
main(void)
{
  int pid, pid2;
  static struct procinfo pinfo[64];
  int n, i;

  printf("=== Forking a child process ===\n");

  pid = fork();
  if(pid == 0){
    // Child 1 — quietly sleep
    pause(50);
    exit(0);
  }

  // Child 2 — zombie
  pid2 = fork();
  if(pid2 == 0){
    exit(0);
  }

  // Parent wait karo taaki dono settle ho jayein
  pause(10);

  printf("Child 1 PID: %d\n", pid);
  printf("Child 2 (Zombie) PID: %d\n", pid2);

  printf("\n=== Process List (psinfo) ===\n");
  n = psinfo(pinfo, 64);
  if(n < 0){
    printf("psinfo failed\n");
    exit(1);
  }

  printf("PID\tNAME\t\tSTATE\n");
  printf("----------------------------------\n");
  for(i = 0; i < n; i++){
    printf("%d\t%s\t\t%s\n",
      pinfo[i].pid,
      pinfo[i].name,
      pinfo[i].state);
  }

  printf("\n=== Waiting for children to finish ===\n");
  wait(0);
  wait(0);
  printf("Children finished!\n");

  exit(0);
}