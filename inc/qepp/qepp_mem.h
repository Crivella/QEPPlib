#ifndef QEPP_MEM_H
#define QEPP_MEM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifndef ARRAY_SIZE_MACRO
#define ARRAY_SIZE_MACRO
#define BASE_ADDR(a) (long int *)a - 1
#define BASE_NUM(a) get_size( a)
#define BASE_DIM(a) *((long int **)a - 1)[0]
#define BASE_SIZE(a) *((long int **)a - 1)[1]
#endif

#ifndef MEM_WRAP
#define MEM_WRAP
#define SECOND(a, b, ...) b
#define THIRD(a, b, c, ...) c
#define FOURTH(a, b, c, d, ...) d
#define FIFTH(a, b, c, d, e, ...) e
/*#define Allocate( a, b, ...) \
	if( SECOND( 0, ##__VA_ARGS__, 0) == 0) \
		AllocateLinearMem1( a, b); \
	else if( THIRD(0, ##__VA_ARGS__, 0) == 0) \
		AllocateLinearMem2( a, b, __VA_ARGS__); \
	else if( FOURTH(0, ##__VA_ARGS__, 0) == 0) \
		AllocateLinearMem3( a, b, __VA_ARGS__); \
	else if( FIFTH(0, ##__VA_ARGS__, 0) == 0) \
		AllocateLinearMem4( a, b, __VA_ARGS__); \
	fi*/
#define alloc1(a,b) AllocateLinearMem1(a,b)
#define alloc2(a,b,c) AllocateLinearMem1(a,b,c)
#define alloc3(a,b,c,d) AllocateLinearMem1(a,b,c,d)
#define alloc4(a,b,c,d,e) AllocateLinearMem1(a,b,c,d,e)
#define duplicate1(a,b) DuplicateLinearMem1(a,b)
#define duplicate2(a,b,c) DuplicateLinearMem1(a,b,c)
#define duplicate3(a,b,c,d) DuplicateLinearMem1(a,b,c,d)
#define duplicate4(a,b,c,d,e) DuplicateLinearMem1(a,b,c,d,e)
#define free1(a,b) FreeLinearMem1(a,b)
#define free2(a,b,c) FreeLinearMem1(a,b,c)
#define free3(a,b,c,d) FreeLinearMem1(a,b,c,d)
#define free4(a,b,c,d,e) FreeLinearMem1(a,b,c,d,e)
#define malloc( a) \
	malloc( a); \
	TOT_MEM += a
#endif

long int get_size( void * a);

void * AllocateLinearMem1( unsigned long int, long int);
void ** AllocateLinearMem2( unsigned long int, long int, long int);
void *** AllocateLinearMem3( unsigned long int, long int, long int, long int);
void **** AllocateLinearMem4( unsigned long int, long int, long int, long int, long int);

void * DuplicateLinearMem1( void *);
void ** DuplicateLinearMem2( void **);
void *** DuplicateLinearMem3( void ***);
void **** DuplicateLinearMem4( void ****);

void FreeLinearMem1( void *);
void FreeLinearMem2( void **);
void FreeLinearMem3( void ***);
void FreeLinearMem4( void ****);




#endif








