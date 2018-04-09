#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#ifdef __GSL

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <qepp/my_lapack.h>
#include <qepp/qepp_constants.h>
#include <qepp/qepp_struct.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_spline.h>
#include <gsl/gsl_rng.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
data_set * derivative( data_set * data, int order);
void derivative_calc( data_set * data, int order);
double average( double * , int);
double dev_std( double *, int);
long int factorial( int);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef FUNC_RES
#define FUNC_RES
typedef double (*func_res)(double *, fit_params *);
#endif

#define MAX_ITER	1.E+6
double chisq( func_res func, fit_params * par, data_set * data);
fit_params * fit( char * func_name, fit_params * par, data_set * data, double MIN_CHISQ);
int   initialize_params( char * func_name, fit_params * par, data_set * data);

double linear( double * val, fit_params * par);
double quadratic( double * val, fit_params * par);
double cubic( double * val, fit_params * par);
double polynomial( double * val, fit_params * par);
double birch_murnaghan( double * val, fit_params * par);



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Structs











#endif //__GSL
#endif //FUNCTIONS_H



























