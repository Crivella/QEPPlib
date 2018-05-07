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
//#include <qepp/qepp_err.h>	//errh, SET_ERRH(), WARN(), FAIL()
#include <qepp/qepp_struct.h>	//structs, initialize_...(), print_...(), duplicate_...(), free_...()
#include <qepp/qepp_file.h>	//
#include <qepp/functions.h>
#include <qepp/my_lapack.h>
#include <qepp/qepp_constants.h>
#include <qepp/qepp_function.h>
//#include <qepp/qepp_input.h>
#include <qepp/qepp_mem.h>	//QEPP_ALLOC/2/3/4(), QEPP_DUPL/2/3/4(), QEPP_FREE/2/3/4()
#include <qepp/qepp_output.h>	//OPEN_IO_ENV(), CLOSE_IO_ENV(), QEPP_PRINT(), QEPP_ALL_PRINT(), QEPP_OUT()
#include <qepp/qepp_mpi.h>	//mb_bcast(), mp_sum(), SET_PW_MPI()

#ifdef __GSL
#include <gsl/gsl_integration.h>
#endif //__GSL



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

mpi_data * mpi = NULL;
//unsigned long int ng_start;
//unsigned long int ng_end;
////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Macros
//#define ROUND(a) ceil(a-0.5)
//#define PRINT_COMPLEX( a) printf( #a ": %lf + i * %lf\n", creal(a), cimag(a))
////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Struct macros
#define PRINT_DATA(a,b) mpi->world_rank == ionode ? \
		a->print( a, b) : \
		0
#define DUPLICATE(a) a!=NULL ? a->duplicate( a) : 0;
#define FREE(a) a!=NULL ? a->free( a) : 0;

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














