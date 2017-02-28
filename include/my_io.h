#ifndef MY_IO_H
#define MY_IO_H

ssize_t readn(int fd, void * data, int len);

ssize_t writen(int fd, const void *data, size_t n);


#endif
