#ifndef THREAD_POOL_H
#define THREAD_POOL_H
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include "server.h"
#include "threadsafe_queue.h"
using namespace std;

#define DEBUG_MODE

#ifdef DEBUG_MODE
#define DEBUG printf
#else
#define DEBUG
#endif

#define SEGSIZE 1024
extern void *accept_req(void *);
struct thread_params {
	int connfd;
	char *header;
	int len;
};

class server;
class thread_pool {
private:
	sem_t nJob;
	int nThread;
	pthread_t *threads;
	threadsafe_queue<int> jobs;

	static void *func(void *args);

	// void delete_event(int epollfd, int fd, int state);

public:
	friend void *manager(void *args);
	
	thread_pool(int nThr = 4);

	// start running threads
	void start();

	void add_job(int fd);

	void exec_job(int fd);

	~thread_pool();
};

#endif
