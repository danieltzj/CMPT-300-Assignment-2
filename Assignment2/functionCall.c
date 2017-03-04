#include <stdio.h>
#include <stdint.h>
#include <time.h>

// Subtraction function taken from instructors sample code @ http://www.sfu.ca/~rws1/cmpt-300/assignments/hr-timer.c
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
	unsigned long long runs = 1000;
	struct timespec start;
	struct timespec stop;
	unsigned long long result; //64 bit integer
	unsigned long long totalTime = 0;
	unsigned long long averageTime;
	// Open a file to write data to
	FILE *f = fopen("functionCallTime.txt", "w");

	int i;
	for (i = 0; i < runs; i++)
	{
		clock_gettime(CLOCK_MONOTONIC, &start);
		func();
		clock_gettime(CLOCK_MONOTONIC, &stop);

		// Use the function to get the difference between the two times
		result=timespecDiff(&stop,&start);
		fprintf(f, "%llu\n",result );

		totalTime += result;
	}
	averageTime = totalTime / runs;

	printf("Average time for a function call using CLOCK_MONOTONIC for %llu cycles: %llu\n", runs, result);

	fclose(f);
}