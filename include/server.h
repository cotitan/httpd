#ifndef SERVER_H_
#define SERVER_H_

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <vector>
using std::vector;

#define SA struct sockaddr

class server {
private:
	int servsock;
	vector<int> clntsock;
	struct sockaddr_in servaddr;
	struct sockaddr_in connaddr;
	int maxfdp1;
	void response_to_req(int fd, char *msg);
public:
	// default = "127.0.0.1:9090"
	server();
	server(const char *addr, short port);
	// bind, listen, accept, response
	int start();
	~server();
};

#endif

