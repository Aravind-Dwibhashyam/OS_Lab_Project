# Project 1 Report: Implementation of the waitpid() System Call

*Author:* Dharavath Hrishikesh
*Group:* Group 6
*Course:* NSCS210

# 1. User Space and the ecall Instruction (The Trigger)
The process starts in user space when a program calls waitpid(pid, &status, options). To talk to the Operating System, the program puts your custom ID (SYS_waitpid 22) and the arguments into the CPU's hardware registers. Then, it executes the ecall instruction.

Added the system call definition to user.h and syscall.h.

This is the "doorbell" that tells the CPU to stop running the user program and switch to the Kernel to perform a privileged task.

# 2. Kernel Interface: sys_waitpid (The Wrapper)
Once the CPU enters the kernel, it lands in sys_waitpid in kernel/sysproc.c. Because the kernel cannot directly "see" user variables, it must safely extract them from the hardware registers using argint (for the PID and options) and argaddr (for the status pointer).

Wrote sys_waitpid to "unwrap" the arguments from the registers.

It acts as a security guard, making sure the arguments sent by the user are valid before passing them to the core logic.

# 3. Core Logic: kwaitpid (The Brain)
This is the heart of the project in kernel/proc.c. The kernel scans the entire process table looking for a child that matches the specific target_pid.

If the child is a ZOMBIE: The kernel "harvests" it, copies its exit status back to the user's memory, and cleans up its data.

If the child is still RUNNING: The parent process is put to sleep() on its own address.

Standard wait() is too broad; this specific logic allows a parent to be a "focused waiter" for one specific child.

# 4. The WNOHANG Feature (The "Don't Wait" Logic)
I implemented the options parameter to support non-blocking waits. If the user passes 1 (WNOHANG), the kernel checks the child's status once. If the child isn't finished yet, the kernel returns 0 immediately instead of sleeping.

What I did: Added a check inside the loop: if(options == 1) { return 0; }.

Why I did it: This allows "multitasking" parents. They can check if a child is done and, if not, go back to doing other work instead of freezing.

# 🧪 Testing & Verification
To prove the system call works, I created test_waitpid.c. This program forks two children with different delays to verify that the parent can specifically wait for the second child while the first one is still alive.

*Execution Output:*


$ test_waitpid
=== Clean waitpid Demonstration ===

[PARENT] Checking Child 2 with WNOHANG (Option 1)...
[PARENT] Success: Child 2 is still running. Moving on...

[PARENT] Now waiting for Child 2 (PID 5)...
    -> [CHILD 1] PID 4 exiting (Status 10).
    -> [CHILD 2] PID 5 exiting (Status 20).
[PARENT] Success: Captured Child 2. Status: 20

[PARENT] Now waiting for Child 1 (PID 4)...
[PARENT] Success: Captured Child 1. Status: 10

=== All tests finished perfectly ===