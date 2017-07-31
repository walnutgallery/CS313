#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include "NetworkRequestChannel.h"

NetworkRequestChannel::NetworkRequestChannel(const string _server_host_name, const unsigned short _port_no){
	/* Creates a CLIENT-SIDE local copy of the channel. The channel is connected
	to the given port number at the given server host. THIS CONSTRUCTOR IS CALLED
	BY THE CLIENT. */
	//setup variables
	struct addrinfo hints, *res;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	//attempt getaddrinfo for connecting
	int stat = getaddrinfo(_server_host_name.c_str(), to_string(_port_no).c_str(), &hints, &res);
	if(stat != 0)
		std::cerr << "getaddrinfo failed" << std::endl;
	//make socket
	master = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if(master < 0)
		std::cerr << "socket failed" << std::endl;
	//connect
	int connecting = connect(master, res->ai_addr, res->ai_addrlen);
	if(connecting < 0)
		std::cerr << "Error connecting to server" << std::endl;
}

NetworkRequestChannel::NetworkRequestChannel(const unsigned short _port_no, void *(*connection_handler)(void*), int backlog){
	/* Creates a SERVER-SIDE local copy of the channel that is accepting
	connections at the given port number. NOTE that multiple clients can be
	connected to the same server-side end of the request channel. Whenever a new
	connection comes in, it is accepted by ther server, and the given connection
	handler is invoked. The parameter to the connection handler is the file
	descriptor of the slave socket returned by the accept call. NOTE that the
	connection handler does not want to deal with closing the socket. You will
	have to close the socket once the connection handler is done. */
	//setup variables
	struct addrinfo hints, *serv;
	struct sockaddr_storage connectingAddr;
	socklen_t sin_size;
	char s[INET6_ADDRSTRLEN];
	int rv;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	//get address info
	rv = getaddrinfo(NULL, to_string(_port_no).c_str(), &hints, &serv);
	if(rv != 0)
		std::cerr << "getaddrinfo failed" << std::endl;
	//get socket
	master = socket(serv->ai_family, serv->ai_socktype, serv->ai_protocol);
	if(master == -1)
		std::cerr << "socketing failed" << std::endl;
	//attempt bind
	if(bind(master, serv->ai_addr, serv->ai_addrlen) == -1)
	{
		close(master);
		std::cerr << "Binding failed" << std::endl;
	}
	freeaddrinfo(serv);
	//attempt listen
	listening = listen(master, backlog);
	if(listening == -1)
		std::cerr << "listening failed" << std::endl;
	//accepting loop
	while(1)
	{
		sin_size = sizeof connectingAddr;
		slave = accept(master, (struct sockaddr*)&connectingAddr, &sin_size);
		if(slave == -1)
		{}
		else
		{
			std::cout << "New connection!" << std::endl;
			NetworkRequestChannel* nrc = new NetworkRequestChannel(slave);
			pthread_t conThread;
			pthread_create(&conThread, NULL, connection_handler, (void*) nrc);
		}
	}
}

NetworkRequestChannel::NetworkRequestChannel(int socketID){
	/* Makes a NetworkRequestChannel using the socket that accept() function 
	returns upon getting a new incoming connection. This is also called the 
	"slave socket". */
	master = socketID;
}
	
NetworkRequestChannel::~NetworkRequestChannel(){
	/* Destructor of the local copy of the channel. */
	close(master);
}

string NetworkRequestChannel::send_request(string _request){
	/* Send a string over the channel adn wait for a reply. */
	cwrite(_request);
	std::string resp = cread();
	return resp;
}
	
string NetworkRequestChannel::cread(){
	/* Blocking read of data from the channel. Returns a string of characters
	read from the channel. Returns NULL if read failed. */
	char buffer[120];
	if(recv(master, buffer, 120, 0) < 0)
		std::cerr << "Read error" << std::endl;
	std::string resp = buffer;
	return resp;
}
	
int NetworkRequestChannel::cwrite(string _msg){
	/* Write the data to the channel. The function returns the number of
	characters written to the channel. */
	if(_msg.length() >= 120)
		std::cerr << "Message is too long" << std::endl;
	const char* message = _msg.c_str();
	int sending = send(master, message, std::strlen(message) + 1, 0);
	if(sending < 0)
		std::cerr << "Write error" << std::endl;
}

int NetworkRequestChannel::socketFD()
{
	//returns socketFD for select()
	return master;
}
