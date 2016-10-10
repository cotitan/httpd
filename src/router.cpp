#include "controller.h"
#include <cstring>
#include <cstdio>
#include <unistd.h>

#ifndef SEGSIZE
	#define SEGSIZE 10240
#endif

void route(int connfd, const httpRequest &req) {
	string url = req.getUrl();

	controller *ctrller = NULL;

	size_t epos = url.find('/', 0 + 1);
	// if on root directory
	if (epos == string::npos) {
		ctrller = new controller;
	} else if (url.substr(0, 8) == "/picture") {
		ctrller = new pic_controller;
	}

	ctrller->handle(connfd, req);
	delete ctrller;
}

void *accept_req(void* fd) {
	int connfd = *(int *)fd, nread = 0;
	char header[SEGSIZE + 1], *pos;
	header[SEGSIZE] = 0;
	do {
		nread += read(connfd, header + nread, SEGSIZE);
		header[nread] = 0;
	} while (nread && (pos = strstr(header, "\r\n\r\n")) == NULL);

	// printf("%s\n", header);

	if (nread == 0) {
		pthread_exit((void *)-1);
	}

	httpRequest req(header);

	if (req.getMethod() == POST) {
		int content_length = req.getContentLength();
		char *data = new char[content_length + 1];
		if (data == NULL)
			printf("fail to allocate memory for new\n");

		nread -= pos + 4 - header;
		memcpy(data, pos + 4, nread);
		while (nread < content_length) {
			nread += read(connfd, data + nread,
				min(SEGSIZE, content_length - nread));
			data[nread] = 0;
		}

		req.setData(data);

		/*
		if (req.getMethod() == POST) {
			string url = req.getUrl();
			const char *path = url.substr(1, url.length() - 1).c_str();
			ofstream fout(path, ios::out | ios::trunc);
			if (fout.is_open()) {
				fout.write(data, content_length);
				fout.close();
			}
		}
		send_resp(connfd, 200, "OK");
		*/
	}

	route(connfd, req);
	// delete[] data; // will be deleted in httpRequest::~httpRequest()
	pthread_exit((void *)0);
}
