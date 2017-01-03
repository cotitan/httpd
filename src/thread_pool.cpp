#include "thread_pool.h"

thread_pool::thread_pool(int epfd, int fd) {

	nThread = nThr;
	epfd = epollfd;
	pthread_mutex_init(&mutex, NULL);
	sem_init(&nJob, 0, 0);
	pthread_t *threads = new pthread_t[nThread];
	for (int i = 0; i < nThread; i++)
		pthread_create(&threads[i], NULL, &func, NULL);
}

void thread_pool::add_job(int fd) {
	
	pthread_mutex_lock(&mutex);
	jobs.push(fd);
	pthread_mutex_unlock(&mutex);
	sem_post(&nJob);
	
}

void thread_pool::exec_job(int fd) {
	char *header = new char[SEGSIZE + 1] { 0 };
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


void thread_pool::delete_event(int epollfd, int fd, int state) {
	struct epoll_event ev;
	ev.events = state;
	ev.data.fd = fd;
	epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &ev);
}

void *func(void *args) {
	while (true) {
		sem_wait(&nJob);
		pthread_mutex_lock(&mutex);
		job cur_job = jobs.front();
		jobs.pop();
		pthread_mutex_unlock(&mutex);
		exec_job(cur_job);
	}
}
