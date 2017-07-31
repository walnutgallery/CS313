#include <queue>
#include <string>
#include <pthread.h>

class semaphore{
private:
	pthread_mutex_t semaLock;
	int count;
	pthread_cond_t waitQueue;
public:
	semaphore(int init);
	~semaphore();
	void P();
	void V();
};