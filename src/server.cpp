#include "server.h"
#include <cstring>
#include <signal.h>
#include <stdexcept>
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <sys/epoll.h>
#include "thread_pool.h"
using namespace std;

#define EPSIZE 10240

server::server() : server("127.0.0.1", 9090) {}

server::server(const char *ip, short port) {
	servaddr.sin_family = AF_INET;
	/*
	if (inet_pton(AF_INET, ip, &servaddr.sin_addr) < 0)
		perror("Error: IP address cannot be resolved");
		*/
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(port);
	bzero(&servaddr.sin_zero, sizeof(servaddr.sin_zero));

	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		perror("Error: fail to get sockfd!");
}

int server::start() {

	signal(SIGPIPE, SIG_IGN);

	if (bind_listen() == -1)
		return -1;

	int epfd = epoll_create(EPSIZE);
	struct epoll_event events[EPSIZE];
	memset(events, 0, sizeof(events));
	add_event(epfd, listenfd, EPOLLIN);
	thread_pool pool(epfd, 8);

	while (1) {
		int ret = epoll_wait(epfd, events, EPSIZE, -1);
		for (int i = 0; i < ret; i++) {
			int fd = events[i].data.fd;
			if (fd == listenfd && (events[i].events & EPOLLIN)) {
				handle_accept(epfd, listenfd);
			} else if (events[i].events & EPOLLIN) {
				pool.add_job(fd);
			}
		}
	}
	close(listenfd);
	return 0;
}

void server::handle_accept(int epfd, int listenfd) {
	socklen_t sin_size = sizeof(servaddr);
	int fd = accept(listenfd, (SA *)&connaddr, &sin_size);
	if (fd == -1)
        perror("accpet error:");
    else {    
        // printf("accept a new client: %s:%d\n",
        // inet_ntoa(connaddr.sin_addr), connaddr.sin_port);                       //添加一个客户描述符和事件         
        add_event(epfd, fd, EPOLLIN);
    } 
}

int server::bind_listen() {
	int flag = 1;
	socklen_t flag_size = sizeof(flag);
	// setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &flag, flag_size);
	if (bind(listenfd, (SA *)&servaddr, sizeof(servaddr)) < 0) {
		perror("Error: fail to bind!");
		return -1;
	}
	if (listen(listenfd, 1000) < 0)
		perror("Error: fail to listen!");
	fprintf(stdout, "start listening on port %d...\n",
		ntohs(servaddr.sin_port));
	return 0;
}

void server::add_event(int epfd, int fd, int state) {
	struct epoll_event ev;
	memset(&ev, 0, sizeof(ev));
	ev.events = state;
	ev.data.fd = fd;
	if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1)
		perror("error adding events!\n");
}

server::~server() {
	close(listenfd);
	cerr << "Server shutdown, all connect closed\n";
}
