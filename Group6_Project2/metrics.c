#include <stdio.h>
#include "scheduler.h"

void print_metrics (struct Process processes[], int num_processes) {
	float total_waiting_time = 0;
	float total_turnaround_time = 0;

	printf("\nFINAL PERFORMANCE METRICS\n");
	printf("PID\tArrival\tBurst\tI/O\tCompletion\tTurnaround\tWaiting\n");

	for (int i=0; i<num_processes; i++) { 
		printf("P%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
				processes[i].pid,
				processes[i].arrival_time,
				processes[i].burst_time,
				processes[i].io_wait_time,
				processes[i].completion_time,
				processes[i].turnaround_time,
				processes[i].waiting_time
			);
		total_waiting_time += processes[i].waiting_time;
		total_turnaround_time += processes[i].turnaround_time;
	}
	printf("\nAverage Turnaround Time: %.2f\n", total_turnaround_time / num_processes);
	printf("Average Waiting Time: %.2f\n", total_waiting_time / num_processes);
}

void print_gantt_chart (int num_cpus, int total_time, int timeline[10][1000]) {
	printf("\nMULTI-PROCESSOR GANTT CHART\n");
	
	printf("Time:\t");
	for (int i=0; i<total_time; i++) {
		printf("%d\t", t);
	}
	printf("\n");

	for (int i=0; i<num_cpus; i++) {
		printf("CPU %d:\t", i);
		for (int j=0; j<total_time; j++) {
			if (timeline[i][j] > 0) {
				printf("[P%d]\t", timeline[i][j]);
			}
			else {
				printf("IDLE\t");
			}
		}
		printf("\n");
	}
	printf("\n");
}

