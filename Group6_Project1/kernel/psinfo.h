#ifndef PSINFO_H
#define PSINFO_H

struct procinfo {
  int pid;
  char name[16];
  char state[10];
};

#endif