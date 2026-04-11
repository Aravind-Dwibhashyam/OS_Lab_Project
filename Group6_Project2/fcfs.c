// First-Come, First-Served (FCFS) Multiprocessor Scheduler
// Implemented by: Sriharsha
// Algorithm: Processes are dispatched to idle CPU cores strictly in arrival-time order.

#include <stdio.h>
#include <stdbool.h>
#include "scheduler.h"

void run_fcfs (struct Process processes[], int num_processes, int num_cpus) {
	int time = 0;
	int completed = 0;

	// Creating the timeline matrix for the Gantt chart
	int timeline[num_cpus][MAX_TIME];
	for (int i=0; i<num_cpus; i++) {
		for (int j=0; j<MAX_TIME; j++) {
			timeline[i][j] = 0;
		}
	}

	// Locking array to prevent multiple CPUs from executing a single process
	int is_running[100] = {0};

	// Dynamic array for CPUs
	struct Processor cpus[num_cpus];
	for (int i=0; i<num_cpus; i++) {
		cpus[i].core_id = i;
		cpus[i].current_pid = -1;
		cpus[i].time_left_on_core = 0;
	}

	// Sort processes by arrival time (for strict FCFS ordering)
	// Simple selection sort — stable enough for small N
	for (int i=0; i<num_processes-1; i++) {
		for (int j=i+1; j<num_processes; j++) {
			if (processes[j].arrival_time < processes[i].arrival_time) {
				struct Process temp = processes[i];
				processes[i] = processes[j];
				processes[j] = temp;
			}
		}
	}

	// Main simulation loop — one iteration per millisecond tick
	while (completed < num_processes && time < MAX_TIME) {

		// STEP 1: Check for process completions on each CPU
		for (int c=0; c<num_cpus; c++) {
			if (cpus[c].current_pid != -1) {
				int p_idx = -1;
				// Find the process array index from the PID
				for (int i=0; i<num_processes; i++) {
					if (processes[i].pid == cpus[c].current_pid) {
						p_idx = i;
					}
				}

				// If this process has finished its burst
				if (processes[p_idx].remaining_time == 0) {
					processes[p_idx].completion_time = time + processes[p_idx].io_wait_time;
					processes[p_idx].turnaround_time = processes[p_idx].completion_time - processes[p_idx].arrival_time;
					processes[p_idx].waiting_time = processes[p_idx].turnaround_time - processes[p_idx].burst_time - processes[p_idx].io_wait_time;

					is_running[p_idx] = 0;
					cpus[c].current_pid = -1;
					completed++;
				}
				// FCFS is non-preemptive — a running process keeps its CPU until done
			}
		}

		// STEP 2: Assign idle CPUs the next available process (earliest arrival first)
		for (int c=0; c<num_cpus; c++) {
			if (cpus[c].current_pid == -1) {
				int best_idx = -1;
				int earliest_arrival = 9999;

				// Scan for the process with the earliest arrival time that has arrived and is not running
				for (int i=0; i<num_processes; i++) {
					if (processes[i].arrival_time <= time && processes[i].remaining_time > 0 && !is_running[i]) {
						if (processes[i].arrival_time < earliest_arrival) {
							earliest_arrival = processes[i].arrival_time;
							best_idx = i;
						}
						// Tie-break on PID (lower PID first)
						else if (processes[i].arrival_time == earliest_arrival && best_idx != -1 && processes[i].pid < processes[best_idx].pid) {
							best_idx = i;
						}
					}
				}

				// Assign the selected process to this CPU
				if (best_idx != -1) {
					cpus[c].current_pid = processes[best_idx].pid;
					is_running[best_idx] = 1;
					// FCFS runs the process to completion — set time_left_on_core to remaining burst
					cpus[c].time_left_on_core = processes[best_idx].remaining_time;
				}
			}
		}

		// STEP 3: Execute for 1 millisecond and record the timeline
		for (int c=0; c<num_cpus; c++) {
			if (cpus[c].current_pid != -1) {
				timeline[c][time] = cpus[c].current_pid;

				// Find the process and decrement its remaining time
				int p_idx = -1;
				for (int i=0; i<num_processes; i++) {
					if (processes[i].pid == cpus[c].current_pid) p_idx = i;
				}
				processes[p_idx].remaining_time--;
				cpus[c].time_left_on_core--;
			}
			else {
				timeline[c][time] = 0;
			}
		}

		// Time has passed
		time++;
	}

	print_gantt_chart(num_cpus, time, timeline);
	print_metrics(processes, num_processes);
}
