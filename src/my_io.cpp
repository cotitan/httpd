#include "my_io.h"
#include <errno.h>
#include <unistd.h>

ssize_t readn(int fd, void * data, int len) {
	ssize_t nread;
	size_t nleft = len;
	while (nleft > 0) {
		if ( (nread = read(fd, data , len)) == -1) {
			if (errno == EINTR)
				nread = 0;
			else if (errno == EAGAIN)
				break;
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

ssize_t writen(int fd, const void *data, size_t n) {
	size_t nleft = n;
	ssize_t nwritten;
	while (nleft > 0) {
		if ( (nwritten = write(fd, data, nleft)) <= 0) {
			if ( nwritten < 0 && errno == EINTR )
				nwritten = 0;
			else
				return -1;
		}
		nleft -= nwritten;
		data += nwritten;
	}
	return n;
}
