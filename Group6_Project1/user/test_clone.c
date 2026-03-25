#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

//creating a global variable. If fork() is used, the child gets a seperate copy of this. If clone() is called, they share the same exact variable
int shared_counter = 0;

// The isolated function our thread will execute
void thread_worker(void *arg) {
  printf("[Child]  Hello from the thread!\n");
  printf("[Child]  Incrementing shared_counter...\n");
  
  shared_counter++; 
  
  printf("[Child]  shared_counter is now = %d\n", shared_counter);
  exit(0); // Thread MUST exit when finished
}

int main(void) {
	printf("---Testing clone() system call---\n");
	printf("[Parent] Initial shared_counter = %d\n", shared_counter);

	//allocate 1 page of memory (4096 bytes) to the child's new stack.
	void *stack = malloc(4096);
	if (stack == 0) {
		printf("Error: Malloc failed\n");
		exit(1);
	}

	//stacks grow DOWNWARDS in memory. So we must pass the pointer to the TOP of the allocated memory block. We align it to 16 bytes, which is standard for RISC-V architecture.
	uint64 stack_top = (uint64)stack + 4096;
	stack_top = stack_top & (~0xF);

	//calling the clone function
	int pid = clone(thread_worker, 0, (void *)stack_top);

	if(pid < 0) {
		printf("Error: Clone failed\n");
		exit(1);
	}

	if(pid == 0) {
		//child thread
		printf("[Child] Incrementing the shared counter\n");
		shared_counter++;
		printf("[Child] Shared counter is now = %d\n", shared_counter);
		exit(0);
	}

	else {
		//parent process
		wait(0);
		printf("[Parent] Child thread finished\n");
		printf("[Parent] Final shared counter = %d\n", shared_counter);
		if (shared_counter == 1) {
			printf("SUCCESS: Memory is successfully shared between parent and child\n");
		}
		else if (shared_counter == 0) {
			printf("FAILURE: Memory is isolated like a normal fork()\n");
		}
	}

	exit(0);
}
