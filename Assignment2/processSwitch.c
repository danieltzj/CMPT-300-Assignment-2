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
	// pipes[0][1] is the childs write end of the pipe
	// pipes[1] for child process
	// pipes[1][0] is the childs read end of the pipe
	// pipes[1][1] is the parents write end of the pipe
	int pipes[2][2];
	pid_t pid, pidp; // child process pid
	int num = 0;
	unsigned long long runs = 100000;
	struct timespec start;
	struct timespec stop;
	unsigned long long result; //64 bit integer
	unsigned long long totalTime = 0;
	unsigned long long averageTime;

	// create the two pipes
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

		// read data from the read end of the corresponding child pipe
		int success;
		success = read(pipes[1][0], &num, sizeof(num));
		if (success == -1) 
		{
			printf("Child process failed to read data from the pipe\n");
			exit(0);
		}
		else
		{
			// message received from parent process
			if (write(pipes[0][1], &num, sizeof(num)) == -1)
			{
				printf("Child process failed to write data to the pipe\n");
				exit(0);
			}
		}
	}
}