/*
	File: client_MP6.cpp

	Author: J. Higginbotham
	Department of Computer Science
	Texas A&M University
	Date  : 2016/05/21

	Based on original code by: Dr. R. Bettati, PhD
	Department of Computer Science
	Texas A&M University
	Date  : 2013/01/31

	MP6 for Dr. //Tyagi's
	Ahmed's sections of CSCE 313.
 */

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

	/* -- (none) -- */
	/* -- This might be a good place to put the size of
		of the patient response buffers -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*
	As in MP7 no additional includes are required
	to complete the assignment, but you're welcome to use
	any that you think would help.
*/
/*--------------------------------------------------------------------------*/

#include <cassert>
#include <cstring>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include "reqchannel.h"
//#include <pthread.h>
//#include <string>
#include <sstream>
#include <sys/time.h>
#include <assert.h>
#include <cmath>
#include <numeric>
#include <list>
#include <vector>
//#include <queue>
#include <fstream>

/*
	This next file will need to be written from scratch, along with
	semaphore.h and (if you choose) their corresponding .cpp files.
 */

#include "bounded_buffer.h"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/

/*
	All *_params structs are optional,
	but they might help.
 */
class Stats{
	pthread_mutex_t statLock;
	
	public:
	std::vector<int> freqCount;
		
	Stats() {
			freqCount = std::vector<int>(10, 0);
			pthread_mutex_init(&statLock, NULL);
	}

	~Stats() {
			pthread_mutex_destroy(&statLock);
	}

	void update(std::string response) {
			pthread_mutex_lock(&statLock);
			freqCount.at(stoi(response) / 10) += 1;
			pthread_mutex_unlock(&statLock);
	}

	int acum() {
			return accumulate(freqCount.begin(), freqCount.end(), 0);
	}
};

struct request_thread_params {
	std::string name;
	int num;
	bounded_buffer* reqBuff; 

};

struct handler_params {
	bounded_buffer* reqBuff;
	bounded_buffer* johnRes;
	bounded_buffer* janeRes;
	bounded_buffer* joeRes;
	int n;
	int w;
	RequestChannel* channel;
};

struct stat_thread_params {
	bounded_buffer* responseBuff;
	Stats* status;
	int resNum;
};


/*
	This class can be used to write to standard output
	in a multithreaded environment. It's primary purpose
	is printing debug messages while multiple threads
	are in execution.
 */
class atomic_standard_output {
	pthread_mutex_t console_lock;
	public:
	atomic_standard_output() { pthread_mutex_init(&console_lock, NULL); }
	~atomic_standard_output() { pthread_mutex_destroy(&console_lock); }
	void print(std::string s){
		pthread_mutex_lock(&console_lock);
		std::cout << s << std::endl;
		pthread_mutex_unlock(&console_lock);
	}
};

atomic_standard_output threadsafe_standard_output;

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

	/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* HELPER FUNCTIONS */
/*--------------------------------------------------------------------------*/

std::string make_histogram(std::string name, std::vector<int> *data) {
	std::string results = "Frequency count for " + name + ":\n";
	for(int i = 0; i < data->size(); ++i){
		results += std::to_string(i * 10) + "-" + std::to_string((i * 10) + 9) + ": " + std::to_string(data->at(i)) + "\n";
	}
	return results;
}

/*
	You'll need to fill these in.
*/
void* request_thread_function(void* arg) {
	request_thread_params* p = (request_thread_params*) arg;
	for( int i = 0; i < p->num; ++i ){
		p->reqBuff->push("data " + p->name);
	}
}

void* handler_function(void* arg) {
	handler_params* p = (handler_params*) arg;
	//ints to keep track of max number of cwrites, creads, and fds, respectively
	int sent = 0, rec = 0, nfd = 0;
	//creates an array of work channels, up to w
	RequestChannel* workChan[p->w];
	//keeps track of the channel's current message so that we can process properly
	std::vector<string> sentMsgs;
	//request string
	std::string req;
	
	//create channels
	for(int i = 0; i < p->w; ++ i )
	{
		std::string s = p->channel->send_request("newthread");
		workChan[i] = new RequestChannel(s, RequestChannel::CLIENT_SIDE);
	}
	//create fd_set
	fd_set rfds;
	FD_ZERO(&rfds);
	for(int i = 0; i < p->w; i++)
	{
		FD_SET(workChan[i]->read_fd(), &rfds);
		nfd = max(workChan[i]->read_fd(), nfd);
	}
	//so that nfd is max fd + 1
	nfd++;
	
	//original message sent to channel
	for(int i = 0; i < p->w; i++)
	{
		req = p->reqBuff->pop();
		sentMsgs.push_back(req);
		workChan[i]->cwrite(req);
		sent++;
	}
	
	//start while loop to take care of all requests
	while(rec < 3 * p->n)
	{
		//reset FD after every pass
		FD_ZERO(&rfds);
		for(int i = 0; i < p->w; i++)
		{
			FD_SET(workChan[i]->read_fd(), &rfds);
		}
		//find ready to read channels
		int selected = select(nfd, &rfds, NULL, NULL, NULL);
		//check for errors
		if(selected == -1)
		{
			std::cerr << "select function error.";
		}
		//if no errors, then start processing
		else
		{
			for (int i = 0; i < p->w; i++)
			{
				if(FD_ISSET(workChan[i]->read_fd(), &rfds))
				{
					//server response
					string resp = workChan[i]->cread();
					//increment received messages so that loop ends
					rec++;
					//the original sent message to server
					string check = sentMsgs[i];
					if(check == "data John Smith")
					{
						p->johnRes->push(resp);
					}
					else if(check == "data Jane Smith")
					{
						p->janeRes->push(resp);
					}
					else if(check == "data Joe Smith")
					{
						p->joeRes->push(resp);
					}
					
					//send new request to empty channel, making sure to stop when no new requests are avaiable
					if(sent < 3 * p->n)
					{
						req = p->reqBuff->pop();
						workChan[i]->cwrite(req);
						sentMsgs[i] = req;
						sent++;
						if(workChan[i]->read_fd() > nfd)
						{
							nfd = workChan[i]->read_fd();
							nfd++;
						}
					}
				}
			}
		}
	}
	//send quits after all requests are processed
	for(int i = 0; i < p->w; i++)
	{
		workChan[i]->send_request("quit");
		delete workChan[i];
	}
}

void* stat_thread_function(void* arg) {
	stat_thread_params* p = (stat_thread_params*) arg;
	while(!(p->responseBuff->finished())) {
		std::string response = p->responseBuff->pop();
		p->status->update(response);
	}
}

/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/

int main(int argc, char * argv[]) {
	int n = 10; //default number of requests per "patient"
	int b = 50; //default size of request_buffer
	int w = 10; //default number of worker threads
	bool USE_ALTERNATE_FILE_OUTPUT = false;
	int opt = 0;
	while ((opt = getopt(argc, argv, "n:b:w:m:h")) != -1) {
		switch (opt) {
			case 'n':
				n = atoi(optarg);
				break;
			case 'b':
				b = atoi(optarg);
				break;
			case 'w':
				w = atoi(optarg);
				break;
			case 'm':
				if(atoi(optarg) == 2) USE_ALTERNATE_FILE_OUTPUT = true;
				break;
			case 'h':
			default:
				std::cout << "This program can be invoked with the following flags:" << std::endl;
				std::cout << "-n [int]: number of requests per patient" << std::endl;
				std::cout << "-b [int]: size of request buffer" << std::endl;
				std::cout << "-w [int]: number of channels" << std::endl;
				std::cout << "-m 2: use output2.txt instead of output.txt for all file output" << std::endl;
				std::cout << "-h: print this message and quit" << std::endl;
				std::cout << "Example: ./client_solution -n 10000 -b 50 -w 120 -m 2" << std::endl;
				std::cout << "If a given flag is not used, a default value will be given" << std::endl;
				std::cout << "to its corresponding variable. If an illegal option is detected," << std::endl;
				std::cout << "behavior is the same as using the -h flag." << std::endl;
				exit(0);
		}
	}
	
	int pid = fork();
	if(pid == 0){
		struct timeval start_time;
		struct timeval finish_time;
		int64_t start_usecs;
		int64_t finish_usecs;
		ofstream ofs;
		if(USE_ALTERNATE_FILE_OUTPUT) ofs.open("output2.txt", ios::out | ios::app);
		else ofs.open("output.txt", ios::out | ios::app);
		
		std::cout << "n == " << n << std::endl;
		std::cout << "b == " << b << std::endl;
		std::cout << "w == " << w << std::endl;
		
		std::cout << "CLIENT STARTED:" << std::endl;
		std::cout << "Establishing control channel... " << std::flush;
		RequestChannel* chan = new RequestChannel("control", RequestChannel::CLIENT_SIDE);
		std::cout << "done." << std::endl;

		Stats* john_frequency_count = new Stats();
		Stats* jane_frequency_count = new Stats();
		Stats* joe_frequency_count = new Stats();

		bounded_buffer* requests = new bounded_buffer(b, n * 3);

		bounded_buffer* johnResponses = new bounded_buffer(n / 2, n);
		bounded_buffer* janeResponses = new bounded_buffer(n / 2, n);
		bounded_buffer* joeResponses = new bounded_buffer(n / 2, n);
		
		/*
			This time you're up a creek.
			What goes in this section of the code?
			Hint: it looks a bit like what went here 
			in MP5, but only a *little* bit.
		 */
		 
		//timer start
		gettimeofday(&start_time, NULL);

		//start requests
		pthread_t reqThreads[3];
		
		request_thread_params rParams[3];
		rParams[0].name = "John Smith";
		rParams[0].num = n;
		rParams[0].reqBuff = requests;
		rParams[1].name = "Jane Smith";
		rParams[1].num = n;
		rParams[1].reqBuff = requests;
		rParams[2].name = "Joe Smith";
		rParams[2].num = n;
		rParams[2].reqBuff = requests;

		for( int i = 0; i < 3; ++i )
		{
			pthread_create(&reqThreads[i], NULL, request_thread_function, &rParams[i]);
		}
		
		//start handler thread
		handler_params handy;
		handy.reqBuff = requests;
		handy.johnRes = johnResponses;
		handy.janeRes = janeResponses;
		handy.joeRes = joeResponses;
		handy.n = n;
		handy.w = w;
		handy.channel = chan;
		handler_params* handyP = &handy;
		

		pthread_t handler;
		pthread_create(&handler, NULL, handler_function, handyP);

		//start stat thread
		pthread_t statThreads[3];
		stat_thread_params sParams[3];
		sParams[0].responseBuff = johnResponses;
		sParams[0].status = john_frequency_count;
		sParams[0].resNum = n;
		sParams[1].responseBuff = janeResponses;
		sParams[1].status = jane_frequency_count;
		sParams[1].resNum = n;
		sParams[2].responseBuff = joeResponses;
		sParams[2].status = joe_frequency_count;
		sParams[2].resNum = n;
		
		for ( int i = 0; i < 3; ++ i )
		{
			pthread_create(&statThreads[i], NULL, stat_thread_function, &sParams[i]);
		}

		for ( int i = 0; i < 3; ++ i ) {
			pthread_join(reqThreads[i], NULL);
		}
		pthread_join(handler, NULL);
		for ( int i = 0; i < 3; ++ i ) {
			pthread_join(statThreads[i], NULL);
		}
		
		//timer stop
		gettimeofday(&finish_time, NULL);
		
		//results
		start_usecs = (start_time.tv_sec * 1e6) + start_time.tv_usec;
        finish_usecs = (finish_time.tv_sec * 1e6) + finish_time.tv_usec;
        std::cout << "Finished!" << std::endl;
        
        std::string john_results = make_histogram("John Smith", &john_frequency_count->freqCount);
        std::string jane_results = make_histogram("Jane Smith", &jane_frequency_count->freqCount);
        std::string joe_results = make_histogram("Joe Smith", &joe_frequency_count->freqCount);
        
        std::cout << "Results for n == " << n << ", w == " << w << std::endl;
        std::cout << "Time to completion: " << std::to_string(finish_usecs - start_usecs) << " usecs" << std::endl;
        std::cout << "John Smith total: " << john_frequency_count->acum() << std::endl;
        std::cout << john_results << std::endl;
        std::cout << "Jane Smith total: " << jane_frequency_count->acum() << std::endl;
        std::cout << jane_results << std::endl;
        std::cout << "Joe Smith total: " << joe_frequency_count->acum() << std::endl;
        std::cout << joe_results << std::endl;


        //STORE RESULTS
        ofs << "N: " << n << "\nB: " << b << "\nW: " << w << "\nTime: " << std::to_string(finish_usecs - start_usecs) <<std::endl;

		std::cout << "Sleeping..." << std::endl;
		usleep(10000);
		std::string finale = chan->send_request("quit");
		std::cout << "Finale: " << finale << std::endl;

		//CLEAN UP
		delete requests;
		delete johnResponses;
		delete john_frequency_count;
		delete janeResponses;
		delete jane_frequency_count;
		delete joeResponses;
		delete joe_frequency_count;
		delete chan;
		ofs.close();
	}
	else if(pid != 0) execl("dataserver", NULL);
}