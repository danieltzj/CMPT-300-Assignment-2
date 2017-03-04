#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

pthread_mutex_t numberMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t conditionMutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t condition0 = PTHREAD_COND_INITIALIZER; // Condition: variable is zero
pthread_cond_t condition1 = PTHREAD_COND_INITIALIZER; // Condition: variable is one

void *oneToZero();
void *zeroToOne();
int number = 0;
struct timespec start; // Start time
struct timespec stop; // End time

unsigned long long result; //64 bit integer

// Takes the difference between two times
unsigned long long timespecDiff(struct timespec *timeA_p, struct timespec *timeB_p)
{
	// First takes the times and converts the seconds to nanoseconds, then adds to the existing nanoseconds
	// Then takes the difference between to times
  return ((timeA_p->tv_sec * 1000000000) + timeA_p->tv_nsec) -
           ((timeB_p->tv_sec * 1000000000) + timeB_p->tv_nsec);
}

// Set threads to run on CPU2 only
void setAffinity()
{
	int result;
	cpu_set_t mask;
	CPU_ZERO(&mask);
	CPU_SET(2, &mask);
	result = sched_setaffinity(0, sizeof(mask), &mask);
	printf ("%d\n",result);
}

int main()
{
	setAffinity();
	pthread_t threadA, threadB;

	pthread_create(&threadA, NULL, &oneToZero, NULL);
	pthread_create(&threadB, NULL, &zeroToOne, NULL);

	pthread_join(threadA, NULL);
	pthread_join(threadB, NULL);

	return 0;
}

// Waits for number to become 1, then changes it to 0
void *oneToZero()
{
	while(1)
	{
		pthread_mutex_lock(&conditionMutex);
		while(number == 0)
		{
			clock_gettime(CLOCK_MONOTONIC, &start);
			pthread_cond_signal(&condition0); // Signal Thread B to unblock since number is 0
			pthread_cond_wait(&condition1, &conditionMutex); // Blocks until number is 1
		}
		pthread_mutex_unlock(&conditionMutex);

		pthread_mutex_lock(&numberMutex);
		number = 0;
		pthread_mutex_unlock(&numberMutex);
	}
}

// Waits for number to become 0, then changes it to 1
void *zeroToOne()
{
	while(1)
	{
		pthread_mutex_lock(&conditionMutex);
		while(number == 1)
		{
			pthread_cond_signal(&condition1); // Signal Thread A to unblock since number is 1
			pthread_cond_wait(&condition0, &conditionMutex); // Blocks until number is 0
			clock_gettime(CLOCK_MONOTONIC, &stop);
			result=timespecDiff(&stop,&start);
			printf("%llu\n", result);
		}
		pthread_mutex_unlock(&conditionMutex);

		pthread_mutex_lock(&numberMutex);
		number = 1;
		pthread_mutex_unlock(&numberMutex);
	}
}