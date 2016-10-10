#include <fstream>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include "httpMessage.h"
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
	httpResponse *res = nullptr;

	if (datapath == nullptr) {
		res = new httpResponse(code, state, type, 0);
		res->send_head(fd);
	} else {
		size_t len = get_file_size(datapath);
		res = new httpResponse(code, state, type, len);
		res->send_head(fd);
		ifstream fin(datapath);
		// fin.read(buf, SEGSIZE);
		while (!fin.eof()) {
			fin.read(buf, SEGSIZE);
			write(fd, buf, fin.gcount());
		}
		// write(fd, buf, strlen(buf));
		fin.close();
	}

	delete res;	
}
