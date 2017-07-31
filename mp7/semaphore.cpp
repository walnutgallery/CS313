#include "semaphore.h"

semaphore::semaphore(int init)
{
	pthread_mutex_init(&semaLock, NULL);
	count = init;
	waitQueue = PTHREAD_COND_INITIALIZER;
}

semaphore::~semaphore()
{
	pthread_mutex_destroy(&semaLock);
	pthread_cond_destroy(&waitQueue);
}

void semaphore::P()
{
	pthread_mutex_lock(&semaLock);
	count--;
	if(count < 0)
	{
		pthread_cond_wait(&waitQueue, &semaLock);
	}
	pthread_mutex_unlock(&semaLock);
}

void semaphore::V()
{
	pthread_mutex_lock(&semaLock);
	count++;
	if(count <= 0)
	{
		pthread_cond_signal(&waitQueue);
	}
	pthread_mutex_unlock(&semaLock);
}