#include <qepp/mpi_base.h>
#include <stdlib.h>

#ifdef __MPI


////////////////////////////      REDUCE      /////////////////////////////////////////////////////////////////
void complexSum(double complex *invec, double complex *inoutvec, int *len, MPI_Datatype *dtype)
{
	/*printf("proc_%d: %p -> %p \n",mpi->world_rank,invec,inoutvec);
	if( mpi->world_rank == 0)
	{
		printf("len: %d\n",*len);
	}*/
	for ( int i=0; i<*len; i++)
	{
		//if( mpi->world_rank == 0)
		//printf("%p -> %lf + I * %lf\n",&invec[i],creal(invec[i]),cimag(invec[i]));
		inoutvec[i] += invec[i];
	}
	/*for( int i=*len; i<2* *len; i++)
		if( mpi->world_rank == 0)
		printf("%p -> %lf + I * %lf\n",&invec[i],creal(invec[i]),cimag(invec[i]));*/
}

int reduce_base_integer( int *a, int dim, MPI_Comm comm, int root)
{
	assert( dim >= 1);
	MPI_Barrier( comm);

	int * b= malloc( dim * sizeof( int));
	if( root >= 0)
		assert( ! MPI_Reduce( a, b, dim, MPI_INT, MPI_SUM, root, comm));
	else
		assert( ! MPI_Allreduce( a, b, dim, MPI_INT, MPI_SUM, comm));
	memcpy( a, b, dim * sizeof( int));
	free( b);
	return 0;
}

int reduce_base_long( long int *a, int dim, MPI_Comm comm, int root)
{
	assert( dim >= 1);
	MPI_Barrier( comm);

	long int * b= malloc( dim * sizeof( long int));
	if( root >= 0)
		assert( ! MPI_Reduce( a, b, dim, MPI_LONG, MPI_SUM, root, comm));
	else
		assert( ! MPI_Allreduce( a, b, dim, MPI_LONG, MPI_SUM, comm));
	memcpy( a, b, dim * sizeof( long int));
	free( b);
	return 0;
}

int reduce_base_double( double *a, int dim, MPI_Comm comm, int root)
{
	assert( dim >= 1);
	MPI_Barrier( comm);

	double *b = malloc( dim * sizeof( double));
	if( root >= 0)
		assert( ! MPI_Reduce( a, b, dim, MPI_DOUBLE, MPI_SUM, root, comm));
	else
		assert( ! MPI_Allreduce( a, b, dim, MPI_DOUBLE, MPI_SUM, comm));
	memcpy( a, b, dim * sizeof( double));
	free( b);
	return 0;
}

int reduce_base_complex( double complex *a, int dim, MPI_Comm comm, int root)
{
	assert( dim >= 1);
	MPI_Barrier( comm);

	MPI_Datatype ctype;
	MPI_Type_contiguous( 2, MPI_DOUBLE, &ctype); 
	MPI_Type_commit(&ctype);

	MPI_Op ctypeSum;
	MPI_Op_create( (MPI_User_function *)complexSum, 1, &ctypeSum );

	//if( mpi->world_rank == 0)
	//	printf("dim: %d\n",dim);

	double complex * b = malloc( dim * sizeof(double complex));
	if( root >= 0)
		assert( ! MPI_Reduce( a, b, dim, ctype, ctypeSum, root, comm));
	else
		assert( ! MPI_Allreduce( a, b, dim, ctype, ctypeSum, comm));
	memcpy( a, b, dim * sizeof( double complex));
	free( b);

	MPI_Op_free( &ctypeSum);

	return 0;
}



////////////////////////////      BCAST       /////////////////////////////////////////////////////////////////
int bcast_base_char( char *a, int dim, MPI_Comm comm, int root)
{
	assert( dim >= 1);
	MPI_Barrier( comm);

	assert( ! MPI_Bcast( a, dim, MPI_UNSIGNED_CHAR, root, comm));

	return 0;
}

int bcast_base_integer( int *a, int dim, MPI_Comm comm, int root)
{
	assert( dim >= 1);
	MPI_Barrier( comm);

	assert( ! MPI_Bcast( a, dim, MPI_INT, root, comm));

	return 0;
}

int bcast_base_long( long int *a, int dim, MPI_Comm comm, int root)
{
	assert( dim >= 1);
	MPI_Barrier( comm);

	assert( ! MPI_Bcast( a, dim, MPI_LONG, root, comm));

	return 0;
}

int bcast_base_double( double *a, int dim, MPI_Comm comm, int root)
{
	assert( dim >= 1);
	MPI_Barrier( comm);

	assert( ! MPI_Bcast( a, dim, MPI_DOUBLE, root, comm));

	return 0;
}

int bcast_base_complex( double complex *a, int dim, MPI_Comm comm, int root)
{
	assert( dim >= 1);

	MPI_Datatype ctype;
	MPI_Type_contiguous( 2, MPI_DOUBLE, &ctype); 
	MPI_Type_commit(&ctype);

	assert( ! MPI_Bcast( a, dim, ctype, root, comm));

	return 0;
}

#endif //__MPI






