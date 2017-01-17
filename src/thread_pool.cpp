#include "thread_pool.h"
#include <sys/types.h>

void *thread_pool::func(void *args) {
	thread_pool *pool = (thread_pool *)args;
	while (true) {
		sem_wait(&(pool->nJob));
		DEBUG("Get new task to do! %ud\n", *pthread_self());
		DEBUG("Queue size: %ud\n", pool->jobs.size());
		pthread_mutex_lock(&(pool->mutex));
		int cur_job = pool->jobs.front();
		pool->jobs.pop();
		DEBUG("Queue size After pop: %ud\n", pool->jobs.size());
		pthread_mutex_unlock(&(pool->mutex));
		pool->exec_job(cur_job); //
	}
	return NULL;
}

thread_pool::thread_pool(server *s, int nThr) { // deque
	serv = s;
	nThread = nThr;
	pthread_mutex_init(&mutex, NULL);
	sem_init(&nJob, 0, 0);
	threads = new pthread_t[nThread]; //
}

void thread_pool::start() {
	for (int i = 0; i < nThread; i++)
		pthread_create(&threads[i], NULL, &func, this);
	cout << nThread << " threads created!\n";
}

void thread_pool::add_job(int fd) {
	DEBUG("Add job #%d\n", fd);
	pthread_mutex_lock(&mutex);
	jobs.push(fd);
	pthread_mutex_unlock(&mutex);
	sem_post(&nJob);
}

void thread_pool::exec_job(int fd) {
	DEBUG("exec_job #%d\n", fd);
	char *header = new char[SEGSIZE + 1] { 0 }; //
	int nread = read(fd, header, SEGSIZE);
	if (nread == -1) {
		perror("read1 error");
		serv->delete_event(fd,EPOLLIN); //删除监听
		close(fd); //记住close fd
		delete[] header;
	}
	else if (nread == 0) {
		// fprintf(stderr,"client close.\n");
		serv->delete_event(fd,EPOLLIN); //删除监听
		close(fd);
		delete[] header;
	}
	else {
		struct thread_params param = { fd, header, nread };
		void *status = accept_req((void *)&param);
		// to achieve connection-alive,
		// keep the connection until client ask to close
		// if (status == (void *)-1) {
			serv->delete_event(fd, EPOLLIN); //删除监听
			close(fd);
		// }
		delete[] header;
	}
}

/*
void thread_pool::delete_event(int epollfd, int fd, int state) {
	struct epoll_event ev;
	ev.events = state;
	ev.data.fd = fd;
	epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &ev);
}
*/

thread_pool::~thread_pool() {
	void *ret;
	for (int i = 0; i < nThread; i++)
		pthread_join(threads[i], &ret);
	printf("threads recycled!\n");
	delete[] threads;
}
