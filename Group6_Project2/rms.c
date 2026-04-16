#include <stdio.h>
#include <stdlib.h>
#include "scheduler.h"

static int gcdi(int a, int b) {
	while (b != 0) {
		int t = a % b;
		a = b;
		b = t;
	}
	return a;
}

static int lcm_cap(int a, int b, int cap) {
	int g = gcdi(a, b);
	long long v = ((long long)a / g) * b;
	if (v > cap) {
		return cap;
	}
	return (int)v;
}

static double pwi(double x, int n) {
	double r = 1.0;
	int i;
	for (i = 0; i < n; i++) {
		r *= x;
	}
	return r;
}

static double root2n(int n) {
	double lo = 1.0;
	double hi = 2.0;
	double mid = 1.0;
	int i;
	for (i = 0; i < 80; i++) {
		mid = (lo + hi) / 2.0;
		if (pwi(mid, n) >= 2.0) {
			hi = mid;
		} else {
			lo = mid;
		}
	}
	return (lo + hi) / 2.0;
}

static double rms_bound(int n) {
	if (n <= 0) {
		return 0.0;
	}
	return n * (root2n(n) - 1.0);
}

static void print_timeline(int c, int h, int *tl) {
	int i;
	int prev = -1;

	printf("\nCPU %d Time vs Running Task\n", c);
	printf("Time :");
	for (i = 0; i < h; i++) {
		printf("%4d", i);
	}
	printf("\nTask :");
	for (i = 0; i < h; i++) {
		if (tl[i] == 0) {
			printf("%4s", "-");
		} else {
			printf("%4d", tl[i]);
		}
	}
	printf("\nEvents:");
	for (i = 0; i < h; i++) {
		if (i == 0 || tl[i] != prev) {
			if (tl[i] == 0) {
				printf(" [t=%d Idle]", i);
			} else {
				printf(" [t=%d T%d]", i, tl[i]);
			}
			prev = tl[i];
		}
	}
	printf("\n");
}

static void print_gantt(int c, int h, int *tl) {
	int i;
	int sc = 0;
	int cur;
	int start;
	int *sv;
	int *se;

	if (h <= 0) {
		return;
	}

	sv = (int *)malloc((h + 1) * sizeof(int));
	se = (int *)malloc((h + 1) * sizeof(int));
	if (sv == NULL || se == NULL) {
		free(sv);
		free(se);
		printf("Memory allocation failed\n");
		return;
	}

	cur = tl[0];
	for (i = 1; i <= h; i++) {
		if (i == h || tl[i] != cur) {
			sv[sc] = cur;
			se[sc] = i;
			sc++;
			if (i < h) {
				cur = tl[i];
			}
		}
	}

	printf("\nCPU %d Gantt Chart\n", c);
	start = 0;
	for (i = 0; i < sc; i++) {
		if (sv[i] == 0) {
			printf("|%d Idle %d|", start, se[i]);
		} else {
			printf("|%d T%d %d|", start, sv[i], se[i]);
		}
		if (i < sc - 1) {
			printf(" ");
		}
		start = se[i];
	}
	printf("\n");

	free(sv);
	free(se);
}

static void free_rms_resources(int *e,int *p,int *a,int *r,int *m,double *u,int *busy,double *su,int *cnt,int *ok,double *b,double *util,int *timeline) {
	free(e);
	free(p);
	free(a);
	free(r);
	free(m);
	free(u);
	free(busy);
	free(su);
	free(cnt);
	free(ok);
	free(b);
	free(util);
	free(timeline);
}

void run_rms(struct Process processes[], int num_processes, int num_cpus) {
	int n = num_processes;
	int i;
	int t;
	int h;
	int total_miss = 0;
	int total_busy = 0;
	int all_ok = 1;
	double util_all;

	int *busy;
	double *su;
	int *cnt;
	int *ok;
	double *b;
	double *util;

	int *e;
	int *p;
	int *a;
	int *r;
	int *m;
	double *u;
	int *timeline;

	if (n <= 0) {
		printf("Invalid number of tasks\n");
		return;
	}

	if (num_cpus <= 0) {
		printf("Invalid number of CPUs\n");
		return;
	}

	e = (int *)malloc(n * sizeof(int));
	p = (int *)malloc(n * sizeof(int));
	a = (int *)malloc(n * sizeof(int));
	r = (int *)malloc(n * sizeof(int));
	m = (int *)malloc(n * sizeof(int));
	u = (double *)malloc(n * sizeof(double));
	busy = (int *)calloc(num_cpus, sizeof(int));
	su = (double *)calloc(num_cpus, sizeof(double));
	cnt = (int *)calloc(num_cpus, sizeof(int));
	ok = (int *)calloc(num_cpus, sizeof(int));
	b = (double *)calloc(num_cpus, sizeof(double));
	util = (double *)calloc(num_cpus, sizeof(double));
	timeline = NULL;

	if (e == NULL || p == NULL || a == NULL || r == NULL || m == NULL || u == NULL ||
	    busy == NULL || su == NULL || cnt == NULL || ok == NULL || b == NULL || util == NULL) {
		printf("Memory allocation failed\n");
		free_rms_resources(e, p, a, r, m, u, busy, su, cnt, ok, b, util, timeline);
		return;
	}

	printf("\nRate-Monotonic Scheduling (RMS)\n");
	printf("Partitioned scheduling on %d CPUs\n", num_cpus);
	printf("Number of tasks: %d\n", n);

	for (i = 0; i < n; i++) {
		printf("Task %d execution time: ", i + 1);
		if (scanf("%d", &e[i]) != 1 || e[i] <= 0) {
			printf("Invalid execution time\n");
			free_rms_resources(e, p, a, r, m, u, busy, su, cnt, ok, b, util, timeline);
			return;
		}

		printf("Task %d period: ", i + 1);
		if (scanf("%d", &p[i]) != 1 || p[i] <= 0) {
			printf("Invalid period\n");
			free_rms_resources(e, p, a, r, m, u, busy, su, cnt, ok, b, util, timeline);
			return;
		}

		r[i] = 0;
		m[i] = 0;
		u[i] = (double)e[i] / (double)p[i];

		int best_cpu = 0;
		for (int c = 1; c < num_cpus; c++) {
			if (su[c] < su[best_cpu] - 1e-12) {
				best_cpu = c;
			} else if (su[c] <= su[best_cpu] + 1e-12 && cnt[c] < cnt[best_cpu]) {
				best_cpu = c;
			}
		}

		a[i] = best_cpu;
		su[best_cpu] += u[i];
		cnt[best_cpu]++;

		processes[i].pid = i + 1;
		processes[i].burst_time = e[i];
		processes[i].period = p[i];
	}

	printf("\nTask Table\n");
	printf("------------------------------------------------\n");
	printf("| Task ID | Execution Time | Period | CPU      |\n");
	printf("------------------------------------------------\n");
	for (i = 0; i < n; i++) {
		printf("| T%-6d| %-14d | %-6d | CPU %-4d|\n", i + 1, e[i], p[i], a[i]);
	}
	printf("------------------------------------------------\n");

	h = 1;
	for (i = 0; i < n; i++) {
		h = lcm_cap(h, p[i], MAX_TIME);
	}
	if (h <= 0) {
		h = 1;
	}

	timeline = (int *)calloc((size_t)num_cpus * (size_t)h, sizeof(int));
	if (timeline == NULL) {
		printf("Memory allocation failed\n");
		free_rms_resources(e, p, a, r, m, u, busy, su, cnt, ok, b, util, timeline);
		return;
	}

	for (t = 0; t < h; t++) {
		for (i = 0; i < n; i++) {
			if (t % p[i] == 0) {
				if (t > 0 && r[i] > 0) {
					m[i]++;
				}
				r[i] += e[i];
			}
		}

		for (int c = 0; c < num_cpus; c++) {
			int k = -1;
			for (i = 0; i < n; i++) {
				if (a[i] == c && r[i] > 0) {
					if (k == -1 || p[i] < p[k] || (p[i] == p[k] && i < k)) {
						k = i;
					}
				}
			}

			if (k == -1) {
				timeline[c * h + t] = 0;
			} else {
				timeline[c * h + t] = k + 1;
				r[k]--;
				busy[c]++;
			}
		}
	}

	for (i = 0; i < n; i++) {
		total_miss += m[i];
	}

	printf("\nScheduling Simulation Length: %d\n", h);
	for (int c = 0; c < num_cpus; c++) {
		print_timeline(c, h, &timeline[c * h]);
	}

	printf("\nGantt Charts\n");
	for (int c = 0; c < num_cpus; c++) {
		print_gantt(c, h, &timeline[c * h]);
	}

	for (int c = 0; c < num_cpus; c++) {
		util[c] = (double)busy[c] / (double)h;
		total_busy += busy[c];
	}
	util_all = (double)total_busy / (double)(num_cpus * h);

	for (int c = 0; c < num_cpus; c++) {
		b[c] = rms_bound(cnt[c]);
		ok[c] = (cnt[c] == 0) || (su[c] <= b[c] + 1e-12);
		if (!ok[c]) {
			all_ok = 0;
		}
	}

	printf("\nCPU Utilization\n");
	for (int c = 0; c < num_cpus; c++) {
		printf("CPU %d: %.2f%%\n", c, util[c] * 100.0);
	}
	printf("Overall: %.2f%%\n", util_all * 100.0);

	printf("\nRMS Schedulability Test\n");
	for (int c = 0; c < num_cpus; c++) {
		printf("CPU %d -> U = %.4f, Bound = %.4f, %s\n", c, su[c], b[c], ok[c] ? "Schedulable" : "Not Schedulable");
	}
	if (all_ok) {
		printf("Task set result: Schedulable under RMS partition\n");
	} else {
		printf("Task set result: Not schedulable under RMS bound\n");
	}

	printf("Deadline misses observed in simulation: %d\n", total_miss);

	free_rms_resources(e, p, a, r, m, u, busy, su, cnt, ok, b, util, timeline);
}
