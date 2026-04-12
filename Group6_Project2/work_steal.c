#include <stdio.h>
#include "scheduler.h"

//Dhruv Thakkar / 24JE0615

// Custom Double-Ended Queue (Deque) structure to handle private CPU queues
typedef struct {
    int process_indices[100]; // Array to hold indices of the processes array
    int front;
    int rear;
    int size;
} CPUQueue;

// Push a process index to the back of the queue
void push_back(CPUQueue *q, int p_idx) {
    q->process_indices[q->rear++] = p_idx;
    q->size++;
}

// Pop a process from the front (for the CPU's own local tasks)
int pop_front(CPUQueue *q) {
    if(q->size == 0) return -1;
    q->size--;
    return q->process_indices[q->front++];
}

// Pop a process from the back (for Work Stealing from other CPUs)
int pop_back(CPUQueue *q) {
    if(q->size == 0) return -1;
    q->size--;
    return q->process_indices[--q->rear];
}

void run_work_steal(struct Process processes[], int num_processes, int num_cpus) {
    CPUQueue queues[num_cpus];
    struct Processor cores[num_cpus];
    int timeline[num_cpus][MAX_TIME];
    int arrived[100] = {0}; // Track which processes have entered the system

    // 1. Initialize our custom queues and processor states
    for(int i = 0; i < num_cpus; i++) {
        queues[i].front = 0;
        queues[i].rear = 0;
        queues[i].size = 0;
        cores[i].core_id = i;
        cores[i].current_pid = -1; // -1 represents an idle CPU
    }

    // Initialize the timeline Gantt chart array to 0 (Idle)
    for(int i = 0; i < num_cpus; i++) {
        for(int j = 0; j < MAX_TIME; j++) {
            timeline[i][j] = 0; 
        }
    }

    int current_time = 0;
    int completed_processes = 0;

    printf("\n--- Starting Custom Work-Stealing Simulation ---\n");

    // 2. Main Simulation Loop
    while(completed_processes < num_processes && current_time < MAX_TIME) {
        
        // A. Load Balancing: Add newly arriving processes to the CPU with the shortest queue
        for(int i = 0; i < num_processes; i++) {
            if(processes[i].arrival_time == current_time && arrived[i] == 0) {
                int min_q = 0;
                for(int c = 1; c < num_cpus; c++) {
                    if(queues[c].size < queues[min_q].size) {
                        min_q = c;
                    }
                }
                push_back(&queues[min_q], i);
                arrived[i] = 1;
            }
        }

        // B. Assign Work: Check queues or Steal
        for(int i = 0; i < num_cpus; i++) {
            if(cores[i].current_pid == -1) { // If CPU is idle
                
                // Try to pop from the FRONT of its own queue
                int p_idx = pop_front(&queues[i]);

                // If local queue is empty, trigger the WORK STEAL logic
                if(p_idx == -1) {
                    int max_q = -1;
                    int max_size = 0;
                    
                    // Scan all other CPUs to find the most overloaded queue
                    for(int c = 0; c < num_cpus; c++) {
                        if(c != i && queues[c].size > max_size) {
                            max_size = queues[c].size;
                            max_q = c;
                        }
                    }

                    // Steal a process from the BACK of the overloaded queue
                    if(max_q != -1) {
                        p_idx = pop_back(&queues[max_q]);
                    }
                }

                // If a process was found (locally or stolen), assign it to the core
                if(p_idx != -1) {
                    cores[i].current_pid = processes[p_idx].pid;
                }
            }
        }

        // C. Execution and Timeline Recording
        for(int i = 0; i < num_cpus; i++) {
            if(cores[i].current_pid != -1) {
                timeline[i][current_time] = cores[i].current_pid; // Record in Gantt Chart

                // Find the index of the running process
                int p_idx = -1;
                for(int j = 0; j < num_processes; j++) {
                    if(processes[j].pid == cores[i].current_pid) {
                        p_idx = j;
                        break;
                    }
                }

                // Simulate 1 unit of execution time
                if(p_idx != -1) {
                    processes[p_idx].remaining_time--;

                    // Process finishes execution
                    if(processes[p_idx].remaining_time == 0) {
                        processes[p_idx].completion_time = current_time + 1;
                        processes[p_idx].turnaround_time = processes[p_idx].completion_time - processes[p_idx].arrival_time;
                        processes[p_idx].waiting_time = processes[p_idx].turnaround_time - processes[p_idx].burst_time;
                        
                        completed_processes++;
                        cores[i].current_pid = -1; // Free the CPU for the next clock cycle
                    }
                }
            } else {
                timeline[i][current_time] = 0; // CPU remains idle
            }
        }

        current_time++; // Advance the system clock
    }

    // 3. Print the final calculated metrics
    print_gantt_chart(num_cpus, current_time, timeline);
    print_metrics(processes, num_processes);
}
