#include "server.h"
#include <cstring>
#include <signal.h>
#include <stdexcept>
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <sys/epoll.h>
using namespace std;

#define SEGSIZE 10240
extern void *accept_req(void *);
struct thread_params {
	int connfd;
	char *header;
	int len;
};

server::server() : server("127.0.0.1", 9090) {}

server::server(const char *ip, short port) {
	servaddr.sin_family = AF_INET;
	/*
	if (inet_pton(AF_INET, ip, &servaddr.sin_addr) < 0)
		throw invalid_argument("Error: IP address cannot be resolved");
		*/
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(port);
	bzero(&servaddr.sin_zero, sizeof(servaddr.sin_zero));

	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		throw invalid_argument("Error: fail to get sockfd!");
}

int server::start() {

	signal(SIGPIPE, SIG_IGN);

	bind_listen();

	int epfd = epoll_create(SEGSIZE);
	struct epoll_event events[SEGSIZE];
	add_event(epfd, listenfd, EPOLLIN);

	while (1) {
		int ret = epoll_wait(epfd, events, SEGSIZE, -1);
		for (int i = 0; i < ret; i++) {
			int fd = events[i].data.fd;
			if (fd == listenfd && (events[i].events & EPOLLIN)) {
				handle_accept(epfd, listenfd);
			} else if (events[i].events & EPOLLIN) {
				do_read(epfd, fd);
			}
		}
	}
	return 0;
}

void server::handle_accept(int epfd, int listenfd) {
	socklen_t sin_size = sizeof(servaddr);
	int fd = accept(listenfd, (SA *)&connaddr, &sin_size);
	if (fd == -1)
        perror("accpet error:");
    else {    
        // printf("accept a new client: %s:%d\n",
        //    inet_ntoa(connaddr.sin_addr), connaddr.sin_port);                       //添加一个客户描述符和事件         
        add_event(epfd, fd, EPOLLIN);
    } 
}

void server::bind_listen() {
	int flag = 1;
	socklen_t flag_size = sizeof(flag);
	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &flag, flag_size);
	if (bind(listenfd, (SA *)&servaddr, sizeof(servaddr)) < 0)
		throw invalid_argument("Error: fail to bind!");
	if (listen(listenfd, 1000) < 0)
		throw invalid_argument("Error: fail to listen!");
	fprintf(stdout, "start listening on port %d...\n",
		ntohs(servaddr.sin_port));
}

void server::add_event(int epfd, int fd, int state) {
	struct epoll_event ev;
	ev.events = state;
	ev.data.fd = fd;
	if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1)
		perror("error adding events!\n");
}

void server::delete_event(int epollfd, int fd, int state) {
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &ev);
}

void server::do_read(int epfd, int fd) {
	char *header = new char[SEGSIZE + 1];
	int nread = read(fd, header, SEGSIZE);
	if (nread == -1) {
        perror("read error:");
        close(fd); //记住close fd
        delete_event(epfd,fd,EPOLLIN); //删除监听
        delete[] header;
    }
    else if (nread == 0) {
        // fprintf(stderr,"client close.\n");
        close(fd);
        delete_event(epfd,fd,EPOLLIN); //删除监听
        delete[] header;
    }
    else {
    	struct thread_params param = { fd, header, nread };
        void *status = accept_req((void *)&param);
        if (status == (void *)-1) {
        	close(fd);
        	delete_event(epfd,fd,EPOLLIN); //删除监听
        	delete[] header;
        }
        // header will be deleted in accept_req
    }
}

server::~server() {
	close(listenfd);
	cerr << "Server shutdown, all connect closed\n";
}

