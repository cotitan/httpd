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
	int listenfd;
	struct sockaddr_in servaddr;
	struct sockaddr_in connaddr;
	void bind_listen();
	void do_read(int epfd, int fd);
	void handle_accept(int epfd, int listenfd);
	void add_event(int epfd, int fd, int state);
	void delete_event(int epfd, int fd, int state);

public:
	// default = "127.0.0.1:9090"
	server();
	server(const char *addr, short port);
	// bind, listen, accept, response
	int start();
	~server();
};

#endif

