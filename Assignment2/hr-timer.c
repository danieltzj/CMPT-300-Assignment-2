#include <stdio.h>
#include <stdint.h>
#include <time.h>

// Function that converts the stop times tv_sec to nano seconds and adds it to the tv_nsec portion of the struct.
// It does this for both start and stop time and then gets the difference between them
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
	// realtime clock is timed using the systems clock, so if the admin sets the clocks value, your results will be skewed
	clock_gettime(CLOCK_REALTIME, &start); // get the start time
	sleep(1);
	clock_gettime(CLOCK_REALTIME, &stop); // get the stop time

	// Use the function to get the difference between the two times
	result=timespecDiff(&stop,&start);

	// print the result out
	printf("CLOCK_REALTIME Measured: %llu\n",result);

	// Uses the 2 structs to get the time after a sleep of 1 second
	// The timer is using the MONOTONIC Clock which is absolute time and good for getting differences between two points in time
	clock_gettime(CLOCK_MONOTONIC, &start);
	sleep(1);
	clock_gettime(CLOCK_MONOTONIC, &stop);

	// Use the function to get the difference between the two times
	result=timespecDiff(&stop,&start);

	printf("CLOCK_MONOTONIC Measured: %llu\n",result);

	// Same thing as above but using CLOCK_PROCESS_CPUTIME_ID
	// this measures how much time a process is using regardless of context switches, good for measuring how much time a specific process is taking on the cpu
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
	sleep(1);
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &stop);

	// Use the function to get the difference between the two times
	result=timespecDiff(&stop,&start);

	printf("CLOCK_PROCESS_CPUTIME_ID Measured: %llu\n",result);

	// Similar to the above clock except with threads
	// it will time how much a thread took on the cpu regardless of switches or preemption, good for measuring time a specific thread is taking on the cpu
	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);
	sleep(1);
	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &stop);

	// Use the function to get the difference between the two times
	result=timespecDiff(&stop,&start);

	printf("CLOCK_THREAD_CPUTIME_ID Measured: %llu\n",result);


}