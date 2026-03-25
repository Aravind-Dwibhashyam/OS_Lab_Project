#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "vm.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  kexit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return kfork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return kwait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int t;
  int n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  if(t == SBRK_EAGER || n < 0) {
    if(growproc(n) < 0) {
      return -1;
    }
  } else {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    if(addr + n < addr)
      return -1;
    if(addr + n > TRAPFRAME)
      return -1;
    myproc()->sz += n;
  }
  return addr;
}

uint64
sys_pause(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kkill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

//We will add the function definitions below. Creating different zones to make sure we dont have any merge conflicts

//NAME: xxx	Adm.No: xxx
uint64 sys_waitpid(void) {
	return 0;
}

//NAME: xxx	Adm.No: xxx
uint64 sys_sendmsg(void) {
	return 0;
}

//NAME: Dwibhashyam S S S Aravind	Adm.No: 24je0617
uint64 sys_clone(void) {
	uint64 fcn, arg, stack_ptr;
	//argaddr() is defines in kernel/syscall.c. It take int n, pointer p, assigns the a(n)'s trapframe of the parent process calling it to the pointer p. argaddr() calles argraw() also defined in kernel/syscall.c and that calls the myproc() to find which process actually called this function. myproc() is defined in kernel/proc.c
	//we are executing this step because if we directly call the clone(), the kernel has no way of reading its data in the user stack. So we are first retrieving the trapframe from the user mode for further use in the kernel space.
	argaddr(0, &fcn);
	argaddr(1, &arg);
	argaddr(2, &stack_ptr);

	//The actual clone() that will be defined in kernel/defs.h and write the working code in kernel/proc.c
	return clone(fcn, arg, stack_ptr);
}

//NAME: xxx	Adm.No: xxx
uint64 sys_sem_wait(void) {
	return 0;
}

//NAME: xxx	Adm.No: xxx
uint64 sys_alarm(void) {
	return 0;
}

//NAME: xxx	Adm.No: xxx
uint64 sys_psinfo(void) {
	return 0;
}

