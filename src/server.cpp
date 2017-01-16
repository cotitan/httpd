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

	pthread_mutex_init(&epl_mutex, NULL);
	epfd = epoll_create(EPSIZE);
	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		perror("Error: fail to get sockfd!");

}

int server::start() {

	signal(SIGPIPE, SIG_IGN);

	if (bind_listen() == -1)
		return -1;

	struct epoll_event events[EPSIZE];
	memset(events, 0, sizeof(events));
	add_event(listenfd, EPOLLIN);
	thread_pool pool(this, 8);
	pool.start();

	int ret, fd, i;
	while (1) {
		ret = epoll_wait(epfd, events, EPSIZE, -1);
		for (i = 0; i < ret; i++) {
			fd = events[i].data.fd;
			if (fd == listenfd && (events[i].events & EPOLLIN)) {
				handle_accept();
			} else if (events[i].events & EPOLLIN) {
				pool.add_job(fd);
			}
		}
	}
	close(epfd);
	return 0;
}

void server::handle_accept() {
	socklen_t sin_size = sizeof(servaddr);
	int fd = accept(listenfd, (SA *)&connaddr, &sin_size);
	if (fd == -1)
        perror("Accpet Error:");
    else {
		DEBUG("Accept a request on fd #%d\n", fd);
        // printf("accept a new client: %s:%d\n",
        // inet_ntoa(connaddr.sin_addr), connaddr.sin_port);                       //添加一个客户描述符和事件         
        add_event(fd, EPOLLIN);
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

void server::add_event(int fd, int state) {
	DEBUG("Add event on fd#%d\n", fd);
	struct epoll_event ev;
	memset(&ev, 0, sizeof(ev));
	ev.events = state;
	ev.data.fd = fd;
	pthread_mutex_lock(&epl_mutex);
	if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1)
		perror("error adding events!\n");
	pthread_mutex_unlock(&epl_mutex);
}

void server::delete_event(int fd, int state) {
	struct epoll_event ev;
	ev.events = state;
	ev.data.fd = fd;
	pthread_mutex_lock(&epl_mutex);
	epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &ev);
	pthread_mutex_unlock(&epl_mutex);
	DEBUG("Delete a fd #%d\n\n", fd);
}

server::~server() {
	close(listenfd);
	cerr << "Server shutdown, all connect closed\n";
}
