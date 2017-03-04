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
int shared_num = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; 
pthread_mutex_t conditionLock = PTHREAD_MUTEX_INITIALIZER; 
pthread_cond_t num_is_zero = PTHREAD_COND_INITIALIZER;
pthread_cond_t num_is_one= PTHREAD_COND_INITIALIZER;
struct timespec start_t;
struct timespec stop_t;
unsigned long long timeTaken_t; //64 bit integer
unsigned long long totalTime_t = 0;
unsigned long long averageTime_t;
void* thread1_func();
void* thread2_func();
int i;
int k;
unsigned long long runs = 1000;

int main()
{
	// Set the CPU Affinity to only use core 1
	cpu_set_t  mask;
	CPU_ZERO(&mask);
	CPU_SET(1, &mask);
	int res;
	res = sched_setaffinity(0, sizeof(mask), &mask);

	unsigned long long averageTime;

	/******************************************* Thread Switch **************************************************/

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

	averageTime = totalTime_t/runs;

	printf("Average time for a process switch using CLOCK_MONOTONIC for %llu cycles: %llu ns\n", runs, averageTime );
}

// Thread 1 func, changes number from 1 to 0
void *thread1_func()
{
	for (i = 0; i < runs; i++)
	{
		//printf("thread1 in for %d for shared num %d\n",i, shared_num);
		pthread_mutex_lock(&conditionLock);
		while( shared_num == 0)
		{
			//printf("thread1 in loop has shared num %d\n", shared_num);
			// start the time here since we know the other thread will run now that it is sent the condition signal in the next instruction
			clock_gettime(CLOCK_MONOTONIC, &start_t);
			// send the condition signal to the thread waiting on it
			pthread_cond_signal(&num_is_zero);
			// will be blocked here waiting for the wake up signal to be sent (when the shared num is changed to a 1)
			pthread_cond_wait(&num_is_one, &conditionLock);
		}
		pthread_mutex_unlock(&conditionLock);

		// shared_num is 1, change it back to 0
		// first lock it so that no other process/thread can access the variable
		pthread_mutex_lock(&lock);
		shared_num = 0;
		// unlock the mutex so it can be used by other threads
		pthread_mutex_unlock(&lock);
	}
}

// Thread 2 func changes number from 0 to 1
void *thread2_func()
{
	// open a file to write the times in to plot on excel
	FILE *f = fopen("threadSwitchTime.txt", "w");
	for (k = 0; k < runs; k++)
	{
		//printf("thread2 in for %d has shared num %d\n", k, shared_num);
		pthread_mutex_lock(&conditionLock);
		while( shared_num == 1)
		{
			//printf("thread2 in loop has shared num %d\n", shared_num);
			// send the condition signal to the thread waiting on it
			pthread_cond_signal(&num_is_one);
			// the thread will be blocked when shared_num is not 0 and will wait until the condition signal is sent back before executing
			pthread_cond_wait(&num_is_zero, &conditionLock);
			// get the stop time here since this means that the shared_num is zero and we can change it back to 1 now
			// do not need to use IPC to access the start time since threads share global variables
			clock_gettime(CLOCK_MONOTONIC, &stop_t);
			timeTaken_t = timespecDiff(&stop_t,&start_t);
			fprintf(f, "%llu\n", timeTaken_t);
			totalTime_t += timeTaken_t;
		}
		pthread_mutex_unlock(&conditionLock);

		// shared_num is 0, change it back to 1
		// first lock it so that no other process/thread can access the variable
		pthread_mutex_lock(&lock);
		shared_num = 1;
		// unlock the mutex so it can be used by other threads
		pthread_mutex_unlock(&lock);

		// Check if its the last run, if it is send the signal to wake up the first thread or it will be sleeping forever
		if (k == runs - 1)
		{
			fclose(f);
			pthread_cond_signal(&num_is_one);
			pthread_exit(NULL);
		}
	}
}


