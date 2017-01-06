#include "controller.h"
#include <cstring>
#include <cstdio>
#include <unistd.h>

#ifndef SEGSIZE
	#define SEGSIZE 10240
#endif

struct thread_params {
	int connfd;
	char *header;
	int len;
};

int route(int connfd, const httpRequest &req) {
	string url = req.getUrl();

	controller *ctrller = NULL;

	size_t epos = url.find('/', 0 + 1);
	// if on root directory
	if (epos == string::npos) {
		ctrller = new controller;
	} else if (url.substr(0, 8) == "/picture") {
		ctrller = new pic_controller;
	}

	int status = ctrller->handle(connfd, req);
	delete ctrller;
	return status;
}

void *accept_req(void* param_) {
	// pthread_detach(pthread_self());
	struct thread_params param = *(struct thread_params *)param_;
	int connfd = param.connfd;
	char *header = param.header, *pos;
	int nread = param.len;
	if ((pos = strstr(header, "\r\n\r\n")) == NULL) {
		return (void *)-1;
	}
	/*
	while (nread && nread <= SEGSIZE
			&& (pos = strstr(header, "\r\n\r\n")) == NULL) {
		nread += read(connfd, header + nread, SEGSIZE);
		header[nread] = 0;
	}
	*/
	// printf("%s\n", header);

	httpRequest req(header);

	if (req.getMethod() == ERR) {
		return (void *) -1;
	}

	// printf("%d %s\n", req.getMethod(), req.getUrl().c_str());
	if (req.getMethod() == POST) {
		int content_length = req.getContentLength();
		char *data = new char[content_length + 1];
		if (data == NULL)
			perror("fail to allocate memory for new\n");

		nread -= pos + 4 - header;
		memcpy(data, pos + 4, nread);
		while (nread < content_length) {
			nread += read(connfd, data + nread,
				min(SEGSIZE, content_length - nread));
			data[nread] = 0;
		}

		req.setData(data);
	}
	
	if (route(connfd, req) == -1)
		return (void *) -1;
	// delete[] data; // will be deleted in httpRequest::~httpRequest()
	// pthread_exit((void *)0);
	return NULL;
}
