#ifndef QEPP_MEM_H
#define QEPP_MEM_H

#include <stdlib.h>	//malloc(), free(), exit()
#include <string.h>	//memcpy()
#include <assert.h>	//assert()

/*-------------------------------------------------------------------------*/
//  To allow the implementation of mpi routines, each multidimensional     //
//  array should be allocated on a contiguos block of memory.              //
//  The function declared in 'qepp_mem.h' do just that. In addition they   //
//  store information on the array in a long int array, the pointer with   //
//  a pointer stored in 8byte of memory allocate just before the block     //
//  data.                                                                  //
/*-------------------------------------------------------------------------*/
//  Here is some examples of the data structures allocated by 'qepp_mem.h' //
//   dim   = dimensionality of array (1->a[], 2->a[][], 3->a[][][], 4->a[][][][]
//           dim will also be used by intermediated 1dimensional array with
//           check set to 0
//   size  = size of the elements of the array
//   check = check variable for function and macros to work properly
//           if 1, dim and s. will contain the information of the full vector
//   s1    = max size for 1st dimension
//   s.    = max size for ... dimension

//                                 1D-array                                //
//                                 eg a[3]
/*                 ├ ┤ ┬ ┴  └ ┘ ┌ ┐

     ┌----------┐    ┌-------┬------┬-------┬----┐
┌--->|long int *| -> | dim=1 | size | check | s1 |
|    |  infos   |    └-------┴------┴-------┴----┘
|    ├----------┤
| ┌->|   type   |[0]
| |  ├----------┤
| |  |   type   |[1]
| |  ├----------┤      
| |  |   type   |[2]
| |  └----------┘
| |
| └--return pointer to
|
└----malloc( 8 + size*num)
-----------------------------------------------------------------------------
//                                 2D-array                                //
                                   eg a[2][3]

                        [0]     [1]    [2]    [3]
     ┌----------┐    ┌-------┬------┬-------┬----┐
┌--->|long int *| -> | dim=1 | size | check | s2 |
|    |  infos   |    └-------┴------┴-------┴----┘              [0]     [1]    [2]    [3]  [4]
|    ├----------┤                            ┌----------┐    ┌-------┬------┬-------┬----┬----┐                       
| ┌->|  type *  |[0]------------------------>|long int *| -> | dim=2 | size | check | s1 | s2 |
| |  ├----------┤                            |  infos   |    └-------┴------┴-------┴----┴----┘
| |  |  type *  |[1]-------\                 ├----------┤
| |  └----------┘           \                |   type   |[0][0]
| |                          \               ├----------┤
| └--return pointer to        \              |   type   |[0][1]
|                              \             ├----------┤
└----malloc( 8 + size*num)      \            |   type   |[0][2]
                                 \           ├----------┤
                                  \--------->|   type   |[1][0]
                                             ├----------┤
                                             |   type   |[1][1]
                                             ├----------┤
                                             |   type   |[1][2]
                                             └----------┘

-----------------------------------------------------------------------------
//                                 3D-array                                //
                                   eg a[2][3][2]

                        [0]     [1]    [2]    [3]
     ┌----------┐    ┌-------┬------┬-------┬----┐
┌--->|long int *| -> | dim=1 | size | check | s3 |
|    |  infos   |    └-------┴------┴-------┴----┘              [0]     [1]    [2]    [3]
|    ├----------┤                            ┌----------┐    ┌-------┬------┬-------┬----┐                       
| ┌->|  type ** |[0]--------------\          |long int *| -> | dim=2 | size | check | s2 |
| |  ├----------┤                  \         |  infos   |    └-------┴------┴-------┴----┘             [0]     [1]    [2]    [3]  [4]  [5]
| |  |  type ** |[1]-------\        \        ├----------┤                            ┌----------┐    ┌-------┬------┬-------┬----┬----┬----┐ 
| |  └----------┘           \        \------>|  type *  |[0][0]-----------\          |long int *| -> | dim=2 | size | check | s1 | s2 | s3 |
| |                          \               ├----------┤                  \         |  infos   |    └-------┴------┴-------┴----┴----┴----┘
| └--return pointer to        \              |  type *  |[0][1]----------\  \        ├----------┤
|                              \             ├----------┤                 \  \------>|   type   |[0][0][0]
└----malloc( 8 + size*num)      \            |  type *  |[0][2]---------\  \         ├----------┤
                                 \           └----------┘                \  \        |   type   |[0][0][1]
                                  \          ┌----------┐                 \  \       ├----------┤
                                   \         |long int *| -> ....          \  \----->|   type   |[0][1][0]
                                    \        |  infos   |                   \        ├----------┤
                                     \       ├----------┤                    \       |   type   |[0][1][1]
                                      \----->|  type *  |[1][0]------------\  \      ├----------┤
                                             ├----------┤                   \  \---->|   type   |[0][2][0]
                                             |  type *  |[1][1]----------\   \       ├----------┤
                                             ├----------┤                 \   \      |   type   |[0][2][1]
                                             |  type *  |[1][2]------\     \   \     ├----------┤
                                             └----------┘             \     \   \--->|   type   |[1][0][0]
                                                                       \     \       ├----------┤
                                                                        \     \      |   type   |[1][0][1]
                                                                         \     \     ├----------┤
                                                                          \     \--->|   type   |[1][1][0]
                                                                           \         ├----------┤
                                                                            \        |   type   |[1][1][1]
                                                                             \       ├----------┤
                                                                              \----->|   type   |[1][2][0]
                                                                                     ├----------┤
                                                                                     |   type   |[1][2][1]
                                                                                     └----------┘


*/

#ifndef ARRAY_SIZE_MACRO
#define ARRAY_SIZE_MACRO

/*-------------------------------------------------------------------------*/
/**
  @name     BASE_ADDR
  @brief    Get true address of the allocated memory for array 'a'

  @param    Address of linear block of memory allocated usind 'qepp_mem.h'

  

  return 0 if successfull or 1 if failed
 */
/*-------------------------------------------------------------------------*/
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








