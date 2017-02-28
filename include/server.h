#ifndef SERVER_H_
#define SERVER_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <vector>
#include "thread_pool.h"
using std::vector;

#define SA struct sockaddr

class server {
private:
	int listenfd;
	int epfd;
	pthread_mutex_t epl_mutex;
	struct sockaddr_in servaddr;
	struct sockaddr_in connaddr;

public:
	// default = "127.0.0.1:9090"
	server();
	server(const char *addr, short port);
	int bind_listen();
	void do_read(int fd);
	void handle_accept();
	void add_event(int fd, int state);
	void delete_event(int fd, int state);
	// bind, listen, accept, response
	int start();
	void setnonblocking(int fd);
	~server();
};

#endif

