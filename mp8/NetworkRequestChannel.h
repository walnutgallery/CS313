#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

using namespace std;

class NetworkRequestChannel{

private:
	int master, slave;
	int port;
	int listening;
	int connecting;
	
public:
	
	//client side
	NetworkRequestChannel(const string _server_host_name, const unsigned short _port_no);
	//server side
	NetworkRequestChannel(const unsigned short _port_no, void *(*connection_handler) (void*), int backlog);
	//slave
	NetworkRequestChannel(int socketID);
	~NetworkRequestChannel();
	string send_request(string _request);
	string cread();
	int cwrite(string _msg);
	int socketFD();
};