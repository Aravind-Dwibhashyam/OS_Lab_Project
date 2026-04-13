#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// Busy-wait delay function
void delay(int iterations) {
    volatile int i, j;
    for(i = 0; i < iterations; i++) {
        for(j = 0; j < 10000; j++) {
            // Waste CPU cycles
        }
    }
}

int main(void) {
    int pid1, pid2;
    int status;
    int ret_pid;

    printf("\n=== Clean waitpid Demonstration ===\n\n");

    // TEST 1: Fork two children with distinct delay windows
    pid1 = fork();
    if (pid1 == 0) {
        delay(1500); // Child 1 waits long enough for Parent to start waiting
        printf("\t-> [CHILD 1] PID %d exiting (Status 10).\n", getpid());
        exit(10); 
    }

    pid2 = fork();
    if (pid2 == 0) {
        delay(3500); // Child 2 waits even longer
        printf("\t-> [CHILD 2] PID %d exiting (Status 20).\n", getpid());
        exit(20);
    }

    // TEST 2: WNOHANG (Non-blocking)
    printf("[PARENT] Checking Child 2 with WNOHANG (Option 1)...\n");
    ret_pid = waitpid(pid2, &status, 1); 
    if (ret_pid == 0) {
        printf("[PARENT] Success: Child 2 is still running. Moving on...\n\n");
    }
    
    delay(50); // Small buffer to let the console clear

    // TEST 3: Blocking Wait for Child 2
    printf("[PARENT] Now waiting for Child 2 (PID %d)...\n", pid2);
    
    // CRITICAL: We wait a tiny bit to ensure "Waiting..." is fully printed 
    // before the Child's delay(3500) expires.
    delay(100); 
    
    ret_pid = waitpid(pid2, &status, 0);
    if (ret_pid == pid2) {
        printf("[PARENT] Success: Captured Child 2. Status: %d\n\n", status);
    }

    delay(50); // Buffer

    // TEST 4: Blocking Wait for Child 1
    printf("[PARENT] Now waiting for Child 1 (PID %d)...\n", pid1);
    
    delay(100); // Buffer
    
    ret_pid = waitpid(pid1, &status, 0);
    if (ret_pid == pid1) {
        printf("[PARENT] Success: Captured Child 1. Status: %d\n\n", status);
    }

    // TEST 5: Error Handling
    printf("[PARENT] Checking a non-existent PID (9999)...\n");
    ret_pid = waitpid(9999, &status, 0);
    if (ret_pid == -1) {
        printf("[PARENT] Success: Correctly rejected fake PID.\n\n");
    }

    printf("=== Final Demonstration Complete ===\n\n");
    exit(0);
}