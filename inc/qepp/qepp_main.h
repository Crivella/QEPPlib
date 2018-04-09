#ifndef POSTPROC_H
#define POSTPROC_H

#define _GNU_SOURCE
#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE
#endif
#ifndef _XOPEN_SOURCE_EXTENDED
#define _XOPEN_SOURCE_EXTENDED
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <complex.h>
#include <stdbool.h>
#include <qepp/qepp_err.h>	//errh, SET_ERRH(), MSG(), WARN()
#include <qepp/qepp_struct.h>	//structs, initialize_...(), print_...(), duplicate_...(), free_...()
#include <qepp/qepp_file.h>
#include <qepp/functions.h>
#include <qepp/my_lapack.h>
#include <qepp/other.h>
#include <qepp/qepp_constants.h>
//#include <qepp/qepp_read.h>
#include <qepp/qepp_mem.h>
#include <qepp/qepp_io.h>

#ifdef __GSL
#include <gsl/gsl_integration.h>
#endif //__GSL

#include <qepp/qepp_mpi.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//extern defined variables
unsigned long int TOT_MEM=0;
const char * tab="  ";
int verbosity=5;
unsigned int ionode=0;
FILE * in_f;
FILE * errf = NULL;
FILE * outf = NULL;

char * err_func[256];
char * err_msg[1024];

char outdir[1024];
char prefix[128];
char workdir[1024];
char datafile_path[1024];
data_file * df = NULL;

#ifdef __MPI
mpi_data * mpi = NULL;
//unsigned long int ng_start;
//unsigned long int ng_end;
#endif //__MPI
////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Macros
#define ROUND(a) ceil(a-0.5)
//#define PRINT_COMPLEX( a) printf( #a ": %lf + i * %lf\n", creal(a), cimag(a))
////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Struct macros
#ifdef __MPI
	#define PRINT_DATA(a,b) mpi==NULL ? \
		a!=NULL ? \
			a->print( a, b) : \
			0 \
		: \
		mpi->world_rank == ionode ? \
			a->print( a, b) : \
			0;
#else
	#define PRINT_DATA(a,b) a!=NULL ? a->print( a, b) : 0;
#endif
	#define DUPLICATE(a) a!=NULL ? a->duplicate( a) : 0;
	#define FREE(a) a!=NULL ? a->free( a) : 0;
/*#define READ( a, b, ...) \
	_Generic( (b), \
		nscf_data **:	parse_errh(read_nscf_data(  a, (nscf_data **)b)), \
		band_data **:	parse_errh(read_band_data(  a, (band_data **)b)), \
		spin_data **:	parse_errh(read_spin_data(  a, (spin_data **)b)), \
		band_pp **:	parse_errh(read_band_pp(    a, (band_pp **)b)), \
		nscf_md **:	parse_errh(read_nscf_md(    a, (nscf_md **)b)), \
		opt_data **:	parse_errh(read_opt_data(   a, (opt_data **)b, SECOND( 0, ##__VA_ARGS__, NULL))), \
		m_elem **:	parse_errh(read_m_elem(     a, (m_elem **)b, SECOND( 0, ##__VA_ARGS__, NULL))),\
		fit_params **:	parse_errh(read_fit_params( a, (fit_params **)b)), \
		data_set **:	parse_errh(read_data_set(   a, (data_set **)b)), \
		pdos_data **:	parse_errh(read_pdos_data(  a, (pdos_data **)b, SECOND( 0, ##__VA_ARGS__, NULL))), \
		pdos_state **:	parse_errh(read_pdos_state( a, (pdos_state **)b)), \
		data_file **:	parse_errh(read_data_file(  a, (data_file **)b)), \
		wfc **:		parse_errh(read_wfc(        a, (wfc **)b)), \
		gkv **:		parse_errh(read_gkv(        a, (gkv **)b)), \
		default: 	fprintf( stderr, "Calling macro READ type not implemented...\n") \
	)*/
#define READ( a, b, ...) \
	_Generic( (b), \
		nscf_data **:	parse_errh(read_nscf_data(  (char *)a, (nscf_data **)b)), \
		band_data **:	parse_errh(read_band_data(  (char *)a, (band_data **)b)), \
		spin_data **:	parse_errh(read_spin_data(  (char *)a, (spin_data **)b)), \
		band_pp **:	parse_errh(read_band_pp(    (char *)a, (band_pp **)b)), \
		nscf_md **:	parse_errh(read_nscf_md(    (char *)a, (nscf_md **)b)), \
		opt_data **:	parse_errh(read_opt_data(   (char *)a, (opt_data **)b, SECOND( 0, ##__VA_ARGS__, NULL))), \
		m_elem **:	parse_errh(read_m_elem(     (char *)a, (m_elem **)b, SECOND( 0, ##__VA_ARGS__, NULL))),\
		fit_params **:	parse_errh(read_fit_params( (char *)a, (fit_params **)b)), \
		data_set **:	parse_errh(read_data_set(   (char *)a, (data_set **)b)), \
		pdos_data **:	parse_errh(read_pdos_data(  (char *)a, (pdos_data **)b, SECOND( 0, ##__VA_ARGS__, NULL))), \
		pdos_state **:	parse_errh(read_pdos_state( (char *)a, (pdos_state **)b)), \
		data_file **:	parse_errh(read_data_file(  (char *)a, (data_file **)b)), \
		wfc **:		parse_errh(read_wfc(        (char *)a, (wfc **)b)), \
		gkv **:		parse_errh(read_gkv(        (char *)a, (gkv **)b)), \
		egv **:		parse_errh(read_egv(        (char *)a, (egv **)b)), \
		char *:		_Generic( (a), \
			nscf_data **:	parse_errh(read_nscf_data(  (char *)b, (nscf_data **)a)), \
			band_data **:	parse_errh(read_band_data(  (char *)b, (band_data **)a)), \
			spin_data **:	parse_errh(read_spin_data(  (char *)b, (spin_data **)a)), \
			band_pp **:	parse_errh(read_band_pp(    (char *)b, (band_pp **)a)), \
			nscf_md **:	parse_errh(read_nscf_md(    (char *)b, (nscf_md **)a)), \
			opt_data **:	parse_errh(read_opt_data(   (char *)b, (opt_data **)b, SECOND( 0, ##__VA_ARGS__, NULL))), \
			m_elem **:	parse_errh(read_m_elem(     (char *)b, (m_elem **)b, SECOND( 0, ##__VA_ARGS__, NULL))),\
			fit_params **:	parse_errh(read_fit_params( (char *)b, (fit_params **)a)), \
			data_set **:	parse_errh(read_data_set(   (char *)b, (data_set **)a)), \
			pdos_data **:	parse_errh(read_pdos_data(  (char *)b, (pdos_data **)b, SECOND( 0, ##__VA_ARGS__, NULL))), \
			pdos_state **:	parse_errh(read_pdos_state( (char *)b, (pdos_state **)a)), \
			data_file **:	parse_errh(read_data_file(  (char *)b, (data_file **)a)), \
			wfc **:		parse_errh(read_wfc(        (char *)b, (wfc **)a)), \
			gkv **:		parse_errh(read_gkv(        (char *)b, (gkv **)a)), \
			egv **:		parse_errh(read_egv(        (char *)b, (egv **)a)), \
			default:	parse_errh( set_errh( WARNING, __func__, "Calling macro READ type not implemented...\n")) \
		), \
		default: 	parse_errh( set_errh( WARNING, __func__, "Calling macro READ type not implemented...\n")) \
	)
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//I/O macros
#define OPEN_IO_ENV( n, m, v) \
	open_io_env( n, m, v)
#define QEPP_PRINT( d, ...) \
	do { \
		if( verbosity >= 1) { \
			char buff[256]; \
			sprintf( buff, d, ##__VA_ARGS__); \
			qepp_print( ionode, buff, NULL); \
		} \
	} while(0)
#define QEPP_OUT( w, d, ...) \
	do { \
		if( verbosity >= 0) { \
			char buff[256]; \
			sprintf( buff, d, ##__VA_ARGS__); \
			qepp_print( ionode, buff, w); \
		} \
	} while(0)
#define QEPP_ALL_PRINT( d, ...) \
	do { \
		if( verbosity >= 1) { \
			char buff[256]; \
			sprintf( buff, d, ##__VA_ARGS__); \
			qepp_print( -1, buff, NULL); \
		} \
	} while(0)
#define CLOSE_IO_ENV() \
	close_io_env()
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//PP macros
#define QEPP_PRINT_BAND_STRUCTURE( in, outf) \
	_Generic((in), \
		nscf_data *:	parse_errh( print_band_structure_nscf( (nscf_data *)in, outf)), \
		band_data *:	parse_errh( print_band_structure_band( (band_data *)in, outf)), \
		default:	fprintf( stderr, "qepp_compute_cd: Type not defined.\n") \
	)
#define PRINT_KPT(a,...) \
	parse_errh( print_kpt(a,JUST2(__VA_ARGS__,0,0)))
#define APPLY_BROADENING( out, in1, p1) \
	parse_errh( apply_broadening( out, in1, p1))
#define CALC_EPS( out, in1, p1, ...) \
	parse_errh( calculate_eps( out, in1, p1, JUST3(__VA_ARGS__,0,0,0)))
#define CALC_EPS_HUGE( out, in1, p1, ...) \
	parse_errh( calculate_eps_huge_files( out, in1, p1, JUST3(__VA_ARGS__,0,0,0)))
#define APPLY_MSKK( out, in, mode, ref_s, nref) \
	do { \
		double *ref11[2]; \
		ref11[0] = malloc( nref * sizeof( double)); \
		ref11[1] = malloc( nref * sizeof( double)); \
		generate_chebychev_zeroes( ref_s, nref, ref11); \
		parse_errh( apply_MSKK( out, in, mode, nref, ref11)); \
		free( ref11[0]); \
		free( ref11[1]); \
	} while(0)
#define CALC_EPS_2D( out, in1, in2, p1, p2) \
	parse_errh( calculate_eps_2d( out, in1, in2, p1, p2))
#define QEPP_COMPUTE_CD( out, in1, b, c, d, e, f, g) \
	_Generic((in1), \
		data_file *:	parse_errh( qepp_compute_cd_datafile( out, (data_file *)in1, b, c, d, e, f, g)), \
		default:	fprintf( stderr, "qepp_compute_cd: Type not defined.\n") \
	)
#define QEPP_COMPUTE_DOS( out, in1, p1, p2, p3) \
	_Generic((in1), \
		nscf_data *:	parse_errh( qepp_compute_dos_nscf( out, (nscf_data *)in1, p1, p2, p3)), \
		data_file *:	parse_errh( qepp_compute_dos_datafile( out, (data_file *)in1, p1, p2, p3)), \
		default:	fprintf( stderr, "qepp_compute_dos: Type not defined.\n") \
	)
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef __MPI
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//MPI macros

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif //__MPI


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

errh * apply_broadening( opt_data **, opt_data *, double);
errh * calculate_eps( opt_data **, opt_data *, double, long int, double, double);
//opt_data	* calculate_eps_m_el( m_elem *, double, long int, double, double);
errh * calculate_eps_huge_files( opt_data **, char *, double max_range, long int n_pt, double a3, double v_cell);
errh * calculate_jdos_huge_files( opt_data **, char *, double max_range, long int n_pt, double a3, double v_cell);
//opt_data 	* calculate_eps_mod(opt_data *, double, long int, double, double);
errh * calculate_eps_mod_huge_files( opt_data **, char *, double max_range, long int n_pt, double a3, double v_cell);
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


errh * qepp_compute_cd_datafile( cd **, data_file *, long int, long int, long int, long int, int, int);


errh * qepp_compute_dos_nscf( opt_data **, nscf_data *, double, double, double);
errh * qepp_compute_dos_datafile( opt_data **, data_file *, double, double, double);
errh * qepp_compute_matrixelements( opt_data ** out_ptr, data_file * data);
errh * qepp_compute_matrixelements_huge( void ** dump, data_file * data);
errh * qepp_compute_berry_phase( double complex ** out_ptr, data_file * data);
errh * qepp_compute_berry_phase2( double complex ** out_ptr, data_file * data, long int * klist, long int num);






/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Ansii escape codes
#ifndef ANSII_ESCAPE_CODES
#define ANSII_ESCAPE_CODES
#define ANSII_UP		"\x1B[A"

#define ANSII_RESET		"\x1B[0m"

#define ANSII_BOLD		"\x1B[1m"
#define ANSII_UNDERSCORE	"\x1B[4m"
#define ANSII_BLINK		"\x1B[5m"
#define ANSII_REV_VID		"\x1B[7m"
#define ANSII_CONCEAL		"\x1B[8m"

#define ANSII_F_BLACK 		"\x1B[30m"
#define ANSII_F_RED		"\x1B[31m"
#define ANSII_F_GREEN		"\x1B[32m"
#define ANSII_F_YELLLOW		"\x1B[33m"
#define ANSII_F_BLUE		"\x1B[34m"
#define ANSII_F_MAGENTA		"\x1B[35m"
#define ANSII_F_CYAN		"\x1B[36m"
#define ANSII_F_WHITE		"\x1B[37m"

#define ANSII_F_BRIGHT_RED	"\x1B[38;2;255;0;0m"
#define ANSII_F_BRIGHT_GREEN	"\x1B[38;2;0;255;0m"
#define ANSII_F_BRIGHT_BLUE	"\x1B[38;2;0;0;255m"
#define ANSII_F_BRIGHT_CYAN	"\x1B[38;2;0;255;255m"

#define ANSII_B_BLACK 		"\x1B[40m"
#define ANSII_B_RED		"\x1B[41m"
#define ANSII_B_GREEN		"\x1B[42m"
#define ANSII_B_YELLLOW		"\x1B[43m"
#define ANSII_B_BLUE		"\x1B[44m"
#define ANSII_B_MAGENTA		"\x1B[45m"
#define ANSII_B_CYAN		"\x1B[46m"
#define ANSII_B_WHITE		"\x1B[47m"

#endif //ANSI_ESCAPE_CODES






#endif














