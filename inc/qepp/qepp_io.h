#ifndef QEPP_IO_H
#define QEPP_IO_H

#include <stdio.h>	//FILE, stderr, stdout, stdin
#include <stdlib.h>	//exit()
#include <string.h>	//strstr()
#include <poll.h>	//sruct pollfd, poll()
#include <unistd.h>	//getcwd

#include <qepp/qepp_mpi.h>
#include <qepp/qepp_file.h>
//#include <qepp/qepp_struct.h>

extern char io_buff[1024];
extern char outdir[1024];
extern char prefix[128];
extern char workdir[1024];
extern char datafile_path[1024];
extern FILE * in_f;
extern FILE * errf;
extern FILE * outf;
extern unsigned int ionode;
extern int verbosity;

#define JUST2(a, b, ...) (a), (b)

#define OPEN_IO_ENV( n, m, v) \
	open_io_env( n, m, v)
void open_io_env( int ionode, int mode, int verb);
#define CLOSE_IO_ENV() \
	close_io_env()
void close_io_env();

#define QEPP_ALL_PRINT( d, ...) \
	do { \
		if( verbosity >= 1) { \
			sprintf( io_buff, d, ##__VA_ARGS__); \
			qepp_print( -1, io_buff, NULL); \
		} \
	} while(0)
#define QEPP_PRINT( d, ...) \
	do { \
		if( verbosity >= 1) { \
			sprintf( io_buff, d, ##__VA_ARGS__); \
			qepp_print( ionode, io_buff, NULL); \
		} \
	} while(0)
#define QEPP_ALL_OUT( w, d, ...) \
	do { \
		if( verbosity >= 0) { \
			sprintf( io_buff, d, ##__VA_ARGS__); \
			qepp_print( -1, io_buff, w); \
		} \
	} while(0)
#define QEPP_OUT( w, d, ...) \
	do { \
		if( verbosity >= 0) { \
			sprintf( io_buff, d, ##__VA_ARGS__); \
			qepp_print( ionode, io_buff, w); \
		} \
	} while(0)
void qepp_print( int proc, void * data, FILE * w);






#endif //QEPP_IO_H

















