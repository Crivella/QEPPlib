#ifndef MYLAPACK_H
#define MYLAPACK_H

#include <qepp/qepp_mem.h>
#include <tgmath.h>
#include <stdlib.h>

//Linear algebra
double delta_k(double *, double *);
int vect_comp(double *, double *);
void vector_vector( double *, double *, double *);
void scalar_vector( double *, double *, double *);
void matrix_vector( double **, double *, double *);
void matrix_matrix( double **, double **, double **);
int matrix_comp(double ** a,double ** b);
int symm_order(double **);








#endif














