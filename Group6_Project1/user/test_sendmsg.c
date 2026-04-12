#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(void) {
    printf("--- Starting Custom IPC Test ---\n");

    int pid = fork();

    if(pid < 0) {
        printf("Fork failed!\n");
        exit(1);
    }

    if (pid > 0) {
        // PARENT PROCESS: Sender
        char *secret_message = "Hello from the parent process!";
        printf("[Parent] Sending message: '%s'\n", secret_message);
        
        if(sendmsg(secret_message) < 0) {
            printf("[Parent] Failed to send message.\n");
        }
        
        wait(0); // Wait for child to exit before finishing
    } else {
        // CHILD PROCESS: Receiver
        char buffer[128];
        
        // Custom busy-wait loop: Keep trying to read until it succeeds (returns 0)
        // This completely avoids needing the sleep() function!
        while(recvmsg(buffer) < 0) {
            // Do nothing, just keep spinning and checking the mailbox
        }
        
        printf("[Child] Message successfully received: '%s'\n", buffer);
        exit(0);
    }

    printf("--- Test Finished ---\n");
    exit(0);
}
