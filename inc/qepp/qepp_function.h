#ifndef FUNCTION_H
#define FUNCTION_H

#include <qepp/qepp_err.h>
#include <qepp/qepp_io.h>
#include <qepp/qepp_struct.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////
// nscf_data PostProcessing
errh * print_band_structure_nscf(nscf_data *, char *);
errh * print_band_structure_band(band_data *, char *);

errh * find_smallest_gap(  long int *, nscf_data *, double *, double);
errh * find_smallest_gap2( long int *, nscf_data *, double *, double);
errh * find_smallest_gap3( long int *, nscf_data *, double *, double);
errh * find_max_v_bnd(     long int *, nscf_data *, double *, double);
errh * find_min_c_bnd(     long int *, nscf_data *, double *, double);
errh * find_min_opt_gap(   long int *, nscf_data *, double *, double);

errh * find_e_fermi( double *, nscf_data *);
errh * tot_elect(    double *, nscf_data *);

///////////////////////////////////////////////////////////////////////////////////////////////////////
// opt_data PostProcessing





#endif //FUNCTION_H






