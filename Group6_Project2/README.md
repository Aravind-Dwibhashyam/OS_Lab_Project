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

2. **First-Come, First-Served (FCFS) - Unassigned**
   * **What it is:** The baseline multiprocessor algorithm.
   * **The Task:** Maintain a single global queue of processes. Whenever any CPU core becomes idle, it simply pulls the next available process based strictly on its arrival time.

3. **Lottery Scheduler - Unassigned**
   * **What it is:** A probabilistic multiprocessor scheduler.
   * **The Task:** Processes are assigned "tickets" based on their priority. When a CPU is idle, the scheduler generates a random number to draw a winning ticket, giving higher-priority tasks a statistically better chance of running.

4. **Earliest Deadline First (EDF) - Unassigned**
   * **What it is:** A dynamic Real-Time scheduling algorithm.
   * **The Task:** Maintain a global queue of real-time tasks. As CPUs become free, the algorithm must dynamically evaluate all available processes and always select the one whose absolute deadline is closest to the current clock cycle.

5. **Rate-Monotonic Scheduling (RMS) - Unassigned**
   * **What it is:** A static Real-Time scheduling algorithm.
   * **The Task:** This algorithm pins tasks to specific CPUs (Partitioned Scheduling). The priority of a task is static and determined strictly by its period: tasks with shorter periods are given higher priority and will preempt slower tasks.

6. **Work-Stealing Scheduler (Custom) - Unassigned**
   * **What it is:** A load-balancing multiprocessor algorithm.
   * **The Task:** Instead of a global queue, each CPU gets its own private queue of processes. If CPU 0 finishes all its work but CPU 1 is overloaded, CPU 0 must "steal" a process from the back of CPU 1's queue to balance the execution load.
