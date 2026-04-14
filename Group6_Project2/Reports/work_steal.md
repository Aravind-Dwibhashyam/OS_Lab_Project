# Technical Report: Multiprocessor Work-Stealing Scheduler Simulation

---

## 1. Core Logic of Work-Stealing

The Work-Stealing scheduler is a load-balancing multiprocessor algorithm designed to keep all CPU cores active while minimizing the bottleneck of a single centralized queue. The objective is to distribute tasks efficiently across all available cores and allow idle cores to dynamically assist overloaded ones.

The scheduler operates using independent data structures for each core:

### Private CPU Queues
Instead of a global queue, each CPU gets its own private queue of processes. These are implemented as Double-Ended Queues (Deques) to allow process retrieval from both ends.

### Local Task Execution
When a CPU needs a task to execute, it looks at its own private queue first. It always pops the next available process from the **front** of its deque.

### The Steal Mechanism
If a CPU finishes all its work but another CPU is overloaded, it must "steal" a process from the back of the overloaded queue to balance the execution load. By popping from the **back**, it minimizes interference with the victim CPU, which is simultaneously pulling from the front.

### Load Balancing at Arrival
Every newly arriving process is initially assigned to the CPU with the shortest current queue length, ensuring a relatively even baseline distribution before any stealing is required.

---

## 2. Centralized vs Distributed Scheduling

The extension from standard scheduling to work-stealing fundamentally changes how processes are managed across multiple cores.

### Global Queue Systems (Centralized)
In algorithms like FCFS or traditional MLFQ, all processors pull from one shared list. The decision space is straightforward, but this creates a massive contention bottleneck as every core fights to lock the same queue simultaneously.

### Work-Stealing Systems (Distributed)
The scheduler must manage isolated queues while preventing idle time. The problem becomes safely accessing another CPU's data only when necessary. 

Two constraints arise:

- **Deque Isolation** A CPU must be able to push and pop from its own queue efficiently without being locked out by other CPUs.

- **Dynamic Load Balancing** No core should remain idle if runnable processes exist in any queue across the entire system.

The scheduler allows lock-free local execution most of the time, only crossing boundaries during a steal.

---

## 3. Implementation Design

The simulation is implemented to model execution. 

### 3.1 Tick-Based Simulation Engine

Execution progresses in discrete time units (**1 millisecond per tick**). At each tick, the simulator performs:

- Check for new arrivals and distribute them to the shortest CPU queue
- Assign processes to idle CPU cores via local pop or work-stealing
- Decrement remaining execution times by one unit
- Check for process completion

This approach ensures accurate representation of concurrent execution across multiple cores.

### 3.2 Dynamic Resource Allocation

The simulation logic adapts to the number of CPU cores and processes dynamically using arrays. A dedicated array-based `CPUQueue` struct is instantiated for every core requested by the user, avoiding fixed upper bounds and allowing the simulator to scale.

### 3.3 Concurrency Control (Front/Back Popping)

To prevent multiple cores from grabbing the same process during a steal, the Deque structure strictly separates interaction ends:

- The owning CPU exclusively pops from `front`
- A stealing CPU exclusively pops from `rear` (back)

This explicit separation ensures a process is cleanly removed and bound to a single core at any moment.

### 3.4 Separation of Execution and Metrics

Execution logic and performance metrics are computed independently. 

**Metrics are calculated as:**
Turnaround Time = Completion Time - Arrival Time
Waiting Time = Turnaround Time - Burst Time - I/O Wait Time

---

## 4. Summary

The simulation models a multiprocessor Work-Stealing scheduler with:

- Private double-ended queues for each CPU core
- Dynamic load distribution at arrival time
- Active work-stealing from the rear of overloaded queues
- Tick-level execution for accurate concurrency modeling
- Explicit handling of multi-core scheduling constraints
