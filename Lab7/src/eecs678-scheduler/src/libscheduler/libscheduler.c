/** @file libscheduler.c
 */

#include <stdio.h>
#include <stdlib.h>

#include "libscheduler.h"
#include "../libpriqueue/libpriqueue.h"

/*
 * Each job is tracked as it moves through the simulator.
 * The scheduler stores the original arrival and total runtime, but also tracks
 * how much work remains, when the job first ran, and which core is currently
 * executing it
 */
typedef struct _job_t
{
	int job_number;
	int arrival_time;
	int running_time;
	int remaining_time;
	int priority;
	int first_run_time;
	int last_start_time;
	int core_id;
} job_t;

static scheme_t scheduling_scheme;		// Active scheduling policy (FCFS, SJF, RR, etc.)
static int core_count;					// Number of available processor cores
static job_t **all_jobs;				// All jobs ever created
static size_t job_count;				// Number of jobs currently tracked
static size_t job_capacity;				// Capacity of all_jobs array
static job_t **core_jobs;				// One job per core, or NULL if idle
static priqueue_t ready_queue;			// Jobs waiting for a core
static long long total_waiting_time;	// Sum of waiting times for completed jobs
static long long total_turnaround_time; // Sum of turnaround times for completed jobs
static long long total_response_time;	// Sum of response times for completed jobs
static size_t completed_jobs;			// Count of finished jobs

static void scheduler_out_of_memory(void)
{
	fprintf(stderr, "Out of memory.\n");
	exit(EXIT_FAILURE);
}

static job_t *create_job(int job_number, int arrival_time, int running_time, int priority)
{
	job_t *job;

	// If the array is full grow it
	if (job_count == job_capacity)
	{
		size_t new_capacity;
		job_t **new_jobs;

		if (job_capacity == 0)
		{
			new_capacity = 16;
		}
		else
		{
			new_capacity = job_capacity * 2;
		}

		if (new_capacity < job_capacity ||
			new_capacity > (size_t)-1 / sizeof(*all_jobs))
		{
			scheduler_out_of_memory();
		}

		new_jobs = realloc(all_jobs, new_capacity * sizeof(*all_jobs));

		if (new_jobs == NULL)
		{
			scheduler_out_of_memory();
		}

		all_jobs = new_jobs;
		job_capacity = new_capacity;
	}

	job = malloc(sizeof(*job));

	if (job == NULL)
	{
		scheduler_out_of_memory();
	}

	job->job_number = job_number;
	job->arrival_time = arrival_time;
	job->running_time = running_time;
	job->remaining_time = running_time; // Remaining work decreases as it runs
	job->priority = priority;
	job->first_run_time = -1; // Set to the first time it begins execution
	job->last_start_time = -1;
	job->core_id = -1;
	all_jobs[job_count++] = job;
	return job;
}

/*
 * This comparison function defines the priority order used by the ready queue.
 * Different scheduling policies sort jobs by different keys:
 * - FCFS/RR: arrival time (oldest first)
 * - SJF: total run time (shortest first)
 * - PSJF: remaining time (shortest remaining first)
 * - PRI/PPRI: priority number (smaller is higher priority)
 *
 * When two jobs are otherwise tied, we fall back to earlier arrival and then job
 * number so the queue ordering remains deterministic
 */
static int compare_jobs(const void *left, const void *right)
{
	const job_t *a = (const job_t *)left;
	const job_t *b = (const job_t *)right;
	int a_value;
	int b_value;

	switch (scheduling_scheme)
	{
	case SJF:
		a_value = a->running_time;
		b_value = b->running_time;
		break;
	case PSJF:
		a_value = a->remaining_time;
		b_value = b->remaining_time;
		break;
	case PRI:
	case PPRI:
		a_value = a->priority;
		b_value = b->priority;
		break;
	case FCFS:
	case RR:
	default:
		a_value = a->arrival_time;
		b_value = b->arrival_time;
		break;
	}

	if (scheduling_scheme != RR)
	{
		if (a_value < b_value)
			return -1;
		if (a_value > b_value)
			return 1;
		if (a->arrival_time < b->arrival_time)
			return -1;
		if (a->arrival_time > b->arrival_time)
			return 1;
		if (a->job_number < b->job_number)
			return -1;
		if (a->job_number > b->job_number)
			return 1;
	}
	return 0;
}

/*
 * As time advances, all jobs currently running on each core are partially
 * completed. This hoelper advances each active job by the elapsed interval and
 * updates the remaining time accordingly
 */
static void account_running_jobs(int time)
{
	int core_id;
	for (core_id = 0; core_id < core_count; core_id++)
	{
		job_t *job = core_jobs[core_id];
		int elapsed;

		if (job == NULL || job->last_start_time < 0)
		{
			continue;
		}

		// Advance the currently running job by the time since it last started
		elapsed = time - job->last_start_time;

		if (elapsed <= 0)
		{
			continue;
		}

		if (job->first_run_time < 0)
		{
			job->first_run_time = job->last_start_time;
		}

		// Update job's remaining time
		job->remaining_time -= elapsed;
		if (job->remaining_time < 0)
		{
			job->remaining_time = 0;
		}

		job->last_start_time = time;
	}
}

/*
 * A scheduler with multiple cores must choose an available processor before it
 * can place a new job. If some core is free, we take it immediately
 */
static int select_idle_core(void)
{
	int core_id;
	for (core_id = 0; core_id < core_count; core_id++)
	{
		if (core_jobs[core_id] == NULL)
		{
			return core_id; // Found an available processor
		}
	}
	return -1;
}

/*
 * Dispatching means the job begins executing on a specific core at a given time.
 * The job remembers which core it is on and when it most recently started
 */
static void dispatch_job(job_t *job, int core_id, int time)
{
	job->core_id = core_id;
	job->last_start_time = time;
	core_jobs[core_id] = job;
}

/*
 * For preemptive policies, a newly arrived job may need to replace a currently
 * running one. We look for the "worst" currently running job according to the
 * same compare_jobs ordering, which lets the scheduler evict the least valuable
 * job in favor of the new higher-priority one
 */
static int worst_running_core_for(job_t *job)
{
	int core_id;
	int worst_core = -1;
	for (core_id = 0; core_id < core_count; core_id++)
	{
		job_t *running = core_jobs[core_id];
		if (running == NULL || compare_jobs(running, job) <= 0)
		{
			continue;
		}

		// Pick the running job that is least preferred according to the policy
		if (worst_core < 0 || compare_jobs(running, core_jobs[worst_core]) > 0)
		{
			worst_core = core_id;
		}
	}
	return worst_core;
}

/*
 * startup initializes the scheduler's global state and creates the per-core
 * array. The ready queue is configured with compare_jobs so it always orders
 * jobs according to the active policy
 */
void scheduler_start_up(int cores, scheme_t scheme)
{
	int core_id;
	scheduling_scheme = scheme;
	core_count = cores;
	all_jobs = NULL;
	job_count = 0;
	job_capacity = 0;
	completed_jobs = 0;
	total_waiting_time = 0;
	total_turnaround_time = 0;
	total_response_time = 0;

	core_jobs = malloc((size_t)cores * sizeof(*core_jobs));
	if (core_jobs == NULL)
	{
		scheduler_out_of_memory();
	}

	for (core_id = 0; core_id < cores; core_id++)
	{
		core_jobs[core_id] = NULL;
	}

	priqueue_init(&ready_queue, compare_jobs);
}

/*
 * When a new job arrives, we first account for all work that has already been
 * done on each core since the last event. Then we attempt to assign it to an
 * idle core. If no cores are free, the behavior depends on the policy:
 *  - PSJF/PPRI: preempt the currently running job that is worst according to the
 *    ordering and swap the new job in
 *  - other policies: simply place the job in the ready queue
 */
int scheduler_new_job(int job_number, int time, int running_time, int priority)
{
	job_t *job;
	int core_id;

	account_running_jobs(time);
	job = create_job(job_number, time, running_time, priority);

	core_id = select_idle_core();
	if (core_id >= 0)
	{
		dispatch_job(job, core_id, time);
		return core_id;
	}

	if (scheduling_scheme == PSJF || scheduling_scheme == PPRI)
	{
		core_id = worst_running_core_for(job);
		if (core_id >= 0)
		{
			job_t *preempted = core_jobs[core_id];
			preempted->core_id = -1;
			preempted->last_start_time = -1;
			priqueue_offer(&ready_queue, preempted); // Resume later in queue
			dispatch_job(job, core_id, time);
			return core_id;
		}
	}

	priqueue_offer(&ready_queue, job); // No core free, so wait in the queue
	return -1;
}

/*
 * A job is considered complete when the simulator reports it finished on a
 * specific core. We finalize the statistics and then immediately schedule the
 * next job from the ready queue on that core
 */
int scheduler_job_finished(int core_id, int job_number, int time)
{
	job_t *job;
	job_t *next;

	if (core_id < 0 || core_id >= core_count)
	{
		return -1;
	}

	account_running_jobs(time);
	job = core_jobs[core_id];

	if (job == NULL || job->job_number != job_number)
	{
		return -1;
	}

	job->remaining_time = 0;
	job->core_id = -1;
	job->last_start_time = -1;
	core_jobs[core_id] = NULL;

	// Record the final stats for this completed job
	total_turnaround_time += (long long)(time - job->arrival_time);
	total_waiting_time +=
		(long long)(time - job->arrival_time - job->running_time);
	if (job->first_run_time >= 0)
	{
		total_response_time +=
			(long long)(job->first_run_time - job->arrival_time);
	}

	completed_jobs++;

	next = (job_t *)priqueue_poll(&ready_queue);

	if (next == NULL)
	{
		return -1;
	}

	dispatch_job(next, core_id, time);
	return next->job_number;
}

/*
 * Round-robin uses a time slice. When the quantum expires, the job is removed
 * from its current core, put back in the ready queue, and the next eligible job
 * is dispatched to the now-free core
 */
int scheduler_quantum_expired(int core_id, int time)
{
	job_t *job;
	job_t *next;

	if (core_id < 0 || core_id >= core_count)
	{
		return -1;
	}

	account_running_jobs(time);
	job = core_jobs[core_id];
	if (job == NULL)
	{
		return -1;
	}

	job->core_id = -1;
	job->last_start_time = -1;
	core_jobs[core_id] = NULL;
	priqueue_offer(&ready_queue, job); // RR gives each job a turn and then requeues it

	next = (job_t *)priqueue_poll(&ready_queue);
	if (next == NULL)
	{
		return -1;
	}

	dispatch_job(next, core_id, time);
	return next->job_number;
}

/*
 * These functions compute the average values reported by the simulator.
 * They divide the accumulated totals by the number of completed jobs so the
 * caller can compare scheduling policies across runs
 */
float scheduler_average_waiting_time()
{
	if (completed_jobs == 0)
	{
		return 0.0f;
	}

	return (float)total_waiting_time / (float)completed_jobs;
}

float scheduler_average_turnaround_time()
{
	if (completed_jobs == 0)
	{
		return 0.0f;
	}

	return (float)total_turnaround_time / (float)completed_jobs;
}

float scheduler_average_response_time()
{
	if (completed_jobs == 0)
	{
		return 0.0f;
	}

	return (float)total_response_time / (float)completed_jobs;
}

/*
 * Cleanup tears down the scheduler state and frees any dynamically allocated
 * jobs so a future run can start fresh without leaking memory
 */
void scheduler_clean_up()
{
	size_t i;
	priqueue_destroy(&ready_queue);
	for (i = 0; i < job_count; i++)
	{
		free(all_jobs[i]);
	}

	free(all_jobs);
	free(core_jobs);
	all_jobs = NULL;
	core_jobs = NULL;
	job_count = 0;
	job_capacity = 0;
	core_count = 0;
	completed_jobs = 0;
}

/*
 * debug helper that shows the jobs currently resident on cores and in the ready
 * queue. It prints the job number and priority for non-round-robin policies and
 * uses -1 as the time quantum marker for RR
 */
void scheduler_show_queue()
{
	int core_id;
	int index;
	int printed = 0;

	for (core_id = 0; core_id < core_count; core_id++)
	{
		job_t *job = core_jobs[core_id];
		if (job == NULL)
		{
			continue;
		}

		if (printed)
		{
			printf(" ");
		}

		if (scheduling_scheme == RR)
		{
			printf("%d(-1)", job->job_number);
		}
		else
		{
			printf("%d(%d)", job->job_number, job->priority);
		}

		printed = 1;
	}

	for (index = 0; index < priqueue_size(&ready_queue); index++)
	{
		job_t *job = (job_t *)priqueue_at(&ready_queue, index);
		if (job == NULL)
		{
			continue;
		}

		if (printed)
		{
			printf(" ");
		}

		if (scheduling_scheme == RR)
		{
			printf("%d(-1)", job->job_number);
		}	
		else
		{
			printf("%d(%d)", job->job_number, job->priority);
		}
			
		printed = 1;
	}
}
