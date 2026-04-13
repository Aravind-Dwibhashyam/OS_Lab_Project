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
