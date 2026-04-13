#include "kernel/types.h"
#include "kernel/psinfo.h"
#include "user/user.h"

int
main(void)
{
  static struct procinfo pinfo[64];
  int n, i;

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
  exit(0);
}