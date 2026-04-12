#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "scheduler.h"

// Dharavath Hrishikesh / 24JE0614
void run_lottery (struct Process processes[], int num_processes, int num_cpus) {
    int time = 0;
    int completed = 0;
    int timeline[num_cpus][MAX_TIME];

    // Initialize timeline with 0 (idle)
    for (int i=0; i<num_cpus; i++) {
        for (int j=0; j<MAX_TIME; j++) timeline[i][j] = 0;
    }

    int is_running[100] = {0};
    struct Processor cpus[num_cpus];
    for (int i=0; i<num_cpus; i++) {
        cpus[i].core_id = i;
        cpus[i].current_pid = -1;
    }

    while (completed < num_processes && time < MAX_TIME) {
        // STEP 1: Release CPUs from finished processes
        for (int c=0; c<num_cpus; c++) {
            if (cpus[c].current_pid != -1) {
                int p_idx = -1;
                for (int i=0; i<num_processes; i++) {
                    if (processes[i].pid == cpus[c].current_pid) p_idx = i;
                }
                if (processes[p_idx].remaining_time == 0) {
                    processes[p_idx].completion_time = time;
                    processes[p_idx].turnaround_time = time - processes[p_idx].arrival_time;
                    processes[p_idx].waiting_time = processes[p_idx].turnaround_time - processes[p_idx].burst_time;
                    is_running[p_idx] = 0;
                    cpus[c].current_pid = -1;
                    completed++;
                }
            }
        }

        // STEP 2: Assign idle CPUs new work using Lottery Logic
        for (int c=0; c<num_cpus; c++) {
            if (cpus[c].current_pid == -1) {
                int total_tickets = 0;
                // Count available tickets
                for (int i=0; i<num_processes; i++) {
                    if (processes[i].arrival_time <= time && processes[i].remaining_time > 0 && !is_running[i]) {
                        total_tickets += (processes[i].priority + 1); // priority + 1 to avoid 0 tickets
                    }
                }

                if (total_tickets > 0) {
                    int winning_ticket = rand() % total_tickets;
                    int current_sum = 0;
                    for (int i=0; i<num_processes; i++) {
                        if (processes[i].arrival_time <= time && processes[i].remaining_time > 0 && !is_running[i]) {
                            current_sum += (processes[i].priority + 1);
                            if (current_sum > winning_ticket) {
                                cpus[c].current_pid = processes[i].pid;
                                is_running[i] = 1;
                                break;
                            }
                        }
                    }
                }
            }
        }

        // STEP 3: Execute for 1ms
        for (int c=0; c<num_cpus; c++) {
            if (cpus[c].current_pid != -1) {
                timeline[c][time] = cpus[c].current_pid;
                for (int i=0; i<num_processes; i++) {
                    if (processes[i].pid == cpus[c].current_pid) processes[i].remaining_time--;
                }
            }
        }
        time++;
    }
    print_gantt_chart(num_cpus, time, timeline);
    print_metrics(processes, num_processes);
}