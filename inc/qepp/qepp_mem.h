#ifndef QEPP_MEM_H
#define QEPP_MEM_H

#include <stdlib.h>	//malloc(), free(), exit()
#include <string.h>	//memcpy()
#include <assert.h>	//assert()

#ifndef ARRAY_SIZE_MACRO
#define ARRAY_SIZE_MACRO
// MACRO BASE_ADDR(a)
// Get true address of the allocated memory for array 'a'
#define BASE_ADDR(a) (long int *)a - 1
// MACRO BASE_NUM(a)
// Get size of the allocated memory for array 'a'
#define BASE_NUM(a) qepp_mem_get_size( a)
// MACRO BASE_DIM(a)
// Get dimensionality of the allocated array 'a'
#define BASE_DIM(a) (*((long int **)a - 1))[0]
// MACRO BASE_ADDR(a)
// Get size of the elements of array 'a'
#define BASE_SIZE(a) (*((long int **)a - 1))[1]
// MACRO BASE_CHECK(a)
// Get check variable for handling multidimensional array
#define BASE_CHECK(a) (*((long int **)a - 1))[2]
#endif

#ifndef MEM_WRAP
#define MEM_WRAP

#define QEPP_FREE( a) FreeLinearMem( a)
#define QEPP_DUPL( a) DuplicateLinearMem( a)
#define QEPP_ALLOC( a, b, ...) malloc_app( a, b, ##__VA_ARGS__, 0, 0, 0)
#define malloc_app(a, b, c, d, e, ...) \
	( c != 0 ? \
		( d != 0 ? \
			( e != 0 ? \
				AllocateLinearMem4( a, b, c, d, e) : \
				AllocateLinearMem3( a, b, c, d) \
			) : \
			AllocateLinearMem2( a, b, c) \
		) : \
		AllocateLinearMem1( a, b) \
	)
#define malloc( a) malloc_b( a)
/*
#define malloc( a) \
	malloc( a); \
	TOT_MEM += a
*/
#endif //MEM_WRAP

extern unsigned long int TOT_MEM;

long int qepp_mem_get_size( void * a);
void * qepp_mem_get_base( void * a);

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








