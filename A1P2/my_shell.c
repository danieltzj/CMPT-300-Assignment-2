#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <wait.h>
#include <sys/stat.h>

#define DELIMITERS " \t\n"
#define MAX_BUFFER 512

void sigint_handler(int signum)
{

}

int main()
{
	// variables
	char commandArg[MAX_BUFFER];
	char* argv[100]; // user command array	
	char* argp[100]; // pipe command array
	char history[10][MAX_BUFFER];
	int counter = 0;
	int maxedOut = 0;
	pid_t pid, parentpid;
	int status;

	// while loop for the parent process
	while(1)
	{
		int empty = 0;
		int pipes = 0;
		int numPipes = 0;
		int pipeCount = 0;
		int commandPos[10];

		printf("->");

		// if a command is input
		if (!fgets (commandArg,MAX_BUFFER,stdin))
		{
			break;
		}
		else if(feof(stdin))
		{
			exit(0);
		}
		// remove the new line character at the end and replace it with a null terminate
		size_t length = strlen(commandArg);
		if (commandArg[length - 1] == '\n')
		{
			commandArg[length - 1] = '\0';
		}

		// store the commands input into the history array
		if (counter == 10)
		{
			maxedOut = 1;
			counter = 0;
			strcpy(history[counter], commandArg);
			counter++;
		}
		else
		{
			strcpy(history[counter], commandArg);
			counter++;
		}

		// Iterate through the command and delimit it into tokens
		int i = 0;
		argv[i] = strtok(commandArg, DELIMITERS);
		if (argv[i] == NULL)
		{
			empty = 1;
			continue;
		}
		else
		{
			while (argv[i+1] = strtok(NULL, DELIMITERS))
			{
				i++;
			}
		}
		// Replace the pipe characters to NULL and store the position as the position of the next command
		pipeCount = 0; // keep track of how many | as the number of pipes
		commandPos[0] = 0; // initialize the first command to the 0th element in the array as the first command always starts from there
		i = 0;
		int iter = 1;
		while (argv[i] != NULL)
		{
			if (!strcmp(argv[i], "|"))
			{
				if (pipes == 0)
				{
					pipes = 1; // set the boolean of pipes to 1 to tell us we have piped input
				}
				argv[i]= NULL;
				commandPos[iter] = i+1;
				iter++;
				pipeCount++;
			}
			i++;
		}
		
		
		// read the command e.g cd,pwd,ls etc...
		char cwd[MAX_BUFFER];
		// cd internal command
		if (!empty)
		{
			if (!strcmp(argv[0], "cd"))
			{
				if(chdir(argv[1]) == -1)
				{
					printf("invalid directory\n");
				}
			}
			// pwd internal command
			else if (!strcmp(argv[0],"pwd"))
			{
				if (getcwd(cwd, sizeof(cwd)) != NULL)
				{
					fprintf(stdout, "%s\n", cwd);
				}
				else
				{
					perror("error with getting current working directory");
				}
			}
			// history internal command
			else if (!strcmp(argv[0], "history"))
			{
				int maxHist = counter;
				printf("List of the last 10 commands:\n");
				if (maxedOut == 1) maxHist = 10;
				int j = 0;
				for ( j = 0; j < maxHist; j++)
				{
					printf("%s\n",history[j]);
				}
			}
			// exit internal command
			else if (!strcmp(argv[0],"exit"))
			{
				exit(0);
			}
			// piped command
			else if (pipes)
			{
				int fd[pipeCount][2]; // create an 2d array of pipes as you need a read and write for each pipe
				
				// Mkae an array of pid to keep track of what processes to wait for at the end
				int pidv[pipeCount+1];

				int i = 0;
				// for every pipe we have, call the pipe function on the two elements of the array
				// iterating through the array by 2 after piping every time
				for ( i = 0; i < pipeCount; i++)
				{
					if (pipe(fd[i]) < 0)
					{
						perror("creation of pipe failed, exiting program now");
						exit(0);
					}
				}

				int commandToExecute; // extract the position of the command to be executed

				// Iterate through the number of commands we have to execute
				// which will be the number of pipes + 1
				for (i = 0; i < pipeCount+1; i++)
				{
					// get the position of the command to execute via the commandPos array that was made
					// The position points to the first argument of the command right after the NULL
					commandToExecute = commandPos[i];

					pidv[i] = fork();

					if (pidv[i] < 0)
					{
						fprintf(stderr, "Fork Failed");
						exit(0);
					}
					else if (pidv[i] == 0)
					{
						// If it's not the last pipe, assign the write end of the pipe to the STDOUT of the process
						if (i == 0)
						{
							// close the STDOUT of the process
							close(STDOUT_FILENO);
							// close the read end of the pipe
							close(fd[i][0]);
							// duplicate the write end of the pipe to the STDOUT
							dup2(fd[i][1], STDOUT_FILENO);
							close(fd[i][1]);
						}
						// if its the last pipe, close the write end and assign the read end to the STDIN of the process
						else if ( i == pipeCount )
						{
							// close the STDIN of the process
							close(STDIN_FILENO);
							// close the write end of the previous pipe
							close(fd[i-1][1]);
							// duplicate the read end of the pipe onto the STDIN of the process
							dup2(fd[i-1][0], STDIN_FILENO);
							close(fd[i-1][0]);
						}
						else
						{
							// close the STDIN of the process	
							close(STDIN_FILENO);
							// duplicate the read end of the previous pipe onto the STDIN of the process
							dup2(fd[i-1][0], STDIN_FILENO);
							close(fd[i-1][0]);

							// close the STDOUT of the process
							close(STDOUT_FILENO);
							// close the read end of the pipe
							close(fd[i][0]);
							// duplicate the write end of the pipe to the STDOUT
							dup2(fd[i][1], STDOUT_FILENO);
							close(fd[i][1]);
						}

						// close ALL the pipes ( somehow fixes the hanging problem, seems like the file is outputting to open pipes or something )
						int a;
						for (a = 0; a < pipeCount; a++)
						{
							close(fd[a][0]);
							close(fd[a][1]);
						}

						// Execute the program with the command as the first token and then the position of where the arguments start from
						// which starts from comandToExecute
						if ( execvp(argv[commandToExecute], argv + commandToExecute) < 0)
						{
							perror("error with executing binary, please check if it is a valid shell command and try again\n");
							// exit the child process
							exit(0);
						}
					}
				}
				// One last check that all pipes are closed
				for ( i = 0; i < pipeCount; i++)
				{
					close(fd[i][0]);
					close(fd[i][1]);
				}
				// Parent process will wait for each process to finish executing.
				for (i = 0 ; i < pipeCount+1; i++)
				{
					wait(&pidv[i]);
				}
			}
			// Any other binary file to execute
			else
			{
				// Referenced from sample code provided @ http://www.sfu.ca/~rws1/cmpt-300/assignments/a1-sample-fork.pdf
				pid = fork();

				if (pid < 0)
				{
					fprintf(stderr, "Fork Failed");
					exit(0);
				}
				// child process created
				else if (pid == 0)
				{
					if (execvp(argv[0], argv) == -1)
					{
						perror("error with executing binary, please check if it is a valid shell command and try again\n");
						// exit the child process
						exit(0);
					}
				}
				else
				{
					do 
					{
				      	parentpid = waitpid(pid, &status, WUNTRACED);
				    } 
				    while (!WIFEXITED(status) && !WIFSIGNALED(status));
				}
			}
		}
	}
	return 0;
}