# OS Lab Project-1: Custom system calls in XV6 riscv architecture

## Workflow and Pull Requests
To prevent merge conflicts, everyone should create a branch with their name and do the work in that branch only. We can merge them into the main at the end after you send a PR. 
I have also added the placeholders for the functions and made individual workspaces in the file where you will write the code. Add your name and admission number wherever you are doing any work.

### Step-1: Clone and Branch
```bash
git clone https://github.com/Aravind-Dwibhashyam/OS_Lab_Project
cd OS_Lab_Project/Group6_Project1
git checkout -b [yourName]
```

### Step-2: Where to write your code
**Kernel Logic:** Open `kernel/sysproc.c` and scroll to the bottom. You will see it.
**User Program:** Open your specific test file in the `user/` directory. You must add a user program here that demonstrates the logic used.

### Step-3: Testing and screenshots
Compile and run the xv6 emulator to test your code:
```bash
make qemu
```
*(Use Ctrl+A, then X to exit qemu)*

Take screenshots of the execution and share it in the whatsapp group.

### Step-4: Pushing the code and creating a PR
```bash
make clean
git add kernel/sysproc.c user/[your_test].c
git commit -m "Write your updates"
git push origin yourBranchName
```

## Work allotted

1. **waitpid (Process Creation) - Unassigned**
   * **What it is:** A targeted version of the standard wait() call.
   * **The Task:** Standard xv6 suspends a parent process until any of its children finish. This custom system call allows a parent to wait for a specific child process (by providing its exact PID) to exit before resuming execution. It involves managing the process table and understanding parent/child lifecycles.

2. **sendmsg (Inter-Process Communication (IPC)) - Unassigned**
   * **What it is:** A secure message-passing system between isolated programs.
   * **The Task:** In xv6, processes have completely separate memory spaces and cannot directly talk to each other. This system call sets up a shared buffer inside the kernel, allowing one process to safely copy a message string into the kernel, which another process can later read.

3. **clone (Threads) - Dwibhashyam S S S Aravind / 24je0617**
   * **What it is:** The foundation for multithreading.
   * **The Task:** Unlike fork(), which creates a completely separate duplicate of a process's memory, clone creates a child process that shares the exact same virtual memory space (the same page directory) as its parent, while having its own independent user stack.

4. **sem_wait (Locks) - Unassigned**
   * **What it is:** A counting semaphore to prevent data corruption.
   * **The Task:** When multiple processes or threads try to access the same shared resource at the same time, data gets corrupted. This system call implements a lock that forces a process to "sleep" if a resource is currently in use, and "wake up" when the resource becomes available.

5. **alarm (Signals) - Unassigned**
   * **What it is:** A timer-based software interrupt.
   * **The Task:** This allows a user program to ask the kernel to interrupt it after a specific amount of time. The program calls alarm(ticks, handler_function), and after that number of CPU timer ticks pass, the kernel forces the program to pause and execute the specific handler function before resuming its normal code.

6. **psinfo (System Diagnostics) - Unassigned**
   * **What it is:** A snapshot of the operating system's current state.
   * **The Task:** This system call reads the kernel's internal process table and returns a formatted array containing the status of every active process running on the OS (including its PID, name, and whether it is RUNNING, SLEEPING, or a ZOMBIE).
