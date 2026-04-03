#include <stdio.h>
#include <stdbool.h>
#include "scheduler.h"

#define Q0_QUANTUM 2
#define Q1_QUANTUM 4
#define Q2_QUANTUM 999		//Essentially making it FCFS

void run_mlfq (struct Process processes[], int num_processes, int num_cpus) {
	int time = 0;
	int completed = 0;

	//Creating the timeline matrix
	int timeline[num_cpus][MAX_TIME];
	for (int i=0; i<num_cpus; i++) {
		for (int j=0; j<MAX_TIME; j++) {
			timeline[i][j] = 0;
		}
	}

	//Locking array to prevent multiple CPUs from executing a single process
	int is_running[100] = {0};

	//Dynamic array for CPUs
	struct Processor cpus[num_cpus];	//The struct is defined in scheduler.h
	for (int i=0; i<num_cpus; i++) {
		cpus[i].core_id = i;
		cpus[i].current_pid = -1;
		cpus[i].time_left_on_core = 0;
	}

	//All new processes start in Q0
	for (int i=0; i<num_processes; i++) {
		processes[i].priority = 0;
	}

	//The main CPU loop
	while (completed < num_processes && time < MAX_TIME) {
		
		//STEP1: Complete the end of the previous time unit (Finishes and demotions)
		for (int c=0; c<num_cpus; c++) {
			//we need to find the processes that are hogging up the CPU
			//So we are checking for each CPU if any process is still using it and if so, we further check it
			if (cpus[c].current_pid != -1) {
				int p_idx = -1;
				//extract the array index from the PID
				for (int i=0; i<num_processes; i++) {
					if (processes[i].pid == cpus[c].current_pid) {
						p_idx = i;
					}
				}

				//If the process finished its execution at this instant
				if  (processes[p_idx].remaining_time == 0) {
					processes[p_idx].completion_time = time + processes[p_idx].io_wait_time;
					processes[p_idx].turnaround_time = processes[p_idx].completion_time - processes[p_idx].arrival_time;
					processes[p_idx].waiting_time = processes[p_idx].turnaround_time - processes[p_idx].burst_time - processes[p_idx].io_wait_time;

					is_running[p_idx] = 0;
					cpus[c].current_pid = -1;
					completed++;
				}

				//If the quantum expired without finsihing (CPU hogging)
				//Ideally, this will only executed on processes in Q0 or Q1 as Q2 will never actually reach the end of quantum (999) before it is completed
				else if (cpus[c].time_left_on_core == 0) {
					//Downgrade the priority
					if (processes[p_idx].priority < 2) {
						processes[p_idx].priority++;
					}
					is_running[p_idx] = 0;
					cpus[c].current_pid = -1;
				}
			}
		}

		//STEP2: Assign idle CPUs new work
		for (int c=0; c<num_cpus; c++) {
			if (cpus[c].current_pid == -1) {
				int best_idx = -1;
				int best_priority = 999;
				int earliest_arrival = 9999;
				
				//Check for an available process
				for  (int i=0; i<num_processes; i++) {
					if (processes[i].arrival_time <= time && processes[i].remaining_time > 0 && !is_running[i]) {
						if (processes[i].priority < best_priority) {
							best_priority = processes[i].priority;
							best_idx = i;
							earliest_arrival = processes[i].arrival_time;
						}
						else if (processes[i].priority == best_priority && processes[i].arrival_time < earliest_arrival) {
                            				best_idx = i;
                            				earliest_arrival = processes[i].arrival_time;
						}
					}
				}

				//If such a process exists, assign it to the CPU
				if (best_idx != -1) {
                 			cpus[c].current_pid = processes[best_idx].pid;
					is_running[best_idx] = 1;
                    			
					//Assign the quanta for each process
                   			if (processes[best_idx].priority == 0) cpus[c].time_left_on_core = Q0_QUANTUM;
                    			else if (processes[best_idx].priority == 1) cpus[c].time_left_on_core = Q1_QUANTUM;
                    			else cpus[c].time_left_on_core = Q2_QUANTUM;
                		}
			}
		}

		//Step3: Execute for 1 millisecond and record the timeline
		for (int c=0; c<num_cpus; c++) {
			if (cpus[c].current_pid != -1) {
				timeline[c][time] = cpus[c].current_pid;

				//update the requirements of every process after the millisecond
				int p_idx = -1;
				for (int i=0; i<num_processes; i++) {
					if (processes[i].pid == cpus[c].current_pid) p_idx = i;
				}
				processes[p_idx].remaining_time--;
				cpus[c].time_left_on_core--;
			}
			else timeline[c][time] = 0;
		}

		//Time has passed
		time++;
	}
	print_gantt_chart(num_cpus, time, timeline);
	print_metrics(processes, num_processes);
}

