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

void func()
{

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
	pthread_mutex_lock(&lock);
	while( shared_num != 1)
	{
		pthread_cond_wait(&num_is_one, &lock);
	}
	clock_gettime(CLOCK_MONOTONIC, &stop_t);
	timeTaken_t = timespecDiff(&stop_t,&start_t);
	totalTime_t += timeTaken_t;
	pthread_mutex_unlock(&lock);

	pthread_mutex_lock(&lock);
	shared_num = 0;
	pthread_cond_signal(&num_is_zero);
	pthread_mutex_unlock(&lock);
	
}

void *thread2_func()
{
	pthread_mutex_lock(&lock);
	while( shared_num != 0)
	{
		pthread_cond_wait(&num_is_zero, &lock);
	}
	pthread_mutex_unlock(&lock);

	pthread_mutex_lock(&lock);
	shared_num = 1;
	pthread_cond_signal(&num_is_one);
	pthread_mutex_unlock(&lock);
	clock_gettime(CLOCK_MONOTONIC, &start_t);
}

int main()
{
	// Set the CPU Affinity to only use core 1
	cpu_set_t  mask;
	CPU_ZERO(&mask);
	CPU_SET(1, &mask);
	int result;
	result = sched_setaffinity(0, sizeof(mask), &mask);

	// 2 pipes for interprocess communication
	// pipes[0] for parent process
	// pipes[0][0] is the parents read end of the pipe
	// pipes[0][1] is the childs write end of the pipe, child is writing into this end to the parent reading from the other end
	// pipes[1] for child process
	// pipes[1][0] is the childs read end of the pipe
	// pipes[1][1] is the parents write end of the pipe, parent is writing into this end to the child reading from the other end
	int pipes[2][2];
	pid_t pid, pidp; // child process pid
	int num = 100;
	int status;
	struct timespec start;
	struct timespec stop;
	unsigned long long timeTaken; //64 bit integer
	unsigned long long totalTime = 0;
	unsigned long long averageTime;
	unsigned long long runs = 1000000;

	//*************************************** Minimal Function Call ********************************************/
	int i;
	for (i = 0; i < runs; i++)
	{
		clock_gettime(CLOCK_MONOTONIC, &start);
		func();
		clock_gettime(CLOCK_MONOTONIC, &stop);

		// Use the function to get the difference between the two times
		timeTaken=timespecDiff(&stop,&start);

		totalTime += timeTaken;
	}
	averageTime = totalTime / runs;

	printf("Average time for a function call using CLOCK_MONOTONIC for %llu cycles: %llu ns\n", runs, averageTime);

	/***************************************** Minimal System Call *******************************************/
	for (i = 0; i < runs; i++)
	{
		clock_gettime(CLOCK_MONOTONIC, &start);
		pid = getpid();
		clock_gettime(CLOCK_MONOTONIC, &stop);

		// Use the function to get the difference between the two times
		timeTaken=timespecDiff(&stop,&start);

		totalTime += timeTaken;

		if (i == 0)
		{
			printf("The first run of getpid() took %llu ns\n",timeTaken );
		}
		else if (i == 1)
		{
			printf("consecutive runs of getpid() take about %llu ns\n",timeTaken );
		}
	}
	averageTime = totalTime/runs;

	printf("Average time for a system call using CLOCK_MONOTONIC for %llu cycles: %llu ns\n", runs, averageTime);

	/********************************************* Process Switch ******************************************/
	int k;
	for ( k = 0; k < runs; k++)
	{
		// create the two pipes
		int i;
		for ( i = 0; i < 2; i++)
		{
			if (pipe(pipes[i]) < 0)
			{
				perror("creation of pipe failed, exiting program now");
				exit(0);
			}
		}

		pid = fork();
		if (pid < 0)
		{
			fprintf(stderr, "Fork Failed");
			exit(0);
		}
		else if (pid == 0)
		{
			// Child Process code here
			// Stop the timer once the child process starts running
			clock_gettime(CLOCK_MONOTONIC, &stop);

			// close pipes that aren't in use ( "parent" ends of the pipes)
			close(pipes[0][0]); // close "parents" read end
			close(pipes[1][1]); // close "parents" write end

			// read data from the read end of the corresponding child pipe
			int successChild;
			successChild = read(pipes[1][0], &start, sizeof(start));
			if (successChild == -1) 
			{
				printf("Child process failed to read data from the pipe\n");
				exit(0);
			}
			else
			{
				// write the stop time back to the parent process so the parent has the stop value
				if (write(pipes[0][1], &stop, sizeof(stop)) == -1)
				{
					printf("Child process failed to write data to the pipe\n");
					exit(0);
				}
			}
			// get the pid of the child so we can know what process to wait for
			pid = getpid();

			// close the other pipes now
			close(pipes[1][0]); // close child read end
			close(pipes[0][1]); // close child write end

			return EXIT_SUCCESS;
		}
		// close child pipes
		close(pipes[1][0]); // close child read end
		close(pipes[0][1]); // close child write end

		// write a number to the child process
		if (write(pipes[1][1], &start, sizeof(start)) == -1)
		{
			printf("Parent process failed to write data to the pipe\n");
			exit(0);
		}

		int successParent;
		// get the time before you read when the read starts it will be read blocked
		// This is because the buffer at the write end of the pipe is empty, so the parent process will sleep
		clock_gettime(CLOCK_MONOTONIC,&start);
		// process will be read blocked.
		successParent = read(pipes[0][0], &stop, sizeof(stop));
		if (successParent == -1) 
		{
			printf("Parent process failed to read data from the pipe\n");
			exit(0);
		}
		// Do the subtraction with the value received from the child
		timeTaken=timespecDiff(&stop,&start);
		totalTime += timeTaken;

		close(pipes[0][0]); // close "parents" read end
		close(pipes[1][1]); // close "parents" write end

		//wait for the child process to finish.
		waitpid(pid, &status, WUNTRACED);
	}
	averageTime = totalTime/(runs);

	printf("Average time for a process switch using CLOCK_MONOTONIC for %llu cycles: %llu ns\n", runs, averageTime);

	/******************************************* Thread Switch **************************************************/
	
	for (i = 0; i < runs; i++)
	{
		int thread_result1, thread_result2;

		pthread_t thread1, thread2;

		// Code referenced from sample provided by the prof @
		// http://199.60.17.135/cmpt-300/wp-content/uploads/2016/09/lock-example.c
		if (thread_result1 = pthread_create( &thread1, NULL, &thread1_func, NULL))
		{
			printf("Thread creation failed: %d\n", thread_result1);
		}
		if (thread_result2 = pthread_create( &thread2, NULL, &thread2_func, NULL))
		{
			printf("Thread creation failed: %d\n", thread_result2);
		}

		pthread_join(thread1, NULL);
		pthread_join(thread2, NULL);
	}

	averageTime = totalTime_t/runs;

	printf("Average time for a process switch using CLOCK_MONOTONIC for %llu cycles: %llu ns\n", runs, averageTime );
}