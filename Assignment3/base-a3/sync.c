/*
 * NAME, etc.
 *
 * sync.c
 *
 *
 */

#define _REENTRANT
#define MIN_DELAY 1
#define MAX_DELAY 100

#include <unistd.h>
#include <stdlib.h>
#include "sync.h"


/*
 * Spinlock routines
 */

int my_spinlock_init(my_spinlock_t *lock)
{
	lock->lock_value = 0;
	return 0;
}

int my_spinlock_destroy(my_spinlock_t *lock)
{
	return 0;
}

int my_spinlock_unlock(my_spinlock_t *lock)
{
	lock->lock_value = 0;
	return 0;
}

int my_spinlock_lockTAS(my_spinlock_t *lock)
{
	while(tas( &(lock->lock_value))) {};
	return 0;
}


int my_spinlock_lockTTAS(my_spinlock_t *lock)
{
	while(1)
	{
		while(lock->lock_value == 1) {};
		if (!tas( &(lock->lock_value)))
		{
			return 0;
		}
	}
}

int my_spinlock_trylock(my_spinlock_t *lock)
{
	while(tas( &(lock->lock_value)))
	{
		return -1;
	}
	return 0;
}


/*
 * Mutex routines
 */

int my_mutex_init(my_mutex_t *lock)
{
	lock->lock_value = 0;
	return 0;
}

int my_mutex_destroy(my_mutex_t *lock)
{
	return 0;
}

int my_mutex_unlock(my_mutex_t *lock)
{	
	lock->lock_value = 0;
	return 0;
}

int my_mutex_lock(my_mutex_t *lock)
{
	useconds_t delay = MIN_DELAY;
	useconds_t max_Delay = MAX_DELAY;
	while (1)
	{
		while(lock->lock_value == 1) {};
		if (!tas( &(lock->lock_value)))
		{
			return 0;
		}
		sleep(rand() % delay);
		if (delay < max_Delay)
		{
			delay = 2 * delay;
		}
	}
}

int my_mutex_trylock(my_mutex_t *lock)
{
	useconds_t delay = MIN_DELAY;
	useconds_t max_Delay = MAX_DELAY;
	while (1)
	{
		while(lock->lock_value == 1) 
		{
			return -1;
		}
		if (!tas( &(lock->lock_value)))
		{
			return 0;
		}
		sleep(rand() % delay);
		if (delay < max_Delay)
		{
			delay = 2 * delay;
		}
	}
}

/*
 * Queue Lock
 */

int my_queuelock_init(my_queuelock_t *lock)
{
	lock->now_serving = 0;
	lock->next_ticket = 0;
	return 0;
}

int my_queuelock_destroy(my_queuelock_t *lock)
{
	return 0;
}

int my_queuelock_unlock(my_queuelock_t *lock)
{
	lock->now_serving += 1;
	return 0;
}

int my_queuelock_lock(my_queuelock_t *lock)
{
	int my_ticket;

	my_ticket = faa(&(lock->next_ticket));

	while(lock->now_serving != my_ticket){ /*spin*/ };
	return 0;
}

int my_queuelock_trylock(my_queuelock_t *lock)
{
	int my_ticket;

	my_ticket = faa(&(lock->next_ticket));

	while(lock->now_serving != my_ticket)
	{
		return -1;
	}
	return 0;
}

