#include "semaphore.h"

class bounded_buffer{
private:
	std::queue<std::string> requests;
	pthread_mutex_t buffLock;
	semaphore* full;
	semaphore* empty;
	int maxReq;
public:
	bounded_buffer(int bin, int max);
	~bounded_buffer();
	void push(std::string s);
	std::string pop();
	bool finished();

};