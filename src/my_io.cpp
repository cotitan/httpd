#include "my_io.h"
#include <errno.h>
#include <unistd.h>

int readn(int fd, void * data, int len) {
	int nread;
	size_t nleft = len;
	while (nleft > 0) {
		if ( (nread = read(fd, data , len)) == -1) {
			if (errno == EAGAIN || errno == EINTR)
				nread = 0;
			else
				return -1;
		} else if (nread == 0) {
			break;
		}
		nleft -= nread;
		data += nread;
	}
	return len - nleft;
}

int writen(int fd, const void *data, size_t n) {
	size_t nleft = n;
	int nwritten;
	while (nleft > 0) {
		if ( (nwritten = write(fd, data, nleft)) <= 0) {
			if ( nwritten < 0 &&
				(errno == EINTR || errno == EAGAIN))
				nwritten = 0;
			else
				return -1;
		}
		nleft -= nwritten;
		data += nwritten;
	}
	return n;
}
