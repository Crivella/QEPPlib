#ifndef MPI_BASE_H
#define MPI_BASE_H

#include <mpi.h>	//MPI_Reduce(), MPI_Allreduce(), MPI_Barrier(), MPI_Bcast()
#include <string.h>	//memcpy()
#include <assert.h>	//assert()
#include <complex.h>

void complexSum ( double complex *, double complex *, int *, MPI_Datatype * );

int reduce_base_integer( int *, int , MPI_Comm , int );
int reduce_base_long( long int *, int , MPI_Comm , int );
int reduce_base_double( double *, int , MPI_Comm , int );
int reduce_base_complex( double complex *, int, MPI_Comm, int);

int bcast_base_char( char *, int , MPI_Comm , int );
int bcast_base_integer( int *, int , MPI_Comm , int );
int bcast_base_long( long int *, int , MPI_Comm , int );
int bcast_base_double( double *, int , MPI_Comm , int );
int bcast_base_complex( double complex *, int, MPI_Comm, int);








#endif
























