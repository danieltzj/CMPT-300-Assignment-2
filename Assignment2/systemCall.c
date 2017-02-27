#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>

unsigned long long timespecDiff(struct timespec *timeA_p, struct timespec *timeB_p)
{
  return ((timeA_p->tv_sec * 1000000000) + timeA_p->tv_nsec) -
           ((timeB_p->tv_sec * 1000000000) + timeB_p->tv_nsec);
}

int main()
{
	pid_t pid;
	struct timespec start;
	struct timespec stop;
	unsigned long long result; //64 bit integer

	clock_gettime(CLOCK_MONOTONIC, &start);
	getpid();
	clock_gettime(CLOCK_MONOTONIC, &stop);

	// Use the function to get the difference between the two times
	result=timespecDiff(&stop,&start);

	printf("CLOCK_REALTIME Measured: %llu\n",result);
}