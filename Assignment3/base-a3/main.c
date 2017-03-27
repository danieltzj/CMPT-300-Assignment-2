#include "main.h"

#define DELIMITERS " \t\n"
#define MAX_BUFFER 512

void *fnC()
{
    int i;
    for(i=0;i<1000000;i++)
    {   
        c++;
    }   
}


pthread_mutex_t count_mutex;
pthread_spinlock_t spinlock;

void *pthreadMutexTest()
{
	
    int i;
	int j;
	int k;
	
	int localCount = 0;
	
    for(i=0;i<numItterations;i++)
    {
		
		for(j=0;j<workOutsideCS;j++)/*How much work is done outside the CS*/
		{
			localCount++;
		}
		
		pthread_mutex_lock(&count_mutex);
		for(k=0;k<workInsideCS;k++)/*How much work is done inside the CS*/
		{
			c++;
		}
		pthread_mutex_unlock(&count_mutex);    
	
    }   
}

void *pthreadSpinklockTest()
{
	
    int i;
	int j;
	int k;
	
	int localCount = 0;
	
    for(i=0;i<numItterations;i++)
    {
		
		for(j=0;j<workOutsideCS;j++)/*How much work is done outside the CS*/
		{
			localCount++;
		}
		
		pthread_spin_lock(&spinlock);
		for(k=0;k<workInsideCS;k++)/*How much work is done inside the CS*/
		{
			c++;
		}
		pthread_spin_unlock(&spinlock);    
	
    }   
}

void *mySpinlockTASTest()
{
	
    int i;
	int j;
	int k;
	
	int localCount = 0;
	
    for(i=0;i<numItterations;i++)
    {
		
		for(j=0;j<workOutsideCS;j++)/*How much work is done outside the CS*/
		{
			localCount++;
		}
		
		my_spinlock_lockTAS(&spinlockTAS);
		for(k=0;k<workInsideCS;k++)/*How much work is done inside the CS*/
		{
			c++;
		}
		my_spinlock_unlock(&spinlockTAS);    
	
    }   
}

void *mySpinlockTTASTest()
{
	
    int i;
	int j;
	int k;
	
	int localCount = 0;
	
    for(i=0;i<numItterations;i++)
    {
		
		for(j=0;j<workOutsideCS;j++)/*How much work is done outside the CS*/
		{
			localCount++;
		}
		
		my_spinlock_lockTTAS(&spinlockTTAS);
		for(k=0;k<workInsideCS;k++)/*How much work is done inside the CS*/
		{
			c++;
		}
		my_spinlock_unlock(&spinlockTTAS);   
	
    }   
}

void *myMutexTASTest()
{
	
    int i;
	int j;
	int k;
	
	int localCount = 0;
	
    for(i=0;i<numItterations;i++)
    {
		
		for(j=0;j<workOutsideCS;j++)/*How much work is done outside the CS*/
		{
			localCount++;
		}
		
		my_mutex_lock(&mutexTAS);
		for(k=0;k<workInsideCS;k++)/*How much work is done inside the CS*/
		{
			c++;
		}
		my_mutex_unlock(&mutexTAS);
	
    }   
}

void *myQueueLockTest()
{
	
    int i;
	int j;
	int k;
	
	int localCount = 0;
	
    for(i=0;i<numItterations;i++)
    {
		
		for(j=0;j<workOutsideCS;j++)/*How much work is done outside the CS*/
		{
			localCount++;
		}
		
		my_queuelock_lock(&queuelockCAS);
		for(k=0;k<workInsideCS;k++)/*How much work is done inside the CS*/
		{
			c++;
		}
		my_queuelock_unlock(&queuelockCAS);   
	
    }   
}

int runTest(int testID)
{

/*You must create all data structures and threads for you experiments*/

if (testID == 0 || testID == 1 ) /*Pthread Mutex*/
{
	c=0;
	struct timespec start;
	struct timespec stop;
	unsigned long long result; //64 bit integer

	pthread_t *threads = (pthread_t* )malloc(sizeof(pthread_t)*numThreads);	
	int i;
	int rt;

	clock_gettime(CLOCK_MONOTONIC, &start);
	for(i=0;i<numThreads;i++)
	{
	
	 if( rt=(pthread_create( threads+i, NULL, &pthreadMutexTest, NULL)) )
	{
		printf("Thread creation failed: %d\n", rt);
		return -1;	
	}
	
	}
	
	for(i=0;i<numThreads;i++) //Wait for all threads to finish
	{
		 pthread_join(threads[i], NULL);
	}
	clock_gettime(CLOCK_MONOTONIC, &stop);

	printf("Threaded Run of %d threads using Pthread(mutex) for %d iterations Total Count: %d\n",numThreads, numItterations,  c);
	result=timespecDiff(&stop,&start);
	printf("Pthread Mutex time(ms): %llu\n",result/1000000);
	free(threads); // free allocated memory

}

if(testID == 0 || testID == 2) /*Pthread Spinlock*/
{
/*Pthread Spinlock goes here*/
	c=0;
	struct timespec start;
	struct timespec stop;
	unsigned long long result; //64 bit integer

	pthread_t *threads = (pthread_t* )malloc(sizeof(pthread_t)*numThreads);	
	int i;
	int rt;

	pthread_spin_init(&spinlock, PTHREAD_PROCESS_SHARED);
	clock_gettime(CLOCK_MONOTONIC, &start);
	for(i=0;i<numThreads;i++)
	{
	
	 if( rt=(pthread_create( threads+i, NULL, &pthreadSpinklockTest, NULL)) )
	{
		printf("Thread creation failed: %d\n", rt);
		return -1;	
	}
	
	}
	
	for(i=0;i<numThreads;i++) //Wait for all threads to finish
	{
		 pthread_join(threads[i], NULL);
	}
	clock_gettime(CLOCK_MONOTONIC, &stop);

	pthread_spin_destroy(&spinlock);

	printf("Threaded Run of %d threads using Pthread(spinlock) for %d iterations Total Count: %d\n",numThreads, numItterations,  c);
	result=timespecDiff(&stop,&start);
	printf("Pthread Mutex time(ms): %llu\n",result/1000000);
	free(threads); // free allocated memory
}

if(testID == 0 || testID == 3) /*MySpinlockTAS*/
{
	c=0;
	struct timespec start;
	struct timespec stop;
	unsigned long long result; //64 bit integer

	my_spinlock_init(&spinlockTAS);
	pthread_t *threads = (pthread_t* )malloc(sizeof(pthread_t)*numThreads);	
	int i;
	int rt;

	clock_gettime(CLOCK_MONOTONIC, &start);
	for(i=0;i<numThreads;i++)
	{
	
	 if( rt=(pthread_create( threads+i, NULL, &mySpinlockTASTest, NULL)) )
	{
		printf("Thread creation failed: %d\n", rt);
		return -1;	
	}
	
	}
	
	for(i=0;i<numThreads;i++) //Wait for all threads to finish
	{
		 pthread_join(threads[i], NULL);
	}
	clock_gettime(CLOCK_MONOTONIC, &stop);

	my_spinlock_destroy(&spinlockTAS);

	printf("Threaded Run of %d threads using mySpinlockTAS for %d iterations Total Count: %d\n",numThreads, numItterations,  c);
	result=timespecDiff(&stop,&start);
	printf("Pthread Mutex time(ms): %llu\n",result/1000000);
	free(threads); // free allocated memory
}

if(testID == 0 || testID == 4) /*MySpinlockTTAS*/
{
	c=0;
	struct timespec start;
	struct timespec stop;
	unsigned long long result; //64 bit integer

	pthread_t *threads = (pthread_t* )malloc(sizeof(pthread_t)*numThreads);	
	int i;
	int rt;

	my_spinlock_init(&spinlockTTAS);
	clock_gettime(CLOCK_MONOTONIC, &start);
	for(i=0;i<numThreads;i++)
	{
	
	 if( rt=(pthread_create( threads+i, NULL, &mySpinlockTTASTest, NULL)) )
	{
		printf("Thread creation failed: %d\n", rt);
		return -1;	
	}
	
	}
	
	for(i=0;i<numThreads;i++) //Wait for all threads to finish
	{
		 pthread_join(threads[i], NULL);
	}
	clock_gettime(CLOCK_MONOTONIC, &stop);

	my_spinlock_destroy(&spinlockTTAS);

	printf("Threaded Run of %d threads using mySpinlockTTAS for %d iterations Total Count: %d\n",numThreads, numItterations,  c);
	result=timespecDiff(&stop,&start);
	printf("Pthread Mutex time(ms): %llu\n",result/1000000);
	free(threads); // free allocated memory
}

if(testID == 0 || testID == 5) /*Exponential Back off Lock (Mutex)*/
{
	c=0;
	struct timespec start;
	struct timespec stop;
	unsigned long long result; //64 bit integer

	pthread_t *threads = (pthread_t* )malloc(sizeof(pthread_t)*numThreads);	
	int i;
	int rt;

	my_mutex_init(&mutexTAS);
	clock_gettime(CLOCK_MONOTONIC, &start);
	for(i=0;i<numThreads;i++)
	{
	
	 if( rt=(pthread_create( threads+i, NULL, &myMutexTASTest, NULL)) )
	{
		printf("Thread creation failed: %d\n", rt);
		return -1;	
	}
	
	}
	
	for(i=0;i<numThreads;i++) //Wait for all threads to finish
	{
		 pthread_join(threads[i], NULL);
	}
	clock_gettime(CLOCK_MONOTONIC, &stop);

	my_mutex_destroy(&mutexTAS);

	printf("Threaded Run of %d threads using myMutexTAS for %d iterations Total Count: %d\n",numThreads, numItterations,  c);
	result=timespecDiff(&stop,&start);
	printf("Pthread Mutex time(ms): %llu\n",result/1000000);
	free(threads); // free allocated memory
}

if(testID == 0 || testID == 6) /*Queue Lock ( Ticket Lock) */
{
	c=0;
	struct timespec start;
	struct timespec stop;
	unsigned long long result; //64 bit integer

	pthread_t *threads = (pthread_t* )malloc(sizeof(pthread_t)*numThreads);	
	int i;
	int rt;

	my_queuelock_init(&queuelockCAS);
	clock_gettime(CLOCK_MONOTONIC, &start);
	for(i=0;i<numThreads;i++)
	{
	
	 if( rt=(pthread_create( threads+i, NULL, &myQueueLockTest, NULL)) )
	{
		printf("Thread creation failed: %d\n", rt);
		return -1;	
	}
	
	}
	
	for(i=0;i<numThreads;i++) //Wait for all threads to finish
	{
		 pthread_join(threads[i], NULL);
	}
	clock_gettime(CLOCK_MONOTONIC, &stop);

	my_queuelock_destroy(&queuelockCAS);

	printf("Threaded Run of %d threads using myQueueLock for %d iterations Total Count: %d\n",numThreads, numItterations,  c);
	result=timespecDiff(&stop,&start);
	printf("Pthread Mutex time(ms): %llu\n",result/1000000);
	free(threads); // free allocated memory
}

/*....you must implement the other tests....*/

	return 0;
}

int testAndSetExample()
{
	volatile long test = 0; //Test is set to 0
	printf("Test before atomic OP:%d\n",test);
	tas(&test);
	printf("Test after atomic OP:%d\n",test);
}

int processInput(int argc, char *argv[])
{

/*testid: 0=all, 1=pthreadMutex, 2=pthreadSpinlock, 3=mySpinLockTAS, 4=mySpinLockTTAS, 5=myMutexTAS, 6=myQueueLock*/
	/*You must write how to parse input from the command line here, your software should default to the values given below if no input is given*/

	numThreads=4;
	numItterations=1000000;
	testID=0;
	workOutsideCS=0;
	workInsideCS=1;

	char commandArg[MAX_BUFFER];
	int default_values = 0;

	fgets (commandArg,MAX_BUFFER,stdin);

	// remove the new line character at the end and replace it with a null terminate
	size_t length = strlen(commandArg);
	if (commandArg[length - 1] == '\n')
	{
		commandArg[length - 1] = '\0';
	}

	int i = 0;
	argv[i] = strtok(commandArg, DELIMITERS);
	if (argv[i] == NULL)
	{
		default_values = 1;
	}
	else
	{
		while (argv[i+1] = strtok(NULL, DELIMITERS))
		{
			i++;
		}
	}

	if (default_values)
	{
		numThreads=4;
		numItterations=1000000;
		testID=0;
		workOutsideCS=0;
		workInsideCS=1;
	}
	else
	{
		int max_arg = i;
		i = 0;
		if (max_arg % 2 != 1)
		{
			printf("odd number of inputs, please restart the program and enter the correct inputs\n");
			exit(0);
		}
		while (i < max_arg)
		{
			if (!strcmp(argv[i], "-t"))
			{
				numThreads = atoi(argv[i+1]);
			}
			else if (!strcmp(argv[i], "-i"))
			{
				numItterations = atoi(argv[i+1]);
			}
			else if (!strcmp(argv[i], "-o"))
			{
				workOutsideCS = atoi(argv[i+1]);
			}
			else if (!strcmp(argv[i], "-c"))
			{
				workInsideCS = atoi(argv[i+1]);
			}
			else if (!strcmp(argv[i], "-d"))
			{
				testID = atoi(argv[i+1]);
			}
			i = i + 2;
		}
	}
	return 0;
}


int main(int argc, char *argv[])
{


	printf("Usage of: %s -t #threads -i #Itterations -o #OperationsOutsideCS -c #OperationsInsideCS -d testid\n", argv[0]);
	printf("testid: 0=all, 1=pthreadMutex, 2=pthreadSpinlock, 3=mySpinLockTAS, 4=mySpinLockTTAS, 5=myMutexTAS, 6=myQueueLock, \n");	
	
	//testAndSetExample(); //Uncomment this line to see how to use TAS
	
	processInput(argc,argv);
	runTest(testID);
	return 0;

}
