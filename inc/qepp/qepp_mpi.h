#ifndef QEPP_MPI_H
#define QEPP_MPI_H

typedef struct mpi_data mpi_data;

#include <stdio.h>	//fprintf(), fopen(), fclose()
#include <string.h>	//memcpy()
#include <stdlib.h>	//malloc(), calloc()
#include <assert.h>	//assert()
#include <complex.h>
#include <qepp/qepp_mem.h>

extern mpi_data * mpi;
#define ID_MPI_DATA 64


#ifdef __MPI
#include <mpi.h>	//MPI_Comm_size(), MPI_Comm_rank(), MPI_Get_processor_name()
#include <qepp/mpi_base.h>

#ifndef ARRAY_SIZE_MACRO //ARRAY_SIZE_MACRO
#define ARRAY_SIZE_MACRO
#define BASE_ADDR(a) (long int *)a - 1
#define BASE_NUM(a) mpi_get_size( a)
#define BASE_DIM(a) *((long int **)a - 1)[0]
#define BASE_SIZE(a) *((long int **)a - 1)[1]
#endif //ARRAY_SIZE_MACRO

long int mpi_get_size( void * a);

#define SECOND(a, b, ...) b
#define THIRD(a, b, c , ...) c


#define mp_sum(a,b, ...) \
	_Generic((a), \
		int *:			mp_sum_iv((int *)a, b, SECOND(0, ##__VA_ARGS__, 0)), \
		int **:			mp_sum_iv((int *)a, b, SECOND(0, ##__VA_ARGS__, 0)), \
		int ***:		mp_sum_iv((int *)a, b, SECOND(0, ##__VA_ARGS__, 0)), \
		int ****:		mp_sum_iv((int *)a, b, SECOND(0, ##__VA_ARGS__, 0)), \
		long int *:		mp_sum_liv((long int *)a, b, SECOND(0, ##__VA_ARGS__, 0)), \
		long int **:		mp_sum_liv((long int *)a, b, SECOND(0, ##__VA_ARGS__, 0)), \
		long int ***:		mp_sum_liv((long int *)a, b, SECOND(0, ##__VA_ARGS__, 0)), \
		long int ****:		mp_sum_liv((long int *)a, b, SECOND(0, ##__VA_ARGS__, 0)), \
		double *:		mp_sum_dv((double *)a, b, SECOND(0, ##__VA_ARGS__, 0)), \
		double **:		mp_sum_dv((double *)a, b, SECOND(0, ##__VA_ARGS__, 0)), \
		double ***:		mp_sum_dv((double *)a, b, SECOND(0, ##__VA_ARGS__, 0)), \
		double ****:		mp_sum_dv((double *)a, b, SECOND(0, ##__VA_ARGS__, 0)), \
		double complex *:	mp_sum_cv((double complex *)a, b, SECOND(0, ##__VA_ARGS__, 0)), \
		double complex **:	mp_sum_cv((double complex *)a, b, SECOND(0, ##__VA_ARGS__, 0)), \
		double complex ***:	mp_sum_cv((double complex *)a, b, SECOND(0, ##__VA_ARGS__, 0)), \
		double complex ****:	mp_sum_cv((double complex *)a, b, SECOND(0, ##__VA_ARGS__, 0)), \
		default:		fprintf(stderr, "mp_sum: Type not defined\n") \
	)

#define mp_bcast(a,b, ...) \
	_Generic((a), \
		char *:			mp_bcast_char((char *)a, b, SECOND(0, ##__VA_ARGS__, 0)), \
		int *:			mp_bcast_iv((int *)a, b, SECOND(0, ##__VA_ARGS__, 0)), \
		int **:			mp_bcast_iv((int *)a, b, SECOND(0, ##__VA_ARGS__, 0)), \
		int ***:		mp_bcast_iv((int *)a, b, SECOND(0, ##__VA_ARGS__, 0)), \
		int ****:		mp_bcast_iv((int *)a, b, SECOND(0, ##__VA_ARGS__, 0)), \
		long int *:		mp_bcast_liv((long int *)a, b, SECOND(0, ##__VA_ARGS__, 0)), \
		long int **:		mp_bcast_liv((long int *)a, b, SECOND(0, ##__VA_ARGS__, 0)), \
		long int ***:		mp_bcast_liv((long int *)a, b, SECOND(0, ##__VA_ARGS__, 0)), \
		long int ****:		mp_bcast_liv((long int *)a, b, SECOND(0, ##__VA_ARGS__, 0)), \
		double *:		mp_bcast_dv((double *)a, b, SECOND(0, ##__VA_ARGS__, 0)), \
		double **:		mp_bcast_dv((double *)a, b, SECOND(0, ##__VA_ARGS__, 0)), \
		double ***:		mp_bcast_dv((double *)a, b, SECOND(0, ##__VA_ARGS__, 0)), \
		double ****:		mp_bcast_dv((double *)a, b, SECOND(0, ##__VA_ARGS__, 0)), \
		double complex *:	mp_bcast_cv((double complex *)a, b, SECOND(0, ##__VA_ARGS__, 0)), \
		double complex **:	mp_bcast_cv((double complex *)a, b, SECOND(0, ##__VA_ARGS__, 0)), \
		double complex ***:	mp_bcast_cv((double complex *)a, b, SECOND(0, ##__VA_ARGS__, 0)), \
		double complex ****:	mp_bcast_cv((double complex *)a, b, SECOND(0, ##__VA_ARGS__, 0)), \
		default:		fprintf(stderr, "mp_bcast: Type not defined\n") \
	)

#define SET_PW_MPI( a, b) \
	if( true) { \
		long int app1 = a, app2 = b; \
		long int n_div = ceil( b / mpi->world_size); \
		a = mpi->world_rank * n_div - 1; \
		if( mpi->world_rank > 0) \
			a++; \
		b = (mpi->world_rank+1) * n_div; \
		if( mpi->world_rank == 0 ) \
			a = app1; \
		if( mpi->world_rank+1 == mpi->world_size) \
			b = app2; \
	}



//SECOND(0, ##__VA_ARGS__, MPI_COMM_WORLD)

int mp_sum_iv( int * a, MPI_Comm comm, int);
int mp_sum_liv( long int * a, MPI_Comm comm, int);
int mp_sum_dv( double * a, MPI_Comm comm, int);
int mp_sum_cv( double complex * a, MPI_Comm comm, int);


int mp_bcast_char( char *a, MPI_Comm comm, int);
int mp_bcast_iv( int * a, MPI_Comm comm, int);
int mp_bcast_liv( long int * a, MPI_Comm comm, int);
int mp_bcast_dv( double * a, MPI_Comm comm, int);
int mp_bcast_cv( double complex * a, MPI_Comm comm, int);


struct mpi_data
{
	int typeID;
	size_t size;

	int world_rank;
	int world_size;
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	int name_len;
	int ionode;

	void * 		(*print)( mpi_data *, FILE *);
	mpi_data *	(*duplicate)( mpi_data *);
	void * 		(*free)( mpi_data *);
	size_t		(*totmem)( mpi_data *);
};



#else //__MPI ***************************************************************************************************************

#define SET_PW_MPI( ...) ;
#define mp_sum(a,b, ...) ;
#define mp_bcast(a,b, ...) ;
#define MPI_COMM_WORLD 0

struct mpi_data
{
	int typeID;
	size_t size;

	int world_rank;
	int world_size;

	void * 		(*print)( mpi_data *, FILE *);
	mpi_data *	(*duplicate)( mpi_data *);
	void * 		(*free)( mpi_data *);
	size_t		(*totmem)( mpi_data *);
};

#endif //__MPI ***************************************************************************************************************

/*****************************************************************************************************************************************/


mpi_data * initialize_mpi_data();
void * print_mpi_data( mpi_data * to_print, FILE * write);
mpi_data * duplicate_mpi_data( mpi_data * to_dupl);
void * free_mpi_data( mpi_data * to_free);


#endif //QEPP_MPI_H

















