# makefile

all: dataserver client 

NetworkRequestChannel.o: NetworkRequestChannel.h NetworkRequestChannel.cpp
	g++ -std=c++11 -c -g NetworkRequestChannel.cpp

bounded_buffer.o: bounded_buffer.h bounded_buffer.cpp
	g++ -std=c++11 -c -g bounded_buffer.cpp
	
semaphore.o: semaphore.h semaphore.cpp
	g++ -std=c++11 -c -g semaphore.cpp
	
dataserver: dataserver.cpp NetworkRequestChannel.o 
	g++ -std=c++11 -g -o dataserver dataserver.cpp NetworkRequestChannel.o -lpthread


client: client_MP8.cpp NetworkRequestChannel.o semaphore.o bounded_buffer.o
	g++ -std=c++11 -g -o client client_MP8.cpp NetworkRequestChannel.o semaphore.o bounded_buffer.o -lpthread

clean:
	rm *.o fifo* dataserver client output*
