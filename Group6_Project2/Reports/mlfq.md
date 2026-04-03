# Technical Report: Multiprocessor Multilevel Feedback Queue (MLFQ) Simulation

---

## 1. Core Logic of MLFQ

The Multilevel Feedback Queue (MLFQ) addresses the scheduling trade-off between responsiveness and throughput. The objective is to prioritize short, interactive (I/O-bound) processes while still allowing long-running (CPU-bound) processes to complete, without prior knowledge of process length.

The scheduler operates using multiple priority queues arranged hierarchically:

### Hierarchical Queues
The system maintains multiple queues with descending priority levels. Higher priority queues use smaller time quantums. Lower priority queues use larger quantums or follow First-Come, First-Served (FCFS).

### Initial Placement
Every arriving process is inserted into the highest priority queue (Queue 0). The assumption is that the process is short and requires immediate CPU access.

### Demotion Policy
If a process consumes its full time quantum without yielding or completing, it is preempted and moved to the next lower priority queue. This identifies CPU-bound behavior.

### Priority Enforcement
The scheduler always selects processes from the highest non-empty queue. Lower priority queues are considered only when all higher queues are empty.

This structure allows the scheduler to adapt dynamically to process behavior over time.

---

## 2. Single-Processor vs Multi-Processor MLFQ

The extension from single-core to multi-core execution changes the scheduling problem.

### Single-Processor System
The scheduler operates along a single timeline. At any moment, only one process is selected. Context switching occurs when a process blocks or exhausts its quantum. The decision space is limited to selecting the next process.

### Multi-Processor System (SMP)
The scheduler must assign processes across multiple CPU cores simultaneously. The problem becomes selecting the top **N** processes for **N** cores at each time step.

Two constraints arise:

- **Avoiding Double Scheduling**  
  A process must not be assigned to more than one core at the same time.

- **Load Balancing**  
  No core should remain idle if runnable processes exist in any queue.

The scheduler must maintain global consistency across all cores at each time step.

---

## 3. Implementation Design

The simulation is implemented to model execution. We have considered using the rand() to assign the initial values like arrival time, burst time, etc.

### 3.1 Tick-Based Simulation Engine

Execution progresses in discrete time units (**1 millisecond per tick**). At each tick, the simulator performs:

- Check for process completion or quantum exhaustion  
- Preempt and demote processes if required  
- Assign processes to idle CPU cores  
- Decrement remaining execution times by one unit  

This approach ensures accurate representation of concurrent execution across multiple cores.

---

### 3.2 Dynamic Resource Allocation

The number of CPU cores and processes is determined at runtime. Memory is allocated dynamically using **Variable Length Arrays (VLAs)**.

This avoids fixed upper bounds and allows the simulator to scale with input size.

---

### 3.3 Concurrency Control (`is_running` State)

To prevent multiple cores from selecting the same process, a global tracking structure (`is_running`) is used.

- When a process is assigned to a core, it is marked as active  
- Other cores skip processes marked as active during the same tick  
- The flag is cleared when the process is preempted or completes  

This ensures each process is bound to a single core at any moment.

---

### 3.4 Separation of Execution and Metrics

Execution logic and performance metrics are computed independently.

**Metrics are calculated as:**
Turnaround Time = Completion Time - Arrival Time
Waiting Time = Turnaround Time - Burst Time - I/O Wait Time

---

## 4. Summary

The simulation models a multiprocessor MLFQ scheduler with:

- Strict priority-based queue selection  
- Dynamic demotion based on runtime behavior  
- Tick-level execution for accurate concurrency modeling  
- Explicit handling of multi-core scheduling constraints  
- Global state tracking to prevent scheduling conflicts  
