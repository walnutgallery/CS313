/* 
    File: dataserver.C

    Author: R. Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 2012/07/16

    Dataserver main program for MP3 in CSCE 313
*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <cassert>
#include <cstring>
#include <sstream>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>

#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

#include "NetworkRequestChannel.h"

using namespace std;

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* VARIABLES */
/*--------------------------------------------------------------------------*/

static int nthreads = 0;

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

//void handle_process_loop(NetworkRequestChannel & _channel);

/*--------------------------------------------------------------------------*/
/* LOCAL FUNCTIONS -- SUPPORT FUNCTIONS */
/*--------------------------------------------------------------------------*/

string int2string(int number) {
   stringstream ss;//create a stringstream
   ss << number;//add number to the stream
   return ss.str();//return a string with the contents of the stream
}

/*--------------------------------------------------------------------------*/
/* LOCAL FUNCTIONS -- THREAD FUNCTIONS */
/*--------------------------------------------------------------------------

void * handle_data_requests(void * args) {

  NetworkRequestChannel * data_channel =  (NetworkRequestChannel*)args;

  // -- Handle client requests on this channel. 
  
  handle_process_loop(*data_channel);

  // -- Client has quit. We remove channel.
 
  delete data_channel;
}

/*--------------------------------------------------------------------------*/
/* LOCAL FUNCTIONS -- INDIVIDUAL REQUESTS */
/*--------------------------------------------------------------------------*/

void process_hello(NetworkRequestChannel* _channel, const string & _request) {
  _channel->cwrite("hello to you too");
}

void process_data(NetworkRequestChannel* _channel, const string &  _request) {
  usleep(1000 + (rand() % 5000));
  //_channel.cwrite("here comes data about " + _request.substr(4) + ": " + int2string(random() % 100));
  _channel->cwrite(int2string(rand() % 100));
}
/*
void process_newthread(NetworkRequestChannel & _channel, const string & _request) {
  int error;
  nthreads ++;

  // -- Name new data channel

  string new_channel_name = "data" + int2string(nthreads) + "_";
  //  cout << "new channel name = " << new_channel_name << endl;

  // -- Pass new channel name back to client

  _channel.cwrite(new_channel_name);

  // -- Construct new data channel (pointer to be passed to thread function)
  
  NetworkRequestChannel * data_channel = new NetworkRequestChannel(new_channel_name, RequestChannel::SERVER_SIDE);

  // -- Create new thread to handle request channel

  pthread_t thread_id;
  //  cout << "starting new thread " << nthreads << endl;
  if (error = pthread_create(& thread_id, NULL, handle_data_requests, data_channel)) {
    fprintf(stderr, "p_create failed: %s\n", strerror(error));
  }  

}

/*--------------------------------------------------------------------------*/
/* LOCAL FUNCTIONS -- THE PROCESS REQUEST LOOP */
/*--------------------------------------------------------------------------*/

void process_request(NetworkRequestChannel* _channel, const string & _request) {

  if (_request.compare(0, 5, "hello") == 0) {
    process_hello(_channel, _request);
  }
  else if (_request.compare(0, 4, "data") == 0) {
    process_data(_channel, _request);
  }
  //else if (_request.compare(0, 9, "newthread") == 0) {
  //  process_newthread(_channel, _request);
  //}
  else {
    _channel->cwrite("unknown request");
  }

}

/*void handle_process_loop(NetworkRequestChannel & _channel) {

  for(;;) {

    //cout << "Reading next request from channel (" << _channel.name() << ") ..." << flush;
      cout << flush;
    string request = _channel.cread();
    //cout << " done (" << _channel.name() << ")." << endl;
    //cout << "New request is " << request << endl;

    if (request.compare("quit") == 0) {
      _channel.cwrite("bye");
      usleep(10000);          // give the other end a bit of time.
      break;                  // break out of the loop;
    }

    process_request(_channel, request);
  }
  
}*/

void* connection_handler(void* arg)
{
	NetworkRequestChannel* nrc = (NetworkRequestChannel*) arg;
	while(1)
	{
		string request = nrc->cread();
		
		if(request.compare("quit") == 0)
		{
			cout << "Quit received!" << endl;
			nrc->cwrite("bye");
			usleep(10000);
			break;
		}
		process_request(nrc, request);
	}
}

/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/

int main(int argc, char * argv[]) {

	int b, p;
	b = 20;
	p = 6969;
	int opt = 0;
	while ((opt = getopt(argc, argv, "b:p:h")) != -1) {
		switch (opt) {
		case 'b':
			b = atoi(optarg);
			break;
		case 'p':
			p = atoi(optarg);
			break;
		case 'h':
		default:
			std::cout << "This program can be invoked with the following flags:" << std::endl;
			std::cout << "-b [int]: backlog" << std::endl;
			std::cout << "-p [int]: port number" << std::endl;
			std::cout << "-h: print this message and quit" << std::endl;
			std::cout << "Example: ./dataserver -b 50 -p 11001" << std::endl;
			std::cout << "If a given flag is not used, a default value will be given" << std::endl;
			std::cout << "to its corresponding variable. If an illegal option is detected," << std::endl;
			std::cout << "behavior is the same as using the -h flag." << std::endl;
			exit(0);
		}
	}
	cout << "Establishing control channel... " << endl;
	NetworkRequestChannel control(p, connection_handler, b);
	cout << "done!" << endl;
}

