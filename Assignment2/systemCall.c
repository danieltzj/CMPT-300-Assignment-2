#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>

// Subtraction function taken from instructors sample code @ http://www.sfu.ca/~rws1/cmpt-300/assignments/hr-timer.c
unsigned long long timespecDiff(struct timespec *timeA_p, struct timespec *timeB_p)
{
  return ((timeA_p->tv_sec * 1000000000) + timeA_p->tv_nsec) -
           ((timeB_p->tv_sec * 1000000000) + timeB_p->tv_nsec);
}

int main()
{
	unsigned long long iterations = 100;
	pid_t pid;
	struct timespec start;
	struct timespec stop;
	unsigned long long result; //64 bit integer
	unsigned long long totalTime = 0;
	unsigned long long averageTime;

	int i;
	for (i = 0; i < iterations; i++)
	{
		clock_gettime(CLOCK_MONOTONIC, &start);
		pid = getpid();
		clock_gettime(CLOCK_MONOTONIC, &stop);

		// Use the function to get the difference between the two times
		result=timespecDiff(&stop,&start);
		printf("%llu\n",result );

		totalTime = totalTime + result;
	}
	averageTime = totalTime/iterations;

	printf("Average time for a system call using CLOCK_MONOTONIC for %llu cycles: %llu ns\n", iterations, averageTime);
}