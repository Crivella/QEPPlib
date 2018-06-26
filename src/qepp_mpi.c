#include <qepp/qepp_mpi.h>

extern unsigned int ionode;

mpi_data * mpi;

#ifdef __MPI

/*long int mpi_get_size( void * a)
{
	long int res=1;
	int dim = BASE_DIM(a);
	long int * ptr = *((long int **)BASE_ADDR(a));

	for( int i=0; i<dim; i++)
		res *= *(ptr+2+i);
	return res;
}*/

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////                        INT                                   /////////////////////////////////////
int mp_sum_iv( int * a, MPI_Comm comm, int dim)
{
	int * ptr = a;
	if( dim == 0)
	{
		ptr = qepp_mem_get_base( a);
		assert( !reduce_base_integer( ptr , BASE_NUM( ptr), comm, -1));
	}
	else
		assert( !reduce_base_integer( ptr , 1, comm, -1));
	return 0;
}

int mp_bcast_iv( int * a, MPI_Comm comm, int dim)
{
	int * ptr = a;
	if( dim == 0)
	{
		ptr = qepp_mem_get_base( a);
		assert( !bcast_base_integer( ptr , BASE_NUM( ptr), comm, ionode));
	}
	else
		assert( !bcast_base_integer( ptr , 1, comm, ionode));
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////                       LONG INT                               /////////////////////////////////////
int mp_sum_liv( long int * a, MPI_Comm comm, int dim)
{
	long int * ptr = a;
	if( dim == 0)
	{
		ptr = qepp_mem_get_base( a);
		assert( !reduce_base_long( ptr , BASE_NUM( ptr), comm, -1));
	}
	else
		assert( !reduce_base_long( ptr , 1, comm, -1));
	return 0;
}

int mp_bcast_liv( long int * a, MPI_Comm comm, int dim)
{
	long int * ptr = a;
	if( dim == 0)
	{
		ptr = qepp_mem_get_base( a);
		assert( !bcast_base_long( ptr , BASE_NUM( ptr), comm, ionode));
	}
	else
		assert( !bcast_base_long( ptr , 1, comm, ionode));
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////                       DOUBLE                                 /////////////////////////////////////
int mp_sum_dv( double * a, MPI_Comm comm, int dim)
{
	double * ptr = a;
	if( dim == 0)
	{
		ptr = qepp_mem_get_base( a);
		assert( !reduce_base_double( ptr, BASE_NUM(ptr), comm, -1));
	}
	else
		assert( !reduce_base_double( ptr, 1, comm, -1));
	return 0;
}

int mp_bcast_dv( double * a, MPI_Comm comm, int dim)
{
	double * ptr = a;
	if( dim == 0)
	{
		ptr = qepp_mem_get_base( a);
		assert( !bcast_base_double( ptr, BASE_NUM(ptr), comm, ionode));
	}
	else
		assert( !bcast_base_double( ptr, 1, comm, ionode));
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////                       COMPLEX                                 /////////////////////////////////////
int mp_sum_cv( double complex* a, MPI_Comm comm, int dim)
{
	double complex * ptr = a;
	if( dim == 0)
	{
		ptr = qepp_mem_get_base( a);
		assert( !reduce_base_complex( ptr, BASE_NUM(ptr), comm, -1));
	}
	else
		assert( !reduce_base_complex( ptr, 1, comm, -1));
	return 0;
}

int mp_bcast_cv( double complex* a, MPI_Comm comm, int dim)
{
	double complex * ptr = a;
	if( dim == 0)
	{
		ptr = qepp_mem_get_base( a);
		assert( !bcast_base_complex( ptr, BASE_NUM(ptr), comm, ionode));
	}
	else
		assert( !bcast_base_complex( ptr, 1, comm, ionode));
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////                         CHAR                                 /////////////////////////////////////
int mp_bcast_char( char * a, MPI_Comm comm, int dim)
{
	if( dim == 0)
	{
		int len;
		if( mpi->world_rank == ionode)
			len = strlen( a);
		mp_bcast( &len, comm, 1);
		assert( !bcast_base_char( a , len, comm, ionode));
	}
	else
		assert( !bcast_base_char( a, 1, comm, ionode));
	return 0;
}




#endif //__MPI





















