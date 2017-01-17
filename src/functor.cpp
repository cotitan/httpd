#include "functor.h"
#include "thread_pool.h"
#define DEBUG_MODE

#ifdef DEBUG_MODE
#define DEBUG printf
#else
#define DEBUG
#endif

#define SEGSIZE 1024

functor::functor(server *s) {
	serv = s;
}

void functor::operator()(int fd) {
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
