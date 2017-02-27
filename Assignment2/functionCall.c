#include <stdio.h>
#include <stdint.h>
#include <time.h>

unsigned long long timespecDiff(struct timespec *timeA_p, struct timespec *timeB_p)
{
  return ((timeA_p->tv_sec * 1000000000) + timeA_p->tv_nsec) -
           ((timeB_p->tv_sec * 1000000000) + timeB_p->tv_nsec);
}

void func()
{

}

int main()
{
	unsigned long long runs = 100000;
	struct timespec start;
	struct timespec stop;
	unsigned long long result; //64 bit integer
	unsigned long long totalTime = 0;
	unsigned long long averageTime;

	int i;
	for (i = 0; i < runs; i++)
	{
		clock_gettime(CLOCK_MONOTONIC, &start);
		func();
		clock_gettime(CLOCK_MONOTONIC, &stop);

		// Use the function to get the difference between the two times
		result=timespecDiff(&stop,&start);

		totalTime += result;
	}
	averageTime = totalTime / runs;

	printf("Average time for a function call using CLOCK_MONOTONIC for %llu cycles: %llu\n", runs, result);
}