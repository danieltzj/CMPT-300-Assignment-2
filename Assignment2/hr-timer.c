#include <stdio.h>
#include <stdint.h>
#include <time.h>

unsigned long long timespecDiff(struct timespec *timeA_p, struct timespec *timeB_p)
{
  return ((timeA_p->tv_sec * 1000000000) + timeA_p->tv_nsec) -
           ((timeB_p->tv_sec * 1000000000) + timeB_p->tv_nsec);
}


int main()
{
	// timespec structs from time.h that contain 2 members
	// time_t tv_sec - seconds
	// long tv_nsec - nanoseconds
	struct timespec start;
	struct timespec stop;
	unsigned long long result; //64 bit integer

	// Uses the 2 structs to get the time after a sleep of 1 second
	// The timer is using the RealTime Clock
	clock_gettime(CLOCK_REALTIME, &start);
	sleep(1);
	clock_gettime(CLOCK_REALTIME, &stop);

	// Use the function to get the difference between the two times
	result=timespecDiff(&stop,&start);

	printf("CLOCK_REALTIME Measured: %llu\n",result);

	// Uses the 2 structs to get the time after a sleep of 1 second
	// The timer is using the MONOTONIC Clock
	clock_gettime(CLOCK_MONOTONIC, &start);
	sleep(1);
	clock_gettime(CLOCK_MONOTONIC, &stop);

	// Use the function to get the difference between the two times
	result=timespecDiff(&stop,&start);

	printf("CLOCK_MONOTONIC Measured: %llu\n",result);

	// Same thing as above but usin
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
	sleep(1);
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &stop);

	// Use the function to get the difference between the two times
	result=timespecDiff(&stop,&start);

	printf("CLOCK_PROCESS_CPUTIME_ID Measured: %llu\n",result);

	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);
	sleep(1);
	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &stop);

	// Use the function to get the difference between the two times
	result=timespecDiff(&stop,&start);

	printf("CLOCK_THREAD_CPUTIME_ID Measured: %llu\n",result);


}