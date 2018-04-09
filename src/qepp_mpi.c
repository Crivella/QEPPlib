#include <qepp/qepp_mpi.h>

#ifdef __MPI

extern mpi_data * mpi;
extern unsigned int ionode;

long int mpi_get_size( void * a)
{
	long int res=1;
	int dim = BASE_DIM(a);
	long int * ptr = *((long int **)BASE_ADDR(a));

	for( int i=0; i<dim; i++)
		res *= *(ptr+2+i);
	return res;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////                        INT                                   /////////////////////////////////////
int mp_sum_i( int * a, MPI_Comm comm)
{
	assert( !reduce_base_integer( a, 1, comm, -1));
	return 0;
}

int mp_sum_iv1( int * a, MPI_Comm comm, int dim)
{
	int * ptr = a;
	if( dim == 0)
		assert( !reduce_base_integer( ptr , BASE_NUM( ptr), comm, -1));
	else
		assert( !reduce_base_integer( ptr , 1, comm, -1));
	return 0;
}

int mp_sum_iv2( int ** a, MPI_Comm comm)
{
	int * ptr = a[0];
	assert( !reduce_base_integer( ptr, BASE_NUM(ptr), comm, -1));
	return 0;
}

int mp_sum_iv3( int *** a, MPI_Comm comm)
{
	int * ptr = a[0][0];
	assert( !reduce_base_integer( ptr, BASE_NUM(ptr), comm, -1));
	return 0;
}

int mp_sum_iv4( int **** a, MPI_Comm comm)
{
	int * ptr = a[0][0][0];
	assert( !reduce_base_integer( ptr, BASE_NUM(ptr), comm, -1));
	return 0;
}



int mp_bcast_i( int * a, MPI_Comm comm)
{
	assert( !bcast_base_integer( a, 1, comm, ionode));
	return 0;
}

int mp_bcast_iv1( int * a, MPI_Comm comm, int dim)
{
	int * ptr = a;
	if( dim == 0)
		assert( !bcast_base_integer( ptr , BASE_NUM( ptr), comm, ionode));
	else
		assert( !bcast_base_integer( ptr , 1, comm, ionode));
	return 0;
}

int mp_bcast_iv2( int ** a, MPI_Comm comm)
{
	int * ptr = a[0];
	assert( !bcast_base_integer( ptr, BASE_NUM(ptr), comm, ionode));
	return 0;
}

int mp_bcast_iv3( int *** a, MPI_Comm comm)
{
	int * ptr = a[0][0];
	assert( !bcast_base_integer( ptr, BASE_NUM(ptr), comm, ionode));
	return 0;
}

int mp_bcast_iv4( int **** a, MPI_Comm comm)
{
	int * ptr = a[0][0][0];
	assert( !bcast_base_integer( ptr, BASE_NUM(ptr), comm, ionode));
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////                       LONG INT                               /////////////////////////////////////
int mp_sum_li( long int * a, MPI_Comm comm)
{
	assert( !reduce_base_long( a, 1, comm, -1));
	return 0;
}

int mp_sum_liv1( long int * a, MPI_Comm comm, int dim)
{
	long int * ptr = a;
	if( dim == 0)
		assert( !reduce_base_long( ptr , BASE_NUM( ptr), comm, -1));
	else
		assert( !reduce_base_long( ptr , 1, comm, -1));
	return 0;
}

int mp_sum_liv2( long int ** a, MPI_Comm comm)
{
	long int * ptr = a[0];
	assert( !reduce_base_long( ptr, BASE_NUM(ptr), comm, -1));
	return 0;
}

int mp_sum_liv3( long int *** a, MPI_Comm comm)
{
	long int * ptr = a[0][0];
	assert( !reduce_base_long( ptr, BASE_NUM(ptr), comm, -1));
	return 0;
}

int mp_sum_liv4( long int **** a, MPI_Comm comm)
{
	long int * ptr = a[0][0][0];
	assert( !reduce_base_long( ptr, BASE_NUM(ptr), comm, -1));
	return 0;
}


int mp_bcast_li( long int * a, MPI_Comm comm)
{
	assert( !bcast_base_long( a, 1, comm, ionode));
	return 0;
}

int mp_bcast_liv1( long int * a, MPI_Comm comm, int dim)
{
	long int * ptr = a;
	if( dim == 0)
		assert( !bcast_base_long( ptr , BASE_NUM( ptr), comm, ionode));
	else
		assert( !bcast_base_long( ptr , 1, comm, ionode));
	return 0;
}

int mp_bcast_liv2( long int ** a, MPI_Comm comm)
{
	long int * ptr = a[0];
	assert( !bcast_base_long( ptr, BASE_NUM(ptr), comm, ionode));
	return 0;
}

int mp_bcast_liv3( long int *** a, MPI_Comm comm)
{
	long int * ptr = a[0][0];
	assert( !bcast_base_long( ptr, BASE_NUM(ptr), comm, ionode));
	return 0;
}

int mp_bcast_liv4( long int **** a, MPI_Comm comm)
{
	long int * ptr = a[0][0][0];
	assert( !bcast_base_long( ptr, BASE_NUM(ptr), comm, ionode));
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////                       DOUBLE                                 /////////////////////////////////////
int mp_sum_d( double * a, MPI_Comm comm)
{
	assert( !reduce_base_double( a, 1, comm, -1));
	return 0;
}

int mp_sum_dv1( double * a, MPI_Comm comm, int dim)
{
	double * ptr = a;
	if( dim == 0)
		assert( !reduce_base_double( ptr, BASE_NUM(ptr), comm, -1));
	else
		assert( !reduce_base_double( ptr, 1, comm, -1));
	return 0;
}

int mp_sum_dv2( double ** a, MPI_Comm comm)
{
	double * ptr = a[0];
	assert( !reduce_base_double( ptr, BASE_NUM(ptr), comm, -1));
	return 0;
}

int mp_sum_dv3( double *** a, MPI_Comm comm)
{
	double * ptr = a[0][0];
	assert( !reduce_base_double( ptr, BASE_NUM(ptr), comm, -1));
	return 0;
}

int mp_sum_dv4( double **** a, MPI_Comm comm)
{
	double * ptr = a[0][0][0];
	assert( !reduce_base_double( ptr, BASE_NUM(ptr), comm, -1));
	return 0;
}

int mp_bcast_d( double * a, MPI_Comm comm)
{
	assert( !bcast_base_double( a, 1, comm, ionode));
	return 0;
}

int mp_bcast_dv1( double * a, MPI_Comm comm, int dim)
{
	double * ptr = a;
	if( dim == 0)
		assert( !bcast_base_double( ptr, BASE_NUM(ptr), comm, ionode));
	else
		assert( !bcast_base_double( ptr, 1, comm, ionode));
	return 0;
}

int mp_bcast_dv2( double ** a, MPI_Comm comm)
{
	double * ptr = a[0];
	assert( !bcast_base_double( ptr, BASE_NUM(ptr), comm, ionode));
	return 0;
}

int mp_bcast_dv3( double *** a, MPI_Comm comm)
{
	double * ptr = a[0][0];
	assert( !bcast_base_double( ptr, BASE_NUM(ptr), comm, ionode));
	return 0;
}

int mp_bcast_dv4( double **** a, MPI_Comm comm)
{
	double * ptr = a[0][0][0];
	assert( !bcast_base_double( ptr, BASE_NUM(ptr), comm, ionode));
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////                       COMPLEX                                 /////////////////////////////////////
int mp_sum_c( double complex * a, MPI_Comm comm)
{
	assert( !reduce_base_complex( a, 1, comm, -1));
	return 0;
}

int mp_sum_cv1( double complex* a, MPI_Comm comm, int dim)
{
	double complex * ptr = a;
	if( dim == 0)
		assert( !reduce_base_complex( ptr, BASE_NUM(ptr), comm, -1));
	else
		assert( !reduce_base_complex( ptr, 1, comm, -1));
	return 0;
}

int mp_sum_cv2( double complex ** a, MPI_Comm comm)
{
	double complex * ptr = a[0];
	assert( !reduce_base_complex( ptr, BASE_NUM(ptr), comm, -1));
	return 0;
}

int mp_sum_cv3( double complex *** a, MPI_Comm comm)
{
	double complex * ptr = a[0][0];
	assert( !reduce_base_complex( ptr, BASE_NUM(ptr), comm, -1));
	return 0;
}


int mp_sum_cv4( double complex **** a, MPI_Comm comm)
{
	double complex * ptr = a[0][0][0];
	assert( !reduce_base_complex( ptr, BASE_NUM(ptr), comm, -1));
	return 0;
}


int mp_bcast_c( double complex * a, MPI_Comm comm)
{
	assert( !bcast_base_complex( a, 1, comm, ionode));
	return 0;
}

int mp_bcast_cv1( double complex* a, MPI_Comm comm, int dim)
{
	double complex * ptr = a;
	if( dim == 0)
		assert( !bcast_base_complex( ptr, BASE_NUM(ptr), comm, ionode));
	else
		assert( !bcast_base_complex( ptr, 1, comm, ionode));
	return 0;
}

int mp_bcast_cv2( double complex ** a, MPI_Comm comm)
{
	double complex * ptr = a[0];
	assert( !bcast_base_complex( ptr, BASE_NUM(ptr), comm, ionode));
	return 0;
}

int mp_bcast_cv3( double complex *** a, MPI_Comm comm)
{
	double complex * ptr = a[0][0];
	assert( !bcast_base_complex( ptr, BASE_NUM(ptr), comm, ionode));
	return 0;
}


int mp_bcast_cv4( double complex **** a, MPI_Comm comm)
{
	double complex * ptr = a[0][0][0];
	assert( !bcast_base_complex( ptr, BASE_NUM(ptr), comm, ionode));
	return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////                         CHAR                                 /////////////////////////////////////
int mp_bcast_char( char * a, MPI_Comm comm, int dim)
{
	int len;
	if( mpi->world_rank == ionode)
		len = strlen( a);
	mp_bcast_iv1( &len, comm, 1);
	if( dim == 0)
		assert( !bcast_base_char( a , len, comm, ionode));
	else
		assert( !bcast_base_char( a, 1, comm, ionode));

	return 0;
}




#endif //__MPI





















