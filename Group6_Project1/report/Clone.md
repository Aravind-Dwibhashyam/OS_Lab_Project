# Project 1 Report: Implementation of the clone() System Call

**Author:** Dwibhashyam S S S Aravind
**Group:** Group 6
**Course:** NSCS210

---

### 1. User Space and the ecall Instruction

The user program runs the `clone` function present in the `main` function in the `test_clone.c` file in the user space. To trigger the OS, we store the function ID (defined in the `kernel/syscall.h` file) along with the arguments for the function in specific hardware registers. Then, it calls an `ecall` (environment call) instruction.

### 2. Context Switch and the Trampoline

The moment the `ecall` executes, the CPU stops the execution of the user program, gets into the kernel mode, and the current Program Counter (PC) is stored in the Exception Program Counter (EPC). The CPU then jumps into a hardcoded memory address in the kernel called the Trampoline. The trampoline takes all 32 of the user registers and dumps them into the parent's Trapframe. Now, the `proc.c` code starts running.

### 3. Memory Management: uvmmirror

A process always thinks that it owns memory from `0x0000` to `0xFFFF`. But this is virtual memory, and the OS uses a page table to translate these virtual addresses to physical addresses. When a `fork()` is executed, the kernel copies the actual physical RAM to a new location and builds a new page table for the child. But when `clone()` is executed, we use `uvmmirror` to perform a shallow copy. Here too, we give the child a brand new page table, but instead of copying the physical data, we copy the physical RAM addresses of the parent so that both of them point to the same data.

### 4. Trapframe Isolation and Execution

The reason why we are not directly copying `np->pagetable = p->pagetable` is because we don't want to copy the Trapframe (which is always located at the very top of the memory). If it does that, then when the CPU tries to wake one of them up, both will be awake and overwrite each other's registers.

`uvmmirror` solves this by giving them a new page table so that we can then just overwrite the EPC to the place where the child function starts (using `np->trapframe->epc = fcn`). We also tell it not to use the parent stack, but use the child stack using `np->trapframe->sp = stack_ptr`. We also plant the argument for the function directly into the return register for the C function to find it using `np->trapframe->a0 = arg`.

Finally, after the kernel is done, the thread is marked `RUNNABLE` and the CPU executes `sret` (supervisor return) so that it returns using the changes we made and not its default instructions.

---

## 🧪 Testing & Verification

To verify the shared physical memory mapping and isolated stack architecture, the `test_clone.c` user program was executed in the QEMU emulator. The parent and child successfully incremented a global `shared_counter` synchronously.

**Execution Output:**
```text
$ test_clone
--- Testing clone() System Call ---
[Parent] Initial shared_counter = 0
[Child]  Hello from the thread!
[Child]  Incrementing shared_counter...
[Child]  shared_counter is now = 1
[Parent] Child thread finished.
[Parent] Final shared_counter = 1

SUCCESS: Memory is successfully shared!
```

