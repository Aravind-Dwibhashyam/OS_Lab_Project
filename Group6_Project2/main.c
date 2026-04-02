#include <stdio.h>
#include <stdlib.h>
#include "scheduler.h"

void initialize_test_data (struct Process p[], int n) {
	for (int i=0; i<n; i++) {
		p[i].pid = i+1;
		p[i].arrival_time = rand()%5;
		p[i].burst_time = (rand%10) + 1;
		p[i].remaining_time = p[i].burst_time;
		p[i].priority = rand()%5;
		p[i].deadline = p[i].arrival_time + p[i].burst_time + (rand()%10);
		p[i].period = p[i].deadline+5;
		p[i].io_wait_time = rand()%3;
	}
}

int main() {
	int num_processes, num_cpus;
	int choice;
	
	printf("Enter the number of processes");
	scanf("%d", &num_processes);	
	printf("Enter the number of CPUs");
	scanf("%d", &num_cpus);

	struct Process processes[num_processes];

	//generate dummy data
	initialize_test_data (processes, num_processes);

	printf("\nSelect Algorithm to run:\n");
	printf("1. Multilevel feedback queue (MLFQ)\n");
	printf("2. First-come, first-serve (FCFS)"\n);
	printf("3. Lottery scheduler\n");
	printf("4. Earliest deadline first (EDF)\n");
	printf("5. Rate-monotonic scheduling (RMS)\n");
	printf("6. Work-stealing scheduler\n");
	printf("Choice: ");
	scanf("%d", &choice);

	switch(choice) {
		case 1: run_mlfq(processes, num_processes, num_cpus); break;
		case 2: run_fcfs(processes, num_processes, num_cpus); break;
		case 3: run_lottery(processes, num_processes, num_cpus); break;
		case 4: run_edf(processes, num_processes, num_cpus); break;
		case 5: run_rms(processes, num_processes, num_cpus); break;
		case 6: run_work_steal(processes, num_processes, num_cpus); break;
		default: printf("Invalid choice\n");
	}

	return 0;
}
