#ifndef THREAD_POOL_H
#define THREAD_POOL_H
#include <semaphore.h>
#include <queue>
#include <pthread.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <cstdlib>
#include <cstdio>
#include <iostream>
using namespace std;

#ifdef DEBUG_MODE
#define DEBUG(str) (cout << (str))
#else
#define DEBUG(str) 
#endif

#define SEGSIZE 1024
extern void *accept_req(void *);
struct thread_params {
	int connfd;
	char *header;
	int len;
};


typedef void (*DEL)(int, int);
class thread_pool {
private:
	DEL delete_event;
	pthread_mutex_t mutex;
	sem_t nJob;
	int nThread;
	int epfd;	// fd of epoll
	pthread_t *threads;
	queue<int> jobs;

	static void *func(void *args);

	// void delete_event(int epollfd, int fd, int state);

	void exec_job(int fd);

public:
	thread_pool(DEL *del, int epollfd, int nThr = 12);

	void add_job(int fd);

	~thread_pool();
};

#endif
