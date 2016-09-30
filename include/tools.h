#ifndef _TOOLS_H
#define _TOOLS_H

#include <fstream>
#include <iostream>
#include <cstring>
#include "httpMessage.h"
#include "controller.h"
using namespace http;
using namespace std;

#ifndef SEGSIZE
	#define SEGSIZE 10240
#endif

inline int min(int x, int y) {
	return x < y ? x : y;
}

size_t get_file_size(const char *file) {
	ifstream fin(file);
	if (!fin.is_open())
		return 0;
	fin.seekg(0, ios::end);
	size_t len = fin.tellg();
	fin.close();
	return len;
}

void send_resp(int fd, int code, const char *state,
	const char *type, const char *datapath) {

	char buf[SEGSIZE];
	size_t len = get_file_size(datapath);
	httpResponse res(code, state, type, len);
	res.send_head(fd);

	if (len == 0)
		return;

	ifstream fin(datapath);
	// fin.read(buf, SEGSIZE);
	while (!fin.eof()) {
		fin.read(buf, SEGSIZE);
		write(fd, buf, fin.gcount());
	}
	// write(fd, buf, strlen(buf));
	fin.close();
}

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
}

void *accept_req(void* fd) {
	int connfd = *(int *)fd, nread = 0;
	char header[SEGSIZE + 1], *pos;
	header[SEGSIZE] = 0;
	do {
		nread += read(connfd, header + nread, SEGSIZE);
		header[nread] = 0;
	} while (nread && (pos = strstr(header, "\r\n\r\n")) == NULL);

	if (nread == 0) {
		pthread_exit((void *)-1);
	}

	httpRequest req(header);

	if (req.getMethod() == POST) {
		int content_length = req.getContentLength();
		char *data = new char[content_length + 1];
		if (data == NULL)
			cout << "fail to allocate memory for new\n";

		nread -= pos + 4 - header;
		strncpy(data, pos + 4, nread);
		while (nread < content_length) {
			nread += read(connfd, data + nread, min(SEGSIZE, content_length - nread));
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
	// delete[] data; // will be deleted in ~httpRequest
	pthread_exit((void *)0);
}

#endif
