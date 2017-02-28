#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <sys/wait.h>

// Subtraction function taken from instructors sample code @ http://www.sfu.ca/~rws1/cmpt-300/assignments/hr-timer.c
unsigned long long timespecDiff(struct timespec *timeA_p, struct timespec *timeB_p)
{
  return ((timeA_p->tv_sec * 1000000000) + timeA_p->tv_nsec) -
           ((timeB_p->tv_sec * 1000000000) + timeB_p->tv_nsec);
}

int main()
{
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
	unsigned long long runs = 100000;
	struct timespec start;
	struct timespec stop;
	unsigned long long result; //64 bit integer
	unsigned long long totalTime = 0;
	unsigned long long averageTime;
	int status;

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
		// close pipes that aren't in use ( "parent" ends of the pipes)
		close(pipes[0][0]); // close "parents" read end
		close(pipes[1][1]); // close "parents" write end

		// get the pid of the child so we can know what process to wait for
		pid = getpid();
		printf("%d\n", num + 20 );
		// read data from the read end of the corresponding child pipe
		int successChild;
		// Start the timer before switching to the parent
		clock_gettime(CLOCK_MONOTONIC, &start);
		successChild = read(pipes[1][0], &num, sizeof(num));
		if (successChild == -1) 
		{
			printf("Child process failed to read data from the pipe\n");
			exit(0);
		}
		else
		{
			num = num*2;
			printf("%d\n", num );
			// message received from parent process
			if (write(pipes[0][1], &num, sizeof(num)) == -1)
			{
				printf("Child process failed to write data to the pipe\n");
				exit(0);
			}
		}

		// close the other pipes now
		close(pipes[1][0]); // close child read end
		close(pipes[0][1]); // close child write end

		return EXIT_SUCCESS;
	}
	printf("%d\n", num );
	// Stop the timer once the parent process starts running
	clock_gettime(CLOCK_MONOTONIC, &stop);
	result=timespecDiff(&stop,&start);
	totalTime += result;
	printf("%llu\n",result );
	// close child pipes
	close(pipes[1][0]); // close child read end
	close(pipes[0][1]); // close child write end

	// write a number to the child process
	// get the time before you write as once the data is written
	if (write(pipes[1][1], &num, sizeof(num)) == -1)
	{
		printf("Parent process failed to write data to the pipe\n");
		exit(0);
	}

	int successParent;
	successParent = read(pipes[0][0], &num, sizeof(num));
	if (successParent == -1) 
	{
		printf("Parent process failed to read data from the pipe\n");
		exit(0);
	}
	else
	{
		printf("%d\n", num);
	}

	close(pipes[0][0]); // close "parents" read end
	close(pipes[1][1]); // close "parents" write end

	waitpid(pid, &status, WUNTRACED);
}