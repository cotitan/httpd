#include <fstream>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include "httpMessage.h"
using namespace http;
using namespace std;

#ifndef SEGSIZE
	#define SEGSIZE 102400
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

int send_resp(int fd, int code, const char *state,
	const char *type, const char *datapath) {

	httpResponse *res = nullptr;
	int status = 0;

	if (datapath == nullptr) {
		res = new httpResponse(code, state, type, 0);
		if (res->send_head(fd) == -1)
			status = -1;
	} else {
		size_t len = get_file_size(datapath);
		char *buf = new char[len + 1];
		res = new httpResponse(code, state, type, len);
		res->send_head(fd);
		ifstream fin(datapath);
		// fin.read(buf, SEGSIZE);
		size_t count = 0;
		while (!fin.eof() && count < len) {
			fin.read(buf + count, len);
			count += fin.gcount();
		}
		fin.close();

		if (write(fd, buf, len) == -1) {
			status = -1;
			perror("fail to write: ");
			close(fd);
			// delete event
		}
		delete[] buf;
	}

	delete res;
	return status;
}
