# OS Lab Project-2: Multiprocessor Scheduling Simulator

## Workflow and Pull Requests
To prevent merge conflicts, everyone should create a branch with their name and do the work in that branch only. We can merge them into the main at the end after you send a PR. 
I have created individual `.c` files for each algorithm and made placeholders for the functions where you will write your code. Add your name and admission number wherever you are doing any work.

### Step-1: Clone and Branch
```bash
git clone [https://github.com/Aravind-Dwibhashyam/OS_Lab_Project](https://github.com/Aravind-Dwibhashyam/OS_Lab_Project)
cd OS_Lab_Project/Group6_Project2
git checkout -b [yourName]
```

### Step-2: Where to write your code
**Algorithm Logic:** Open your specifically assigned C file (e.g., `fcfs.c`, `lottery.c`). You will see the function placeholder there.
**Dependencies:** You must implement your logic using the process arrays and multiprocessor structs defined in the shared `scheduler.h` file. Do not edit `scheduler.h` or `main.c` without discussing it first.

### Step-3: Testing and screenshots
Compile and run the simulator to test your specific algorithm:
```bash
gcc main.c [your_assigned_file].c metrics.c -o scheduler
./scheduler
```
*(Ensure your algorithm successfully generates the multiprocessor Gantt chart and calculates the correct metrics).*

Take screenshots of the execution and share it in the whatsapp group.

### Step-4: Pushing the code and creating a PR
```bash
git add [your_assigned_file].c
git commit -m "Write your updates"
git push origin yourBranchName
```

## Work allotted

1. **Multilevel Feedback Queue (MLFQ) - Dwibhashyam S S S Aravind / 24je0617**
   * **What it is:** A complex multi-queue priority scheduler.
   * **The Task:** Manage multiple priority queues across multiple simulated CPUs. If a process exhausts its allotted time slice on a CPU, it must be demoted to a lower-priority queue to prevent CPU hogging.

2. **First-Come, First-Served (FCFS) - Gattu Sri Harsha / 24je0618**
   * **What it is:** The baseline multiprocessor algorithm.
   * **The Task:** Maintain a single global queue of processes. Whenever any CPU core becomes idle, it simply pulls the next available process based strictly on its arrival time.

3. **Lottery Scheduler - Dharavath Hrishikesh / 24je0614**
   * **What it is:** A probabilistic multiprocessor scheduler.
   * **The Task:** Processes are assigned "tickets" based on their priority. When a CPU is idle, the scheduler generates a random number to draw a winning ticket, giving higher-priority tasks a statistically better chance of running.

4. **Earliest Deadline First (EDF) - Dhanya Gautam / 24je0613**
   * **What it is:** A dynamic Real-Time scheduling algorithm.
   * **The Task:** Maintain a global queue of real-time tasks. As CPUs become free, the algorithm must dynamically evaluate all available processes and always select the one whose absolute deadline is closest to the current clock cycle.

5. **Rate-Monotonic Scheduling (RMS) - Dipesh Jain / 24je0616**
   * **What it is:** A static Real-Time scheduling algorithm.
   * **The Task:** This algorithm pins tasks to specific CPUs (Partitioned Scheduling). The priority of a task is static and determined strictly by its period: tasks with shorter periods are given higher priority and will preempt slower tasks.

6. **Work-Stealing Scheduler (Custom) - Dhruv Thakkar / 24je0615**
   * **What it is:** A load-balancing multiprocessor algorithm.
   * **The Task:** Instead of a global queue, each CPU gets its own private queue of processes. If CPU 0 finishes all its work but CPU 1 is overloaded, CPU 0 must "steal" a process from the back of CPU 1's queue to balance the execution load.

---
---

# Technical Report: Multiprocessor Multilevel Feedback Queue (MLFQ) Simulation
**Author: Dwibhashyam S S S Aravind**<br>
**Admission.no: 24je0617**<br>

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

## 4. Results

```text
dwibh@Valakhilyasa:~/OS_Lab_Project/Group6_Project2$ ./scheduler 
Enter the number of processes: 6
Enter the number of CPUs: 3

Select Algorithm to run:
1. Multilevel feedback queue (MLFQ)
2. First-come, first-serve (FCFS)
3. Lottery scheduler
4. Earliest deadline first (EDF)
5. Rate-monotonic scheduling (RMS)
6. Work-stealing scheduler
Choice: 1

MULTI-PROCESSOR GANTT CHART
Time:	0	1	2	3	4	5	6	7	8	9	10	11	12	13	
CPU 0:	[P2]	[P2]	[P5]	[P5]	[P1]	[P1]	[P3]	[P3]	[P3]	[P3]	[P5]	[P5]	[P5]	IDLE	
CPU 1:	[P6]	[P6]	[P3]	[P3]	[P6]	[P5]	[P5]	[P5]	[P5]	[P2]	[P3]	[P3]	IDLE	IDLE	
CPU 2:	IDLE	[P4]	[P2]	[P2]	[P2]	[P2]	[P1]	[P1]	[P1]	[P1]	[P1]	IDLE	IDLE	IDLE	


FINAL PERFORMANCE METRICS
PID	Arrival	Burst	I/O	Deadline	Completion	Turnaround	Waiting
P1	3	7	2	15		13		10		1
P2	0	7	1	16		11		11		3
P3	2	8	2	19		14		12		2
P4	1	1	1	4		3		2		0
P5	1	9	2	19		15		14		3
P6	0	3	1	10		6		6		2

Average Turnaround Time: 9.17
Average Waiting Time: 1.83
```

---

## 5. Summary

The simulation models a multiprocessor MLFQ scheduler with:

- Strict priority-based queue selection  
- Dynamic demotion based on runtime behavior  
- Tick-level execution for accurate concurrency modeling  
- Explicit handling of multi-core scheduling constraints  
- Global state tracking to prevent scheduling conflicts  
# Rate-Monotonic Scheduling (RMS) Report

---
---

## 1. Title
**Project:** Operating Systems Lab Project 2 - Scheduling Algorithms  
**Module:** Rate-Monotonic Scheduling (RMS)  
**Implementation Language:** C  
**Scheduling Type:** Preemptive, Static-Priority, Partitioned Multiprocessor

## 2. Introduction to Real-Time Scheduling
Real-time scheduling is used in systems where tasks must complete within timing constraints. In these systems, correctness depends on both logical output and completion time. A scheduler must allocate CPU time so deadlines are met predictably.

## 3. What is RMS
Rate-Monotonic Scheduling (RMS) is a fixed-priority real-time scheduling algorithm for periodic tasks.

- Each task has an execution time and a period.
- Priority is assigned statically from period.
- Smaller period means higher priority.
- The scheduler is preemptive.

## 4. Key Characteristics
- **Static priority:** Priority does not change at runtime.
- **Period-based priority:** A task with shorter period gets higher priority.
- **Preemption:** A higher-priority ready task can interrupt a lower-priority running task.

## 5. Algorithm Steps
1. Read number of tasks.
2. Read execution time and period for each task.
3. Compute utilization for each task: $u_i = C_i / T_i$.
4. Partition tasks across 2 CPUs before execution.
5. For each CPU, run independent preemptive RMS.
6. At each time unit, release periodic jobs.
7. Select ready task with smallest period on that CPU.
8. Update remaining execution time and timeline.
9. Compute utilization and schedulability test.
10. Print task table, timeline, and Gantt charts.

## 6. Multiprocessor Partitioned Scheduling Explanation
This implementation uses **partitioned scheduling** with **2 CPUs**.

- Tasks are assigned to CPU 0 or CPU 1 before simulation.
- Assignment is done once and remains fixed.
- Each CPU runs its own RMS scheduler independently.
- No task migrates between CPUs during runtime.

This keeps scheduling simple and deterministic.

## 7. Implementation Details
The RMS module is implemented in [Group6_Project2/rms.c](../rms.c).

- Uses simple arrays for execution time, period, assignment, remaining time, and timeline.
- Uses a discrete-time simulation model.
- Releases jobs periodically at times where $t \bmod T_i = 0$.
- Chooses the ready task with smallest period on each CPU at each time unit.
- Records timeline per CPU and prints events to show preemption points.
- Computes per-CPU and overall CPU utilization.
- Performs RMS schedulability check using Liu-Layland bound.

## 8. Sample Input
```text
Enter the number of processes: 4
Enter the number of CPUs: 2
Choice: 2
Task 1 execution time: 1
Task 1 period: 4
Task 2 execution time: 1
Task 2 period: 5
Task 3 execution time: 2
Task 3 period: 8
Task 4 execution time: 1
Task 4 period: 10
```

## 9. Sample Output
```text
Rate-Monotonic Scheduling (RMS)
Partitioned scheduling on 2 CPUs
Number of tasks: 4

Task Table
| Task ID | Execution Time | Period | CPU |
| T1      | 1              | 4      | 0   |
| T2      | 1              | 5      | 1   |
| T3      | 2              | 8      | 0   |
| T4      | 1              | 10     | 1   |

CPU Utilization
CPU 0: 37.50%
CPU 1: 30.00%
Overall: 33.75%

RMS Schedulability Test
CPU 0 -> U = 0.5000, Bound = 0.8284, Schedulable
CPU 1 -> U = 0.3000, Bound = 0.8284, Schedulable
Task set result: Schedulable under RMS partition
```

## 10. Gantt Chart Explanation
A separate Gantt chart is printed for each CPU.

- Each block represents the task running in a time interval.
- `Idle` means no ready task on that CPU.
- Block transitions indicate dispatch or preemption.
- Since RMS is preemptive, higher-priority tasks may interrupt lower-priority tasks when released.

## 11. CPU Utilization Formula
For one CPU with $n$ assigned periodic tasks:

$$
U = \sum_{i=1}^{n} \frac{C_i}{T_i}
$$

Where:
- $C_i$ = execution time of task $i$
- $T_i$ = period of task $i$

Overall utilization in 2-CPU simulation:

$$
U_{overall} = \frac{busy_0 + busy_1}{2 \times simulation\_time}
$$

## 12. Schedulability Condition Explanation
RMS schedulability bound for $n$ tasks:

$$
U \le n\left(2^{1/n} - 1\right)
$$

- If utilization is below or equal to the bound, the task set is guaranteed schedulable.
- If utilization is above the bound, schedulability is not guaranteed by the bound test.

In this project, the bound test is applied **per CPU partition**.

## 13. Advantages
- Simple fixed-priority policy.
- Low runtime overhead.
- Predictable behavior for periodic tasks.
- Good practical performance for many embedded systems.

## 14. Disadvantages
- Requires known periodic task model.
- Lower-priority tasks may suffer long delays.
- Liu-Layland bound is sufficient but not necessary, so some feasible sets may be rejected.
- Partitioning quality affects multiprocessor efficiency.

## 15. Conclusion
RMS was integrated as a separate module while preserving existing project structure. The implementation supports preemptive periodic scheduling with static priorities, partitioned assignment to two CPUs, timeline and Gantt visualization, utilization reporting, and formal schedulability testing. This provides a clean and extensible real-time scheduling component for Project 2.

---
---

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

---
---

**Name:** Dhanya Gautam
**Adm_no.:** 24je0613

# Technical Report: Multiprocessor Earliest Deadline First (EDF) Simulation

**Course:** NSCS210 / CSC211 — Operating Systems Lab  
**Project:** Project 2 — Advanced Scheduling Algorithms  
**Instructor:** Dr. Jaishree Mayank

---

## 1. Introduction

Real-time systems require guarantees that critical tasks complete before their deadlines. General-purpose schedulers like Round Robin or FCFS do not account for timing constraints, making them unsuitable for such environments.

**Earliest Deadline First (EDF)** is a dynamic, preemptive scheduling algorithm that addresses this by always running the process with the most urgent deadline. It is widely used in real-time operating systems and is the theoretical benchmark for deadline-based scheduling.

---

## 2. Core Logic of EDF

### 2.1 Dynamic Priority Assignment
EDF does not assign fixed priorities at process creation. Instead, priority is recalculated at every time unit based on each process's absolute deadline. The ready process with the smallest deadline value is always selected next.

### 2.2 Preemption Policy
EDF is fully preemptive. If a new process arrives whose deadline is earlier than the currently running process, the running process is immediately preempted and the new process takes the CPU.

### 2.3 Optimality
EDF is provably optimal on single-processor systems — if any valid schedule exists that meets all deadlines, EDF will find one. This is a stronger guarantee than any fixed-priority algorithm (such as RMS) can provide.

> **Schedulability condition (single processor):**  
> A set of n tasks is schedulable under EDF if:  
> `U = Σ (Ci / Di) ≤ 1`  
> where Ci = execution time and Di = deadline of task i.

On multiprocessor systems, full optimality is not always guaranteed, but EDF remains one of the most effective algorithms available.

---

## 3. Single-Processor vs Multi-Processor EDF

| Aspect | Single-Processor | Multi-Processor (SMP) |
|---|---|---|
| Cores scheduled per tick | 1 | N |
| Preemption target | Current process | Process with latest deadline across all cores |
| Scheduling decision | Pick 1 earliest deadline | Pick N earliest deadlines |
| Conflict prevention | Not needed | `is_running` lock per process |

### Multi-Processor Behaviour
At each tick, the scheduler fills all N cores with the N ready processes having the earliest deadlines. When a new process arrives mid-execution, the scheduler compares its deadline against all currently running processes and preempts the one with the latest deadline if applicable.

---

## 4. Implementation Design

### 4.1 Tick-Based Simulation Engine
Execution advances in discrete **1ms time units**. Each tick performs four steps in order:

1. **Completion check** — processes that finished are recorded and metrics computed
2. **Preemption check** — newly arrived processes preempt if their deadline is earlier
3. **Assignment** — idle cores are filled with the earliest-deadline ready processes
4. **Execution** — each running process has its `remaining_time` decremented by 1

### 4.2 Data Structures

```
Process {
    pid, arrival_time, burst_time, remaining_time
    deadline, io_wait_time
    completion_time, turnaround_time, waiting_time
    deadline_missed
}

Processor {
    core_id, current_pid, time_left_on_core
}

timeline[num_cpus][MAX_TIME]   — records which PID ran on which core at each tick
is_running[100]                — prevents a process from being assigned to 2 cores
```

### 4.3 Preemption Mechanism

```
On process arrival at time t:
  1. Find the core running the process with the LATEST deadline
  2. If new_process.deadline < that_process.deadline:
       → preempt: mark that process as not running, free the core
  3. Core is now idle and will be filled in the assignment step
```

### 4.4 Performance Metrics

```
Turnaround Time  =  Completion Time − Arrival Time
Waiting Time     =  Turnaround Time − Burst Time − I/O Wait Time
Deadline Miss    =  1  if  Completion Time > Deadline
```

EDF-specific summary metrics reported after simulation:
- **Average Turnaround Time**
- **Average Waiting Time**  
- **Deadline Miss Count** — number of processes that exceeded their deadline
- **Schedulability verdict** — FEASIBLE or NOT FULLY FEASIBLE

### 4.5 Test Data Generation
Process parameters are generated using `rand()` to simulate varied workloads:

| Parameter | Range |
|---|---|
| Arrival time | 0 – 4 |
| Burst time | 1 – 10 |
| Deadline | arrival + burst + (0–9) |
| I/O wait | 0 – 2 |

---

## 5. Example Output

```
Enter the number of processes: 5
Enter the number of CPUs: 2

Select Algorithm to run:
1. Multilevel feedback queue (MLFQ)
2. First-come, first-serve (FCFS)
3. Lottery scheduler
4. Earliest deadline first (EDF)
5. Rate-monotonic scheduling (RMS)
6. Work-stealing scheduler
Choice: 4

MULTI-PROCESSOR GANTT CHART
Time:   0       1       2       3       4       5       6       7       8       9       10      11      12      13      14      15      16      17      18      19      20 
CPU 0:  [P3]    [P1]    [P1]    [P1]    [P1]    [P1]    [P1]    [P1]    [P1]    [P4]    [P4]    [P4]    [P4]    [P4]    [P4]    IDLE    IDLE    IDLE    IDLE    IDLE    IDLE
CPU 1:  IDLE    [P5]    [P5]    [P5]    [P5]    [P5]    [P3]    [P3]    [P3]    [P3]    [P3]    [P2]    [P2]    [P2]    [P2]    [P2]    [P2]    [P2]    [P2]    [P2]    IDLE


FINAL PERFORMANCE METRICS
PID     Arrival Burst   I/O     Deadline        Completion      Turnaround      Waiting Missed
P1      1       8       2       9               11              10              0       [MISS]
P2      4       9       2       15              22              18              7       [MISS]
P3      0       6       1       13              12              12              5       [OK]
P4      1       6       0       14              15              14              8       [MISS]
P5      1       5       1       9               7               6               0       [OK]

Average Turnaround Time: 12.00
Average Waiting Time: 4.00
Deadline Misses: 3 / 5 processes
Schedulability : NOT FULLY FEASIBLE
```

---

## 6. Summary

The simulation models a multiprocessor EDF scheduler with:

- Dynamic priority recalculated every tick based on absolute deadlines
- Full preemption — tighter deadline processes immediately displace running ones
- Multi-core support with explicit conflict prevention via `is_running` locks
- Tick-accurate Gantt chart across all CPU cores
- Deadline miss detection and schedulability reporting per run

---
---

# Technical Report: Multiprocessor First-Come, First-Served (FCFS) Simulation

---

## 1. Core Logic of FCFS

First-Come, First-Served (FCFS) is the simplest CPU scheduling algorithm. Processes are dispatched to available CPU cores in the exact order they arrive in the ready queue, with no preemption. Once a process begins executing on a core, it runs to completion before that core considers any other process.

### Key Properties

- **Non-preemptive:** A running process is never interrupted; it holds the CPU until its entire burst is finished.
- **Arrival-time ordering:** The process with the earliest arrival time is always selected first. Ties are broken by process ID (lower PID first).
- **No starvation (guaranteed):** Every process will eventually be scheduled because no process can be indefinitely overtaken by higher-priority arrivals.

### Trade-offs

FCFS is easy to implement and understand but suffers from the **convoy effect** — short processes arriving behind a long process must wait for the long process to finish, inflating average waiting times.

---

## 2. Single-Processor vs Multi-Processor FCFS

### Single-Processor System
With a single core, FCFS maintains a single ready queue and dispatches processes one at a time. The scheduler selects the next process only after the current one completes. Average waiting time depends heavily on the order of arrival and burst lengths.

### Multi-Processor System (SMP)
With multiple cores, the same global ready queue is maintained, but up to **N** processes can execute concurrently on **N** cores. When any core becomes idle, the next process from the sorted queue is assigned to it.

Two constraints must be enforced:

- **Avoiding Double Scheduling**
  A process must not be assigned to more than one core at the same time. A global `is_running` flag array ensures mutual exclusion.

- **Full Core Utilization**
  No core should remain idle while runnable processes exist in the ready queue.

The multiprocessor extension preserves FCFS ordering while exploiting parallelism to reduce total completion time.

---

## 3. Implementation Design

The simulation models execution using the shared structures (`struct Process`, `struct Processor`) defined in `scheduler.h`, following the same tick-based architecture as the other scheduling algorithms in the project.

### 3.1 Tick-Based Simulation Engine

Execution progresses in discrete time units (**1 millisecond per tick**). At each tick, the simulator performs three phases:

1. **Completion check:** For every CPU core, check if the currently running process has finished (remaining_time == 0). If so, record its completion time (including I/O wait), compute turnaround and waiting times, free the core, and increment the completed count.
2. **Dispatch:** For every idle core, scan the process list for the arrived, non-running process with the earliest arrival time and assign it. FCFS uses no quantum — the process runs until its burst is exhausted.
3. **Execute:** Decrement the remaining time of every running process by one unit and record the CPU–process assignment in the timeline matrix.

---

### 3.2 Arrival-Time Sorting

Before the simulation loop begins, the process array is sorted by arrival time using selection sort. This ensures that the linear scan in the dispatch phase naturally picks the correct FCFS order.

---

### 3.3 Concurrency Control (`is_running` State)

To prevent multiple cores from selecting the same process, a global `is_running` array is used:

- When a process is assigned to a core, its entry is set to 1
- Other cores skip processes marked as active during the same tick
- The flag is cleared when the process completes

This is identical to the approach used in the MLFQ implementation.

---

### 3.4 Separation of Execution and Metrics

Execution logic and performance metrics are computed independently, using the shared `metrics.c` functions.

**Metrics are calculated as:**
- Turnaround Time = Completion Time − Arrival Time
- Waiting Time = Turnaround Time − Burst Time − I/O Wait Time

---

## 4. Test Execution

### Terminal Output Screenshot

![FCFS Terminal Output](fcfs_screenshot.png)

### 4.1 Compilation

```bash
gcc main.c fcfs.c mlfq.c metrics.c -o scheduler
```

### 4.2 Input

```
Enter the number of processes: 5
Enter the number of CPUs: 2

Select Algorithm to run:
1. Multilevel feedback queue (MLFQ)
2. First-come, first-serve (FCFS)
3. Lottery scheduler
4. Earliest deadline first (EDF)
5. Rate-monotonic scheduling (RMS)
6. Work-stealing scheduler
Choice: 2
```

### 4.3 Output — Gantt Chart

```
MULTI-PROCESSOR GANTT CHART
Time:   0       1       2       3       4       5       6       7       8       9       10      11      12      13      14      15      16      17      18      19      20      21
CPU 0:  [P3]    [P3]    [P3]    [P3]    [P3]    [P3]    [P4]    [P4]    [P4]    [P4]    [P4]    [P4]    [P2]    [P2]    [P2]    [P2]    [P2]    [P2]    [P2]    [P2]    [P2]    IDLE
CPU 1:  IDLE    [P1]    [P1]    [P1]    [P1]    [P1]    [P1]    [P1]    [P1]    [P5]    [P5]    [P5]    [P5]    [P5]    IDLE    IDLE    IDLE    IDLE    IDLE    IDLE    IDLE    IDLE
```

### 4.4 Output — Performance Metrics

```
FINAL PERFORMANCE METRICS
PID     Arrival Burst   I/O     Completion      Turnaround      Waiting
P3      0       6       1       7               7               0
P1      1       8       2       11              10              0
P4      1       6       0       12              11              5
P5      1       5       1       15              14              8
P2      4       9       2       23              19              8

Average Turnaround Time: 12.20
Average Waiting Time: 4.20
```

### 4.5 Analysis of Results

- **P3** (arrival=0, burst=6) is the first to arrive and is immediately assigned to CPU 0. It completes at time 6 with **0 waiting time**.
- **P1** (arrival=1, burst=8) arrives at t=1 and is assigned to CPU 1 (which was idle). It runs to completion at t=9 with **0 waiting time**.
- **P4** (arrival=1, burst=6) arrives at the same time as P1 but CPU 1 is taken. It waits until CPU 0 finishes P3 at t=6, then runs until t=12. **Waiting time = 5**.
- **P5** (arrival=1, burst=5) waits for CPU 1 to finish P1 at t=9, then runs until t=14. **Waiting time = 8**.
- **P2** (arrival=4, burst=9) arrives at t=4 but both CPUs are busy. It gets CPU 0 at t=12 after P4 finishes, completing at t=21. **Waiting time = 8**.

The **convoy effect** is visible: P2 has a high waiting time (8) despite having burst=9, because it had to wait behind earlier arrivals on both cores.

---

## 5. Summary

The simulation models a multiprocessor FCFS scheduler with:

- Strict arrival-time ordering for process dispatch
- Non-preemptive execution — each process runs to completion once started
- Tick-level execution for accurate concurrency modeling
- Explicit handling of multi-core scheduling constraints
- Global state tracking (`is_running`) to prevent scheduling conflicts
- Gantt chart and performance metric output via shared utility functions
# Technical Report: Multiprocessor Lottery Scheduling Simulation

**Author:** Dharavath Hrishikesh (24JE0614)  
**Group:** Group 6  
**Course:** Operating System

---

## 1.The Raffle System
Instead of a standard queue where the first process always goes first, implemented a **Lottery System**. In this model, CPU time is treated like a prize in a raffle.

* **Tickets:** Every process is assigned "tickets" based on its priority.
* **The Draw:** When a CPU core becomes free, the scheduler holds a raffle. It picks a random number, and the process holding that "winning ticket" gets to run.
* **The Rule:** **More Tickets = Higher Priority = Better statistical chance of winning.**

---

## 2.The "Fair-Share" Solution
In many OS algorithms, low-priority tasks suffer from **starvation** (they never run because high-priority tasks keep cutting in line).

> **The Solution:** In this Lottery Scheduler, even a process with only **1 ticket** still has a mathematical chance to win. It might wait longer, but it is guaranteed to run eventually. This is known as **Fair-Share Scheduling**.

---

## 3.Multiprocessor Logic
Handling multiple CPUs simultaneously makes the logic more complex. I solved two primary technical hurdles:

1.  **The "Double Booking" Problem:** Two CPUs must not run the same process at the same time.
    * **Solution:** implemented an `is_running` flag. Before a CPU picks a winner, it checks this state. If the process is already busy on another core, the scheduler bypasses it.
2.  **The "Idle CPU" Problem:** We must ensure no CPU sits empty if there is work available.
    * **Solution:** My engine is designed so that whenever any CPU finishes its task, it immediately triggers a new "Lottery Draw" to grab the next available process from the global pool.

---

## 4. Implementation Walkthrough
I built a **Tick-Based Engine** where 1 loop equals 1 millisecond. Every "tick," the code performs the following:

* **Collect Tickets:** Identifies all processes that have arrived and are currently ready (not finished or already running).
* **Draw Winner:** Uses `rand()` to pick a winning ticket number from the total pool.
* **Assign:** Places that process onto the idle CPU and marks its `is_running` status.
* **Execute:** Decreases the process's `remaining_time` by 1 unit.

---

## 5. Test Results & Analysis
The simulator was tested with **5 Processes** and **2 CPUs**. 

### The Gantt Chart (Timeline)
The output chart demonstrates **CPU 0** and **CPU 1** working side-by-side:
* At **Time 1**, `P2` is active on CPU 0 while `P5` is active on CPU 1, proving **true multiprocessing**.
* **IDLE** gaps correctly appear when no processes have arrived yet, proving the **Arrival Time** logic is functional.

### Performance Metrics
| **Average Turnaround Time** | **10.80** | Total time from arrival to completion. |
| **Average Waiting Time** | **4.40** | Total time spent sitting in the ready queue. |

---
