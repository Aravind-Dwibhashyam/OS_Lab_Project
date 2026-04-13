# Project 1 Report: Implementation of the sendmsg() System Call

*Author:* Dhruv Thakkar
*Group:* Group 6
*Course:* NSCS210

# 1. User Space and the ecall Instruction (The Trigger)
The process starts in user space when a program calls `sendmsg(msg)` or `recvmsg(buffer)`. To talk to the Operating System, the program puts your custom IDs (`SYS_sendmsg 23` and `SYS_recvmsg 24`) and the arguments into the CPU's hardware registers. Then, it executes the `ecall` instruction.

Added the system call definitions to `user.h` and `syscall.h`.

This is the "doorbell" that tells the CPU to stop running the user program and switch to the Kernel to perform a privileged task.

# 2. Kernel Interface: sys_sendmsg (The Wrapper)
Once the CPU enters the kernel, it lands in `sys_sendmsg` and `sys_recvmsg` in `kernel/sysproc.c`. Because the kernel cannot directly "see" user variables, it must safely extract them from the hardware registers using `argaddr` (for the memory pointers).

Wrote `sys_sendmsg` to "unwrap" the arguments from the registers.

It acts as a security guard, making sure the arguments sent by the user are valid before passing them to the core logic.

# 3. Core Logic: The Kernel Mailbox (The Brain)
This is the heart of the project in `kernel/sysproc.c`. Because processes have completely separate memory spaces and cannot directly talk to each other, this sets up a shared buffer inside the kernel. 

If the parent sends a message: The kernel acquires a custom spinlock, safely copies the user's string into a secure global mailbox buffer (`char msg[128]`), marks the mailbox state as FULL, and releases the lock.

If the child reads a message: The kernel acquires the lock, checks if a message exists, uses `copyout` to safely push the string back into the child's user memory space, clears the mailbox state, and releases the lock.

Standard xv6 processes are strictly isolated; this specific logic allows one process to safely copy a message string into the kernel, which another process can later read.

# 4. The Busy-Wait Loop (The Synchronization Logic)
I implemented a custom busy-wait loop to handle process synchronization without relying on standard blocking calls.

What I did: Added a continuous check inside the child process: `while(recvmsg(buffer) < 0) { }`.

Why I did it: Instead of putting the process to sleep, this allows the receiver to actively spin and check the kernel mailbox. It ensures the child perfectly catches the message the exact moment the parent deposits it, demonstrating a highly distinct approach to synchronization.

# 🧪 Testing & Verification
To prove the system call works, I created `test_sendmsg.c`. This program forks a child process. The parent deposits a string into the kernel space, and the child actively waits to retrieve and print it, successfully demonstrating Inter-Process Communication.

*Execution Output:*
```text
$ test_sendmsg
--- Starting Custom IPC Test ---
[Parent] Sending message: 'Hello from the parent process!'
[Child] Message successfully received: 'Hello from the parent process!'
--- Test Finished ---
