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
	for (i = 0; i < sc; i++) {
		if (sv[i] == 0) {
			printf("| Idle ");
		} else {
			printf("| T%d ", sv[i]);
		}
	}
	printf("|\n");
	printf("0");
	for (i = 0; i < sc; i++) {
		printf("%7d", se[i]);
	}
	printf("\n");

	free(sv);
	free(se);
}

void run_rms(struct Process processes[], int num_processes, int num_cpus) {
	int n = num_processes;
	int i;
	int t;
	int h;
	int busy[2] = {0, 0};
	double su[2] = {0.0, 0.0};
	int cnt[2] = {0, 0};
	int total_miss = 0;
	int ok0;
	int ok1;
	double b0;
	double b1;
	double util0;
	double util1;
	double util_all;

	int *e;
	int *p;
	int *a;
	int *r;
	int *m;
	double *u;
	int *tl0;
	int *tl1;

	(void)num_cpus;

	if (n <= 0) {
		printf("Invalid number of tasks\n");
		return;
	}

	e = (int *)malloc(n * sizeof(int));
	p = (int *)malloc(n * sizeof(int));
	a = (int *)malloc(n * sizeof(int));
	r = (int *)malloc(n * sizeof(int));
	m = (int *)malloc(n * sizeof(int));
	u = (double *)malloc(n * sizeof(double));

	if (e == NULL || p == NULL || a == NULL || r == NULL || m == NULL || u == NULL) {
		printf("Memory allocation failed\n");
		free(e);
		free(p);
		free(a);
		free(r);
		free(m);
		free(u);
		return;
	}

	printf("\nRate-Monotonic Scheduling (RMS)\n");
	printf("Partitioned scheduling on 2 CPUs\n");
	printf("Number of tasks: %d\n", n);

	for (i = 0; i < n; i++) {
		printf("Task %d execution time: ", i + 1);
		if (scanf("%d", &e[i]) != 1 || e[i] <= 0) {
			printf("Invalid execution time\n");
			free(e);
			free(p);
			free(a);
			free(r);
			free(m);
			free(u);
			return;
		}

		printf("Task %d period: ", i + 1);
		if (scanf("%d", &p[i]) != 1 || p[i] <= 0) {
			printf("Invalid period\n");
			free(e);
			free(p);
			free(a);
			free(r);
			free(m);
			free(u);
			return;
		}

		r[i] = 0;
		m[i] = 0;
		u[i] = (double)e[i] / (double)p[i];

		if (su[0] <= su[1]) {
			a[i] = 0;
			su[0] += u[i];
			cnt[0]++;
		} else {
			a[i] = 1;
			su[1] += u[i];
			cnt[1]++;
		}

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

	tl0 = (int *)malloc(h * sizeof(int));
	tl1 = (int *)malloc(h * sizeof(int));
	if (tl0 == NULL || tl1 == NULL) {
		printf("Memory allocation failed\n");
		free(e);
		free(p);
		free(a);
		free(r);
		free(m);
		free(u);
		free(tl0);
		free(tl1);
		return;
	}

	for (t = 0; t < h; t++) {
		tl0[t] = 0;
		tl1[t] = 0;
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

		for (int c = 0; c < 2; c++) {
			int k = -1;
			for (i = 0; i < n; i++) {
				if (a[i] == c && r[i] > 0) {
					if (k == -1 || p[i] < p[k] || (p[i] == p[k] && i < k)) {
						k = i;
					}
				}
			}

			if (c == 0) {
				if (k == -1) {
					tl0[t] = 0;
				} else {
					tl0[t] = k + 1;
					r[k]--;
					busy[0]++;
				}
			} else {
				if (k == -1) {
					tl1[t] = 0;
				} else {
					tl1[t] = k + 1;
					r[k]--;
					busy[1]++;
				}
			}
		}
	}

	for (i = 0; i < n; i++) {
		total_miss += m[i];
	}

	printf("\nScheduling Simulation Length: %d\n", h);
	print_timeline(0, h, tl0);
	print_timeline(1, h, tl1);

	printf("\nGantt Charts\n");
	print_gantt(0, h, tl0);
	print_gantt(1, h, tl1);

	util0 = (double)busy[0] / (double)h;
	util1 = (double)busy[1] / (double)h;
	util_all = (double)(busy[0] + busy[1]) / (double)(2 * h);

	b0 = rms_bound(cnt[0]);
	b1 = rms_bound(cnt[1]);
	ok0 = (cnt[0] == 0) || (su[0] <= b0 + 1e-12);
	ok1 = (cnt[1] == 0) || (su[1] <= b1 + 1e-12);

	printf("\nCPU Utilization\n");
	printf("CPU 0: %.2f%%\n", util0 * 100.0);
	printf("CPU 1: %.2f%%\n", util1 * 100.0);
	printf("Overall: %.2f%%\n", util_all * 100.0);

	printf("\nRMS Schedulability Test\n");
	printf("CPU 0 -> U = %.4f, Bound = %.4f, %s\n", su[0], b0, ok0 ? "Schedulable" : "Not Schedulable");
	printf("CPU 1 -> U = %.4f, Bound = %.4f, %s\n", su[1], b1, ok1 ? "Schedulable" : "Not Schedulable");
	if (ok0 && ok1) {
		printf("Task set result: Schedulable under RMS partition\n");
	} else {
		printf("Task set result: Not schedulable under RMS bound\n");
	}

	printf("Deadline misses observed in simulation: %d\n", total_miss);

	free(e);
	free(p);
	free(a);
	free(r);
	free(m);
	free(u);
	free(tl0);
	free(tl1);
}
