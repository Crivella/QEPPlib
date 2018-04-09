#ifndef QEPP_IO_H
#define QEPP_IO_H

#include <stdio.h>	//FILE, stderr, stdout, stdin
#include <stdlib.h>	//exit()
#include <string.h>	//strstr()
#include <poll.h>	//sruct pollfd, poll()
#include <unistd.h>	//getcwd

#include <qepp/qepp_mpi.h>
#include <qepp/qepp_file.h>
#include <qepp/qepp_struct.h>

#define JUST2(a, b, ...) (a), (b)

#define OPEN_IO_ENV( n, m, v) \
	open_io_env( n, m, v)
void open_io_env( int ionode, int mode, int verb);
void close_io_env();

#define QEPP_ALL_PRINT( d, ...) \
	do { \
		if( verbosity >= 1) { \
			char buff[256]; \
			sprintf( buff, d, ##__VA_ARGS__); \
			qepp_print( -1, buff, NULL); \
		} \
	} while(0)
#define QEPP_PRINT( d, ...) \
	do { \
		if( verbosity >= 1) { \
			char buff[256]; \
			sprintf( buff, d, ##__VA_ARGS__); \
			qepp_print( ionode, buff, NULL); \
		} \
	} while(0)
#define QEPP_OUT( w, d, ...) \
	do { \
		if( verbosity >= 0) { \
			char buff[256]; \
			sprintf( buff, d, ##__VA_ARGS__); \
			qepp_print( ionode, buff, w); \
		} \
	} while(0)
void qepp_print( int proc, void * data, FILE * w);






#endif //QEPP_IO_H

















