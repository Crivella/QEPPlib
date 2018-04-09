#ifndef QEPP_ERR_H
#define QEPP_ERR_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>	//malloc(),
#include <stdbool.h>

#ifdef __MPI
#include <qepp/qepp_mpi.h>
#endif //__MPI

typedef enum {	SUCCESS,
		WARNING,
		//MSG,
		FAIL,

		NULL_IN,
		NULL_OUT,
		OPEN_IN_FAIL,
} ecode;

typedef struct error_handle errh;

#define SECOND( a, b, ...) b
#define THIRD( a, b, c, ...) c
#define SKIP( dump, b, ...) __VA_ARGS__

#define PRINT_ERRH( a, ...) parse_errh( set_errh( a, __func__, ##__VA_ARGS__))
/*#define MSG( a, ...) \
	do { \
		char buff[256]; \
		sprintf( buff, a, ##__VA_ARGS__); \
		parse_errh( set_errh( MSG, __func__, buff)); \
	} while(0)*/
//#define WARN( a) parse_errh( set_errh( WARNING, __func__, a))
#define WARN( a, ...) \
	do { \
		char buff[256]; \
		sprintf( buff, a, ##__VA_ARGS__); \
		parse_errh( set_errh( WARNING, __func__, buff)); \
	} while(0)

#define FAIL( a, b, ...) \
	do { \
		char buff[256]; \
		if( b != NULL) \
			sprintf( buff, b, ##__VA_ARGS__); \
		return set_errh( a, __func__, buff); \
	} while(0)

#define SUCCESS() \
	return set_errh( SUCCESS, __func__, NULL)
/*#define SET_ERRH( a, ...) \
	set_errh( a, __func__, SECOND( 0, ##__VA_ARGS__, NULL))*/
errh * set_errh( ecode, const char [], const char []);

struct error_handle {
	char cname[256];
	bool ext;
	char extra[256];
	ecode code;
};

ecode parse_errh( errh *);






/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Ansii escape codes
#ifndef ANSII_ESCAPE_CODES
#define ANSII_ESCAPE_CODES
#define ANSII_UP		"\x1B[A"

#define ANSII_RESET		"\x1B[0m"

#define ANSII_BOLD		"\x1B[1m"
#define ANSII_UNDERSCORE	"\x1B[4m"
#define ANSII_BLINK		"\x1B[5m"
#define ANSII_REV_VID		"\x1B[7m"
#define ANSII_CONCEAL		"\x1B[8m"

#define ANSII_F_BLACK 		"\x1B[30m"
#define ANSII_F_RED		"\x1B[31m"
#define ANSII_F_GREEN		"\x1B[32m"
#define ANSII_F_YELLLOW		"\x1B[33m"
#define ANSII_F_BLUE		"\x1B[34m"
#define ANSII_F_MAGENTA		"\x1B[35m"
#define ANSII_F_CYAN		"\x1B[36m"
#define ANSII_F_WHITE		"\x1B[37m"

#define ANSII_F_BRIGHT_RED	"\x1B[38;2;255;0;0m"
#define ANSII_F_BRIGHT_GREEN	"\x1B[38;2;0;255;0m"
#define ANSII_F_BRIGHT_BLUE	"\x1B[38;2;0;0;255m"
#define ANSII_F_BRIGHT_CYAN	"\x1B[38;2;0;255;255m"

#define ANSII_B_BLACK 		"\x1B[40m"
#define ANSII_B_RED		"\x1B[41m"
#define ANSII_B_GREEN		"\x1B[42m"
#define ANSII_B_YELLLOW		"\x1B[43m"
#define ANSII_B_BLUE		"\x1B[44m"
#define ANSII_B_MAGENTA		"\x1B[45m"
#define ANSII_B_CYAN		"\x1B[46m"
#define ANSII_B_WHITE		"\x1B[47m"

#endif //ANSI_ESCAPE_CODES



#endif //QEPP_ERR_H








