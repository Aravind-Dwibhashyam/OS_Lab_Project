# Project 1 Report: Implementation of the psinfo() System Call

**Author:** Dhanya Gautam
**Adm_no:** 24je0613
**Group:** Group 6
**Course:** NSCS210

---

# 1. User Space and the ecall Instruction (The Trigger)
The process starts in user space when a program calls psinfo(pinfo, max). To talk to the Operating System, the program puts the custom ID (SYS_psinfo 27) and the arguments into the CPU's hardware registers. Then, it executes the ecall instruction.

Added the system call definition to user.h, usys.pl, and syscall.h.

This is the "doorbell" that tells the CPU to stop running the user program and switch to the Kernel to perform a privileged task.

---

# 2. Kernel Interface: sys_psinfo (The Wrapper)
Once the CPU enters the kernel, it lands in sys_psinfo in kernel/sysproc.c. Because the kernel cannot directly "see" user variables, it must safely extract them from the hardware registers using argaddr (for the buffer pointer) and argint (for the max count).

Wrote sys_psinfo to "unwrap" the arguments from the registers.

It acts as a security guard, making sure the pointer and count sent by the user are valid before passing them to the core logic.

---

# 3. Core Logic: psinfo (The Brain)
This is the heart of the project in kernel/proc.c. The kernel scans the entire process table (proc[NPROC]) looking for all active (non-UNUSED) processes.

For each active process, it reads:
- **PID**: The unique process identifier.
- **Name**: The name of the executable running.
- **State**: Whether it is RUNNING, SLEEPING, RUNNABLE, ZOMBIE, or USED.

Standard debugging tools like procdump() only print to the console. This system call instead returns structured data to user space, making it programmable and useful for building tools like a custom top or ps command.

---

# 4. Safe Memory Transfer with copyout (The Bridge)
The most critical part of psinfo is safely writing the kernel data back to user space. Since the kernel and user program have completely separate memory spaces, a direct pointer write would cause a page fault and kernel panic.

What I did: Used copyout(caller->pagetable, (uint64)(pinfo + count), &info, sizeof(info)) to safely transfer each process's data one struct at a time.

Why I did it: This is the correct and safe way to write from kernel space to user space in xv6. It validates the user address against the process's page table before writing, preventing memory corruption.

---

# 5. Synchronization with Locks (The Safety Net)
When reading the process table, other CPUs could be modifying it simultaneously. To prevent reading corrupted data, proper locking is essential.

What I did: Acquired wait_lock before scanning the table, and acquired each individual p->lock before reading a process's fields.

Why I did it: This follows the same locking discipline used by the rest of xv6's process management code (like procdump), ensuring data consistency in a multicore environment.

---

# 🧪 Testing & Verification
To prove the system call works, I created two test programs:

**1. psinfo** — Basic test that simply lists all running processes.

**2. test_psinfo.c** —  Advanced test that forks two child processes: one that sleeps (SLEEPING) and one that exits immediately without being waited on (ZOMBIE), verifying all three states appear correctly in a single psinfo call.

*Execution Output (psinfo):*

PID     NAME            STATE
----------------------------------
1       init            SLEEPING
2       sh              SLEEPING
3       psinfo          RUNNING

*Execution Output (test_psinfo):*

=== Forking a child process ===
Child 1 PID: 4
Child 2 (Zombie) PID: 5

=== Process List (psinfo) ===
PID     NAME            STATE
----------------------------------
1       init            SLEEPING
2       sh              SLEEPING
3       test_psinfo             RUNNING
4       test_psinfo             SLEEPING
5       test_psinfo             ZOMBIE

=== Waiting for children to finish ===
Children finished!

---

*All tests passed successfully. The psinfo system call correctly reads the kernel process table and safely transfers the data to user space.*


**For reffernce output image is attached below ->**

![psinfo output](output_images/image.png)