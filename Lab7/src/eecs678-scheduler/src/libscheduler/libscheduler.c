/** @file libscheduler.c
 */

#include <stdio.h>
#include <stdlib.h>

#include "libscheduler.h"
#include "../libpriqueue/libpriqueue.h"

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

static scheme_t scheduling_scheme;
static int core_count;
static job_t **all_jobs;
static size_t job_count;
static size_t job_capacity;
static job_t **core_jobs;
static priqueue_t ready_queue;
static long long total_waiting_time;
static long long total_turnaround_time;
static long long total_response_time;
static size_t completed_jobs;

static void scheduler_out_of_memory(void)
{
	fprintf(stderr, "Out of memory.\n");
	exit(EXIT_FAILURE);
}

static job_t *create_job(int job_number, int arrival_time, int running_time, int priority)
{
	job_t *job;
	if (job_count == job_capacity)	{
		size_t new_capacity = job_capacity == 0 ? 16 : job_capacity * 2;
		job_t **new_jobs;
		if (new_capacity < job_capacity ||
			new_capacity > (size_t)-1 / sizeof(*all_jobs))
			scheduler_out_of_memory();
		new_jobs = realloc(all_jobs, new_capacity * sizeof(*all_jobs));
		if (new_jobs == NULL)
			scheduler_out_of_memory();
		all_jobs = new_jobs;
		job_capacity = new_capacity;
	}

	job = malloc(sizeof(*job));
	if (job == NULL)
		scheduler_out_of_memory();
	job->job_number = job_number;
	job->arrival_time = arrival_time;
	job->running_time = running_time;
	job->remaining_time = running_time;
	job->priority = priority;
	job->first_run_time = -1;
	job->last_start_time = -1;
	job->core_id = -1;
	all_jobs[job_count++] = job;
	return job;
}

static int compare_jobs(const void *left, const void *right)
{
	const job_t *a = (const job_t *)left;
	const job_t *b = (const job_t *)right;
	int a_value;
	int b_value;

	switch (scheduling_scheme)	{
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

	if (scheduling_scheme != RR)	{
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

static void account_running_jobs(int time)
{
	int core_id;
	for (core_id = 0; core_id < core_count; core_id++)	{
		job_t *job = core_jobs[core_id];
		int elapsed;
		if (job == NULL || job->last_start_time < 0)
			continue;

		elapsed = time - job->last_start_time;
		if (elapsed <= 0)
			continue;
		if (job->first_run_time < 0)
			job->first_run_time = job->last_start_time;
		job->remaining_time -= elapsed;
		if (job->remaining_time < 0)
			job->remaining_time = 0;
		job->last_start_time = time;
	}
}

static int select_idle_core(void)
{
	int core_id;
	for (core_id = 0; core_id < core_count; core_id++)	{
		if (core_jobs[core_id] == NULL)
			return core_id;
	}
	return -1;
}

static void dispatch_job(job_t *job, int core_id, int time)
{
	job->core_id = core_id;
	job->last_start_time = time;
	core_jobs[core_id] = job;
}

static int worst_running_core_for(job_t *job)
{
	int core_id;
	int worst_core = -1;
	for (core_id = 0; core_id < core_count; core_id++)	{
		job_t *running = core_jobs[core_id];
		if (running == NULL || compare_jobs(running, job) <= 0)
			continue;
		if (worst_core < 0 || compare_jobs(running, core_jobs[worst_core]) > 0)
			worst_core = core_id;
	}
	return worst_core;
}

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
		scheduler_out_of_memory();
	for (core_id = 0; core_id < cores; core_id++)
		core_jobs[core_id] = NULL;

	priqueue_init(&ready_queue, compare_jobs);
}

int scheduler_new_job(int job_number, int time, int running_time, int priority)
{
	job_t *job;
	int core_id;

	account_running_jobs(time);
	job = create_job(job_number, time, running_time, priority);

	core_id = select_idle_core();
	if (core_id >= 0)	{
		dispatch_job(job, core_id, time);
		return core_id;
	}

	if (scheduling_scheme == PSJF || scheduling_scheme == PPRI)	{
		core_id = worst_running_core_for(job);
		if (core_id >= 0)	{
			job_t *preempted = core_jobs[core_id];
			preempted->core_id = -1;
			preempted->last_start_time = -1;
			priqueue_offer(&ready_queue, preempted);
			dispatch_job(job, core_id, time);
			return core_id;
		}
	}

	priqueue_offer(&ready_queue, job);
	return -1;
}

int scheduler_job_finished(int core_id, int job_number, int time)
{
	job_t *job;
	job_t *next;

	if (core_id < 0 || core_id >= core_count)
		return -1;
	account_running_jobs(time);
	job = core_jobs[core_id];
	if (job == NULL || job->job_number != job_number)
		return -1;

	job->remaining_time = 0;
	job->core_id = -1;
	job->last_start_time = -1;
	core_jobs[core_id] = NULL;

	total_turnaround_time += (long long)(time - job->arrival_time);
	total_waiting_time +=
		(long long)(time - job->arrival_time - job->running_time);
	if (job->first_run_time >= 0)
		total_response_time +=
			(long long)(job->first_run_time - job->arrival_time);
	completed_jobs++;

	next = (job_t *)priqueue_poll(&ready_queue);
	if (next == NULL)
		return -1;
	dispatch_job(next, core_id, time);
	return next->job_number;
}

int scheduler_quantum_expired(int core_id, int time)
{
	job_t *job;
	job_t *next;

	if (core_id < 0 || core_id >= core_count)
		return -1;
	account_running_jobs(time);
	job = core_jobs[core_id];
	if (job == NULL)
		return -1;

	job->core_id = -1;
	job->last_start_time = -1;
	core_jobs[core_id] = NULL;
	priqueue_offer(&ready_queue, job);

	next = (job_t *)priqueue_poll(&ready_queue);
	if (next == NULL)
		return -1;
	dispatch_job(next, core_id, time);
	return next->job_number;
}

float scheduler_average_waiting_time()
{
	if (completed_jobs == 0)
		return 0.0f;
	return (float)total_waiting_time / (float)completed_jobs;
}

float scheduler_average_turnaround_time()
{
	if (completed_jobs == 0)
		return 0.0f;
	return (float)total_turnaround_time / (float)completed_jobs;
}

float scheduler_average_response_time()
{
	if (completed_jobs == 0)
		return 0.0f;
	return (float)total_response_time / (float)completed_jobs;
}

void scheduler_clean_up()
{
	size_t i;
	priqueue_destroy(&ready_queue);
	for (i = 0; i < job_count; i++)
		free(all_jobs[i]);
	free(all_jobs);
	free(core_jobs);
	all_jobs = NULL;
	core_jobs = NULL;
	job_count = 0;
	job_capacity = 0;
	core_count = 0;
	completed_jobs = 0;
}

void scheduler_show_queue()
{
	int core_id;
	int index;
	int printed = 0;

	for (core_id = 0; core_id < core_count; core_id++)	{
		job_t *job = core_jobs[core_id];
		if (job == NULL)
			continue;
		if (printed)
			printf(" ");
		if (scheduling_scheme == RR)
			printf("%d(-1)", job->job_number);
		else
			printf("%d(%d)", job->job_number, job->priority);
		printed = 1;
	}

	for (index = 0; index < priqueue_size(&ready_queue); index++)	{
		job_t *job = (job_t *)priqueue_at(&ready_queue, index);
		if (job == NULL)
			continue;
		if (printed)
			printf(" ");
		if (scheduling_scheme == RR)
			printf("%d(-1)", job->job_number);
		else
			printf("%d(%d)", job->job_number, job->priority);
		printed = 1;
	}
}
