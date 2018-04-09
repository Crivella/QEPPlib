#ifndef OTHER_H
#define OTHER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <qepp/qepp_err.h>
#include <qepp/qepp_mem.h>
#include <qepp/my_lapack.h>
#include <qepp/qepp_constants.h>


int check_interval( double , double , double );
double my_pow10( int);
double ** generate_spherical_grid( double *, double, double *);
double ** generate_spherical_surf( double *, double, double, double);




#endif












