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
#define BASE_DIM(a) (*((long int **)a - 1))[0]
#define BASE_SIZE(a) (*((long int **)a - 1))[1]
#define BASE_CHECK(a) (*((long int **)a - 1))[2]
#endif

#ifndef MEM_WRAP
#define MEM_WRAP

#define alloc1(a,b) AllocateLinearMem1(a,b)
#define alloc2(a,b,c) AllocateLinearMem2(a,b,c)
#define alloc3(a,b,c,d) AllocateLinearMem3(a,b,c,d)
#define alloc4(a,b,c,d,e) AllocateLinearMem4(a,b,c,d,e)
#define duplicate1(a) DuplicateLinearMem1(a)
#define duplicate2(a) DuplicateLinearMem2(a)
#define duplicate3(a) DuplicateLinearMem3(a)
#define duplicate4(a) DuplicateLinearMem4(a)
#define free1(a) FreeLinearMem1(a)
#define free2(a) FreeLinearMem2(a)
#define free3(a) FreeLinearMem3(a)
#define free4(a) FreeLinearMem4(a)

#define QEPP_free( a) FreeLinearMem( a)
#define QEPP_dupl( a) DuplicateLinearMem( a)
#define QEPP_alloc( a, b, ...) malloc_app( a, b, ##__VA_ARGS__, 0, 0, 0)
#define malloc_app(a, b, c, d, e, ...) \
	( c != 0 ? \
		( d != 0 ? \
			( e != 0 ? \
				alloc4( a, b, c, d, e) : \
				alloc3( a, b, c, d) \
			) : \
			alloc2( a, b, c) \
		) : \
		alloc1( a, b) \
	)
#define malloc( a) malloc_b( a)
/*
#define malloc( a) \
	malloc( a); \
	TOT_MEM += a
*/
#endif //MEM_WRAP

long int get_size( void * a);
void * get_base( void * a);

void * AllocateLinearMem1( unsigned long int, long int);
void * AllocateLinearMem2( unsigned long int, long int, long int);
void * AllocateLinearMem3( unsigned long int, long int, long int, long int);
void * AllocateLinearMem4( unsigned long int, long int, long int, long int, long int);

void * DuplicateLinearMem( void *);
void * DuplicateLinearMem1( void *);
void * DuplicateLinearMem2( void **);
void * DuplicateLinearMem3( void ***);
void * DuplicateLinearMem4( void ****);

void FreeLinearMem( void *);
void FreeLinearMem1( void *);
void FreeLinearMem2( void **);
void FreeLinearMem3( void ***);
void FreeLinearMem4( void ****);

void * malloc_b( size_t a);




#endif








