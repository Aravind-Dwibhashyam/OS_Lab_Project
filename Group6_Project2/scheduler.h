#ifndef SCHEDULER_H
#define SCHEDULER_H
#define MAX_TIME 1000
struct Process {
	int pid;
	int arrival_time;
	int burst_time;
	int remaining_time;

	//Algorithm parametres
	int priority;		//For MLFQ and Lottery
	int deadline;		//For EDF
	int period;		//For RMS
	int io_wait_time;	//For PS Requirements
				
	//Metric tracking
	int completion_time;
	int turnaround_time;
	int waiting_time;
};

struct Processor {
	int core_id;
	int current_pid;	//PID currently running (-1 if idle)
	int time_left_on_core	//How much longer the current process gets
};

//Function prototype
void run_mlfq (struct Process processes[], int num_processes, int num_cpus);
void run_fcfs (struct Process processes[], int num_processes, int num_cpus);
void run_lottery (struct Process processes[], int num_processes, int num_cpus);
void run_edf (struct Process processes[], int num_processes, int num_cpus);
void run_rms (struct Process processes[], int num_processes, int num_cpus);
void run_work_steal (struct Process processes[], int num_processes, int num_cpus);

void print_metrics (struct Process processes[], int num_processes);
void print_gantt_chart (int num_cpus, int total_time, int timeline[10][1000]);

#endif
