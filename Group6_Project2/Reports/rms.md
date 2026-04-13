# Rate-Monotonic Scheduling (RMS) Report

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
