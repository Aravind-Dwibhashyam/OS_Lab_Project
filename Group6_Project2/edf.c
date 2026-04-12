#include <stdio.h>
#include "scheduler.h"

void run_edf(struct Process processes[], int num_processes, int num_cpus) {
    int time = 0;
    int completed = 0;

    // Timeline matrix
    int timeline[num_cpus][MAX_TIME];
    for (int i = 0; i < num_cpus; i++)
        for (int j = 0; j < MAX_TIME; j++)
            timeline[i][j] = 0;

    // Locking array to prevent multiple CPUs from executing a single process
    int is_running[100] = {0};

    // Dynamic array for CPUs
    struct Processor cpus[num_cpus];
    for (int i = 0; i < num_cpus; i++) {
        cpus[i].core_id = i;
        cpus[i].current_pid = -1;
        cpus[i].time_left_on_core = 0;
    }

    // Track deadline misses
    int deadline_misses = 0;

    // Main CPU loop
    while (completed < num_processes && time < MAX_TIME) {

        // STEP 1: Check completions
        for (int c = 0; c < num_cpus; c++) {
            if (cpus[c].current_pid != -1) {
                int p_idx = -1;
                for (int i = 0; i < num_processes; i++)
                    if (processes[i].pid == cpus[c].current_pid) { p_idx = i; break; }

                if (processes[p_idx].remaining_time == 0) {
                    processes[p_idx].completion_time = time + processes[p_idx].io_wait_time;
                    processes[p_idx].turnaround_time = processes[p_idx].completion_time - processes[p_idx].arrival_time;
                    processes[p_idx].waiting_time    = processes[p_idx].turnaround_time - processes[p_idx].burst_time - processes[p_idx].io_wait_time;

                    // Check deadline miss
                    if (processes[p_idx].completion_time > processes[p_idx].deadline) {
                        deadline_misses++;
                        processes[p_idx].deadline_missed = 1;
                    }

                    is_running[p_idx] = 0;
                    cpus[c].current_pid = -1;
                    completed++;
                }
            }
        }

        // STEP 2: Preemption check — if a newly arrived process has earlier deadline,
        //         preempt the running process on the CPU with the latest deadline
        for (int i = 0; i < num_processes; i++) {
            if (processes[i].arrival_time == time && processes[i].remaining_time > 0 && !is_running[i]) {
                // Find CPU running the process with the latest deadline
                int worst_cpu = -1;
                int worst_deadline = -1;
                for (int c = 0; c < num_cpus; c++) {
                    if (cpus[c].current_pid != -1) {
                        int p_idx = -1;
                        for (int k = 0; k < num_processes; k++)
                            if (processes[k].pid == cpus[c].current_pid) { p_idx = k; break; }
                        if (processes[p_idx].deadline > worst_deadline) {
                            worst_deadline = processes[p_idx].deadline;
                            worst_cpu = c;
                        }
                    }
                }
                // Preempt if new process has earlier deadline
                if (worst_cpu != -1 && processes[i].deadline < worst_deadline) {
                    int p_idx = -1;
                    for (int k = 0; k < num_processes; k++)
                        if (processes[k].pid == cpus[worst_cpu].current_pid) { p_idx = k; break; }
                    is_running[p_idx] = 0;
                    cpus[worst_cpu].current_pid = -1;
                }
            }
        }

        // STEP 3: Assign idle CPUs — pick process with earliest deadline (EDF)
        for (int c = 0; c < num_cpus; c++) {
            if (cpus[c].current_pid == -1) {
                int best_idx = -1;
                int earliest_deadline = 999999;

                for (int i = 0; i < num_processes; i++) {
                    if (processes[i].arrival_time <= time &&
                        processes[i].remaining_time > 0 &&
                        !is_running[i]) {
                        if (processes[i].deadline < earliest_deadline) {
                            earliest_deadline = processes[i].deadline;
                            best_idx = i;
                        }
                    }
                }

                if (best_idx != -1) {
                    cpus[c].current_pid = processes[best_idx].pid;
                    is_running[best_idx] = 1;
                }
            }
        }

        // STEP 4: Execute for 1 time unit and record timeline
        for (int c = 0; c < num_cpus; c++) {
            if (cpus[c].current_pid != -1) {
                timeline[c][time] = cpus[c].current_pid;

                int p_idx = -1;
                for (int i = 0; i < num_processes; i++)
                    if (processes[i].pid == cpus[c].current_pid) { p_idx = i; break; }

                processes[p_idx].remaining_time--;
            } else {
                timeline[c][time] = 0;
            }
        }

        time++;
    }

    print_gantt_chart(num_cpus, time, timeline);
    print_metrics(processes, num_processes);

    // EDF-specific: deadline summary
    printf("Deadline Misses: %d / %d processes\n", deadline_misses, num_processes);
    printf("Schedulability : %s\n",
           deadline_misses == 0 ? "FEASIBLE (all deadlines met)"
                                : "NOT FULLY FEASIBLE");
}