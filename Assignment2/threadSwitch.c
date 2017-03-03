#define _GNU_SOURCE

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sched.h>
#include <pthread.h>

// Subtraction function taken from instructors sample code @ http://www.sfu.ca/~rws1/cmpt-300/assignments/hr-timer.c
unsigned long long timespecDiff(struct timespec *timeA_p, struct timespec *timeB_p)
{
  return ((timeA_p->tv_sec * 1000000000) + timeA_p->tv_nsec) -
           ((timeB_p->tv_sec * 1000000000) + timeB_p->tv_nsec);
}

// volatile variables that can be changed at any time, declared to use in the thread_funcs
volatile int shared_num = 0;
pthread_mutex_t lock;
pthread_cond_t num_is_zero;
pthread_cond_t num_is_one;
struct timespec start_t;
struct timespec stop_t;
unsigned long long timeTaken_t; //64 bit integer
unsigned long long totalTime_t = 0;
unsigned long long averageTime_t;

// Thread 1 func, checks if the lock is free and
void *thread1_func()
{
	// wait for the shared_num to become 1 before executing
	pthread_mutex_lock(&lock);
	while( shared_num != 1)
	{
		// will be blocked here waiting for the wake up signal to be sent
		pthread_cond_wait(&num_is_one, &lock);
	}
	// get the stop time here since this means that the shared_num is one and we can change it back to 0 now
	// do not need to use IPC to access the start time since threads share global variables
	clock_gettime(CLOCK_MONOTONIC, &stop_t);
	timeTaken_t = timespecDiff(&stop_t,&start_t);
	printf("%llu\n", timeTaken_t);
	totalTime_t += timeTaken_t;
	pthread_mutex_unlock(&lock);

	// shared_num is 1, change it back to 0
	// first lock it so that no other process/thread can access the variable
	pthread_mutex_lock(&lock);
	shared_num = 0;
	// send the condition signal to the thread waiting on it
	pthread_cond_signal(&num_is_zero);
	// unlock the mutex so it can be used by other threads
	pthread_mutex_unlock(&lock);
	
}

void *thread2_func()
{
	// wait for the shared num to be 0
	pthread_mutex_lock(&lock);
	while( shared_num != 0)
	{
		// the thread will be blocked when shared_num is not 0 and will wait until the condition signal is sent back before executing
		pthread_cond_wait(&num_is_zero, &lock);
	}
	pthread_mutex_unlock(&lock);

	// shared_num is 0, change it back to 1
	// first lock it so that no other process/thread can access the variable
	pthread_mutex_lock(&lock);
	shared_num = 1;
	// send the condition signal to the thread waiting on it
	pthread_cond_signal(&num_is_one);
	// unlock the mutex so it can be used by other threads
	pthread_mutex_unlock(&lock);
	// start the time here since we know the other thread will run now that it is sent the condition signal
	clock_gettime(CLOCK_MONOTONIC, &start_t);
}

int main()
{
	// Set the CPU Affinity to only use core 1
	cpu_set_t  mask;
	CPU_ZERO(&mask);
	CPU_SET(1, &mask);
	int res;
	res = sched_setaffinity(0, sizeof(mask), &mask);

	unsigned long long runs = 100;
	struct timespec start;
	struct timespec stop;
	unsigned long long result; //64 bit integer
	unsigned long long totalTime = 0;
	unsigned long long averageTime;

	/******************************************* Thread Switch **************************************************/
	int i;
	for (i = 0; i < runs; i++)
	{
		int thread_result1, thread_result2;

		pthread_t thread1, thread2;

		// Code referenced from sample provided by the prof @
		// http://199.60.17.135/cmpt-300/wp-content/uploads/2016/09/lock-example.c
		// first create the two threads and pass in their corresponding functions listed in the beginning of the code
		// thread1_func for thread1
		// thread2_func for thread2
		if (thread_result1 = pthread_create( &thread1, NULL, &thread1_func, NULL))
		{
			printf("Thread creation failed: %d\n", thread_result1);
		}
		if (thread_result2 = pthread_create( &thread2, NULL, &thread2_func, NULL))
		{
			printf("Thread creation failed: %d\n", thread_result2);
		}

		// wait for the threads to finish executing
		pthread_join(thread1, NULL);
		pthread_join(thread2, NULL);
	}

	averageTime = totalTime_t/runs;

	printf("Average time for a process switch using CLOCK_MONOTONIC for %llu cycles: %llu ns\n", runs, averageTime );
}
