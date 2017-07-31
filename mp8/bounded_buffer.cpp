#include "bounded_buffer.h"

bounded_buffer::bounded_buffer(int bin, int max)
{
	requests = std::queue<std::string>();
	pthread_mutex_init(&buffLock, NULL);
	full = new semaphore(0);
	empty = new semaphore(bin);
	maxReq = max;
}

bounded_buffer::~bounded_buffer()
{
	pthread_mutex_destroy(&buffLock);
	delete full;
	delete empty;
}

void bounded_buffer::push(std::string s)
{
	empty->P();
	pthread_mutex_lock(&buffLock);
	requests.push(s);
	pthread_mutex_unlock(&buffLock);
	full->V();
}

std::string bounded_buffer::pop()
{
	full->P();
	pthread_mutex_lock(&buffLock);
	std::string rt = requests.front();
	requests.pop();
	maxReq--;
	pthread_mutex_unlock(&buffLock);
	empty->V();
	return rt;
}	

bool bounded_buffer::finished()
{
	pthread_mutex_lock(&buffLock);
	bool finished = maxReq == 0;
	pthread_mutex_unlock(&buffLock);
	return finished;
}