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