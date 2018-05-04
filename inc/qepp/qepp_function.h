#ifndef FUNCTION_H
#define FUNCTION_H

#include <qepp/qepp_err.h>
#include <qepp/qepp_io.h>
#include <qepp/qepp_struct.h>

#define ROUND(a) ceil(a-0.5)

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
errh * apply_broadening( opt_data **, opt_data *, double);
errh * calculate_opt_properties( opt_data ** out_ptr, opt_data * eps1, opt_data * eps2);

/*-------------------------------------------------------------------------*/
/**
  @name     calculate_eps_sl
  @brief    Superlattice optical properties.
  @param    eps[4] In order: eps1_re esp1_im eps2_re eps2_im where the
            number indicates the material
  @param    d1_L The ratio of d1/L which also define d2/L L=d1+d2

  This function calculate the complex dielectric funxtion of a superlattice 
  of 2 materials given their complex dielectric functions
 */
/*-------------------------------------------------------------------------*/
opt_data	* calculate_eps_sl( opt_data * eps[4], const double d1_L);


/*-------------------------------------------------------------------------*/
/**
  @name     calculate_eps_2d
  @brief    2d optical properties.
  @param    eps1: Real part of the dielectric function (if not given it is 
                  calculated through KK relations from eps2)
  @param    eps2: Imaginary part of the dielectric function
  @param    d: Effective thickness of the 2d layer
  @param    L: Size of the cell along the vacum direction

  Calculate the optical properties of a two-dimensional material ( optical
  conductivity, reflectivity, transmittance, absorbance)
 */
/*-------------------------------------------------------------------------*/
errh * calculate_eps_2d( opt_data **, opt_data * eps1, opt_data * eps2, const double d, const double L);

///////////////////////////////////////////////////////////////////////////////////////////////////////
// dielectric_function PostProcessing
errh * calculate_eps( opt_data **, opt_data *, double, long int, double, double);
//opt_data	* calculate_eps_m_el( m_elem *, double, long int, double, double);
errh * calculate_eps_huge_files( opt_data **, char *, double max_range, long int n_pt, double a3, double v_cell);
errh * calculate_jdos_huge_files( opt_data **, char *, double max_range, long int n_pt, double a3, double v_cell);
//opt_data 	* calculate_eps_mod(opt_data *, double, long int, double, double);
errh * calculate_eps_mod_huge_files( opt_data **, char *, double max_range, long int n_pt, double a3, double v_cell);

///////////////////////////////////////////////////////////////////////////////////////////////////////
// Kramers-Kronig PostProcessing
opt_data 	* apply_kk(opt_data *);
opt_data 	* apply_kk2(opt_data *);
opt_data 	* apply_kk_im(opt_data *);
opt_data	* apply_kk_real(opt_data *);
opt_data	* apply_kk_real2(opt_data *);
opt_data 	* apply_kk_im2(opt_data *);
//opt_data	* apply_kk_real2(opt_data *);
errh * apply_MSKK( opt_data ** out_ptr, opt_data * data, int mode, int n_ref, double * ref[2]);
errh * generate_chebychev_zeroes( opt_data *, int, double * [2]);
/*-------------------------------------------------------------------------*/
/**
  @name     apply_MSKK_real
  @brief    Apply the Multi Subtractive Kramers Kronig agorithm to the real part of a response function.
  @param    data:	The data to which to apply the MSKK
  @param    n_ref:	The number of reference points to be used
  @param    ref:	The data of the reference points where
				ref[0] -> vector containing the x coordinates
				ref[1] -> vector containing the y coordinates

  This function calculate the imaginary part of a response function by applying MSKK relations to the real part, using a set of
  reference data provided by the user. (The function generate_chebychev_zeroes can be used to generete such set starting from a
  set of reference data)
 */
/*-------------------------------------------------------------------------*/
opt_data	* apply_MSKK_real(opt_data * data, int n_ref, double * ref[2]);
opt_data	* apply_MSKK_im(opt_data * data, int n_ref, double * ref[2]);
//opt_data	* apply_MSKK_real_light(opt_data *, int, double * [2]);


///////////////////////////////////////////////////////////////////////////////////////////////////////
// Wave-function PostProcessing
errh * qepp_compute_cd_datafile( cd **, data_file *, long int, long int, long int, long int, int, int);
errh * qepp_compute_dos_nscf( opt_data **, nscf_data *, double, double, double);
errh * qepp_compute_dos_datafile( opt_data **, data_file *, double, double, double);
errh * qepp_compute_matrixelements( opt_data ** out_ptr, data_file * data);
errh * qepp_compute_matrixelements_huge( void ** dump, data_file * data);
errh * qepp_compute_berry_phase( double complex ** out_ptr, data_file * data);
errh * qepp_compute_berry_phase2( double complex ** out_ptr, data_file * data, long int * klist, long int num);





#endif //FUNCTION_H






