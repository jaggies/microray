/*
 * armsupport.cpp
 *
 *  Created on: Apr 23, 2020
 *      Author: jmiller
 */

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

// Basic compatibility functions when compiling on an 
// embedded ARM target, like STM32F415

void exit(int code) {
	// TODO
	while (1) 
		;
}

int _fstat(int fd, struct stat *statbuf) {
	// TODO
	return 0;
}

int _open(const char *pathname, int flags, mode_t mode) {
	// TODO
	return -1;
}

void* _sbrk(ptrdiff_t __incr) {
	// TODO
	return 0;
}

ssize_t _write(int fd, const void *buf, size_t count) {
	// TODO
	return 0;
}

int _close(int fd) {
	// TODO
	return -1;
}

int _isatty(int fd) {
	// TODO
	return 0;
}

off_t _lseek(int fd, off_t offset, int whence) {
	// TODO 
	return 0;
}

ssize_t _read(int fd, void *buf, size_t count) {
	// TODO
	return 0;
}

