#ifndef STRUCT_H
#define STRUCT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <complex.h>

#include <qepp/qepp_constants.h>
#include <qepp/qepp_io.h>
#include <qepp/qepp_err.h>	//errh, SET_ERRH(), MSG(), WARN()
#include <qepp/qepp_mem.h>	//AllocateLinearMem_(), DuplicateLinearMem_(), FreeLinearMem_()

//Struct IDs
#define ID_NSCF_DATA	1
#define ID_BAND_DATA	2
#define ID_BAND_PP	3
#define ID_NSCF_MD	4
#define ID_SPIN_DATA	5
#define ID_OPT_DATA	8
#define ID_COL_DATA	9
#define ID_M_ELEM	10
#define ID_FIT_PARAMS	16
#define ID_DATA_SET	32
#define ID_WFC		64
#define	ID_EVC		65
#define ID_GKV		66
#define ID_EGV		67
#define ID_DATA_FILE	68
#define ID_PDOS_DATA	128
#define ID_PDOS_STATE	129
#define ID_CD		256

#define MAX_H_SIZE 64

#define PASTE(a,b) a ##b
#ifndef JUST_MACRO
#define JUST_MACRO
#define JUST1(a, ...) (a)
#define JUST2(a, b, ...) (a), (b)
#define JUST3(a, b, c, ...) (a), (b), (c)
#endif

//((nscf_data *)a)->var(a,b);
typedef void * (*func_ptr)();

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//List of my structures
typedef struct nscf_data nscf_data;
typedef struct band_data band_data;
typedef struct band_pp band_pp;
typedef struct spin_data spin_data;
typedef struct nscf_metadata nscf_md;
typedef struct opt_data opt_data;
typedef struct col_data col_data;
typedef struct m_elem m_elem;
typedef struct fit_params fit_params;
typedef struct data_set data_set;
typedef struct pdos_data pdos_data;
	typedef struct pdos_state pdos_state;

typedef struct data_file data_file;
typedef struct wfc wfc;
typedef struct evc evc;
typedef struct gkv gkv;
typedef struct egv egv;

typedef struct cd cd;

#include <qepp/qepp_read.h>	//READ()


nscf_md		* initialize_nscf_md();
band_data	* initialize_band_data( int, long int);
band_pp		* initialize_band_pp( long int, int);
spin_data	* initialize_spin_data( int, long int);
nscf_data	* initialize_nscf_data(long int, int);
opt_data	* initialize_opt_data( long int, int);
col_data	* initialize_col_data( long int, int);
m_elem		* initialize_m_elem( long int, int);	
fit_params 	* initialize_fit_params( int nparams);
data_set 	* initialize_data_set( int npt);
pdos_data	* initialize_pdos_data(long int, int, int);
pdos_state	* initialize_pdos_state();
////////////////////////////////////////////////
data_file	* initialize_data_file();
wfc	 	* initialize_wfc( int n_evc);
evc	 	* initialize_evc( long int nc);
gkv	 	* initialize_gkv( long int ngkv);
egv		* initialize_egv( int n_bnd);
cd	 	* initialize_cd( long int, long int, long int);

void * 		print_nscf_data(nscf_data *, FILE *);
void *		print_band_data(band_data *, FILE *);
void *		print_band_pp( band_pp *, FILE *);
void *		print_spin_data(spin_data *, FILE *);
void * 		print_nscf_md(nscf_md *, FILE *);
void * 		print_opt_data(opt_data *, FILE *);
void * 		print_col_data(col_data *, FILE *);
void * 		print_m_elem(m_elem *, FILE *);
void * 		print_fit_params( fit_params *, FILE *);
void * 		print_data_set( data_set *, FILE *);
void * 		print_pdos_data( pdos_data *, FILE *);
void * 		print_pdos_state( pdos_state *, FILE *);
////////////////////////////////////////////////
void *		print_data_file( data_file *, FILE *);
void * 		print_wfc( wfc *, FILE *);
void * 		print_evc( evc *, FILE *);
void * 		print_gkv( gkv *, FILE *);
void *		print_egv( egv *, FILE *);
void * 		print_cd( cd *, FILE *);

nscf_md		* duplicate_nscf_md( nscf_md *);
band_data	* duplicate_band_data( band_data *);
band_pp		* duplicate_band_pp( band_pp *);
spin_data	* duplicate_spin_data( spin_data *);
nscf_data	* duplicate_nscf_data(nscf_data *);
opt_data	* duplicate_opt_data(opt_data *);
col_data	* duplicate_col_data(col_data *);
m_elem		* duplicate_m_elem(m_elem *);
fit_params 	* duplicate_fit_params( fit_params * to_dupl);
data_set	* duplicate_data_set( data_set * to_dupl);
pdos_data	* duplicate_pdos_data( pdos_data * to_dupl);
pdos_state	* duplicate_pdos_state( pdos_state * to_dupl);
////////////////////////////////////////////////
data_file	* duplicate_data_file( data_file * to_dupl);
wfc		* duplicate_wfc( wfc * to_dupl);
evc		* duplicate_evc( evc * to_dupl);
gkv		* duplicate_gkv( gkv * to_dupl);
egv		* duplicate_egv( egv * to_dupl);
cd		* duplicate_cd( cd * to_dupl);

void * 		free_nscf_md(nscf_md *);
void *		free_band_data( band_data *);
void *		free_band_pp( band_pp *);
void *		free_spin_data( spin_data *);
void * 		free_nscf_data(nscf_data *);
void * 		free_opt_data(opt_data *);
void * 		free_col_data(col_data *);
void * 		free_m_elem(m_elem *);
void * 		free_fit_params( fit_params * to_free);
void *		free_data_set( data_set * to_free);
void *		free_pdos_data( pdos_data * to_free);
void *		free_pdos_state( pdos_state * to_free);
////////////////////////////////////////////////
void *		free_data_file( data_file * to_free);
void *		free_wfc( wfc * to_free);
void *		free_evc( evc * to_free);
void *		free_gkv( gkv * to_free);
void *		free_egv( egv * to_free);
void *		free_cd( cd * to_free);

/*size_t		totmem_nscf_md(nscf_md *);
size_t		totmem_band_data( band_data *);
size_t		totmem_nscf_data(nscf_data *);
size_t		totmem_opt_data(opt_data *);
size_t		totmem_col_data(col_data *);
size_t		totmem_m_elem(m_elem *);
size_t		totmeme_fit_params( fit_params * to_free);
size_t		totmem_data_set( data_set * to_free);*/

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

void *		free_array_ptr(double **, long int);
#define EPS_HEADERS {"E[eV]","eps_x","eps_y","eps_z","TOT"}
#define M_EL_HEADERS {"n_kpt","cond_b","val_b","pp_x","pp_y","pp_z","E_gap","Occup"}
#define EELS_HEADERS {"E[eV]","Im(eps)","Re(eps)","EELS"}
#define SUM_PDOS_HEADERS {"E[eV]","s","p","d","f"}
void make_headers(opt_data *, char **, int);
#define MAKE_HEADERS(a,b) \
char *app_h[]=b; \
make_headers( a, app_h, sizeof(app_h)/sizeof(char *))

errh * print_kpt( nscf_data *, long int, char *);
#define PRINT_KPT(a,...) \
	parse_errh( print_kpt(a,JUST2(__VA_ARGS__,0,0)))

errh * nscf_data_getband( band_data **, nscf_data *);
errh * spin_data_getband( band_data **, spin_data *, int, nscf_data *);
errh * band_pp_getopt( opt_data **, band_pp *);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Structs
struct nscf_data
{
	int typeID;
	size_t size;
	//size_t mem;

	nscf_md * md;
	long int n_kpt;
	int n_bnd;

	double ** kpt;
	double ** energies;
	double ** occup;
	double * weight;
	double tot_weight;

	void * 		(*print)( nscf_data *, FILE *);
	nscf_data *	(*duplicate)( nscf_data *);
	void * 		(*free)( nscf_data *);
	//size_t		(*totmem)( nscf_data *);
};

struct band_data
{
	int typeID;
	size_t size;
	//size_t mem;

	long int n_kpt;
	int n_bnd;

	double ** kpt;
	double ** bands;

	void * 		(*print)( band_data *, FILE *);
	band_data *	(*duplicate)( band_data *);
	void * 		(*free)( band_data *);
	//size_t		(*totmem)( band_data *);
};

struct band_pp
{
	int typeID;
	size_t size;
	//size_t mem;

	long int n_kpt;
	int n_bnd;

	int * n_bnd_occ;
	double ** kpt;
	double **** pp;

	void * 		(*print)( band_pp *, FILE *);
	band_pp *	(*duplicate)( band_pp *);
	void * 		(*free)( band_pp *);
	//size_t		(*totmem)( band_data *);
};

struct spin_data
{
	int typeID;
	size_t size;
	//size_t mem;

	long int n_kpt;
	int n_bnd;

	double ***  cd;		//[kpt][bnd][Re/Im]
	double *** elicity;	//[kpt][bnd][Re/Im]
	double *** chirality;	//[kpt][bnd][Re/Im]
	double *** m_tot;	//[kpt][bnd][Re/Im]
	//double *** chir2;	//[kpt][bnd][Re/Im]
	double **** m;		//[kpt][bnd][x/y/z/tot][Re/Im]

	void * 		(*print)( spin_data *, FILE *);
	spin_data *	(*duplicate)( spin_data *);
	void * 		(*free)( spin_data *);
	//size_t		(*totmem)( band_data *);
};

struct nscf_metadata
{
	int typeID;
	size_t size;
	//size_t mem;

	long int lines;
	//long int *lines_pos;

	char formula[128];
	long int n_kpt;
	double e_fermi;
	//double e_tot;
	double vc;
	double n_el;
	int n_bnd;
	int spin_orbit;

	void * 		(*print)( nscf_md *, FILE *);
	nscf_md *	(*duplicate)( nscf_md *);
	void * 		(*free)( nscf_md *);
	//size_t		(*totmem)( nscf_md *);
};

struct opt_data
{
	int typeID;
	size_t size;
	//size_t mem;

	int n_col;
	long int n_pt;

	double * x;
	double ** values;
	char ** headers;

	void * 		(*print)( opt_data *, FILE *);
	opt_data *	(*duplicate)( opt_data *);
	void * 		(*free)( opt_data *);
	//size_t		(*totmem)( opt_data *);
};
///////////////////////////////////da implementare
struct col_data
{
	int typeID;
	size_t size;
	size_t mem;

	void * 		(*print)( col_data *, FILE *);
	col_data *	(*duplicate)( col_data *);
	void * 		(*free)( col_data *);
	size_t		(*totmem)( col_data *);
};/////////////////////////////////////////////////////////////////

struct m_elem
{
	int typeID;
	size_t size;
	size_t mem;

	long int n_kpt;
	int n_bnd;
	//int v_bnd;

	double **** pp;
	double *** gap;
	double *** df;

	void * 		(*print)( m_elem *, FILE *);
	m_elem *	(*duplicate)( m_elem *);
	void * 		(*free)( m_elem *);
	size_t		(*totmem)( m_elem *);
};

struct fit_params
{
	int typeID;
	size_t size;
	size_t mem;

	int nparams;
	double * params;

	double * min;
	double * max;
	double * delta;

	double offset;

	void * 		(*print)( fit_params *, FILE *);
	fit_params *	(*duplicate)( fit_params *);
	void * 		(*free)( fit_params *);
	size_t		(*totmem)( fit_params *);
};

struct data_set
{
	int typeID;
	size_t size;
	size_t mem;

	int n_pt;
	double * x;
	double * y;

	void * 		(*print)( data_set *, FILE *);
	data_set *	(*duplicate)( data_set *);
	void * 		(*free)( data_set *);
	size_t		(*totmem)( data_set *);
};

struct pdos_data
{
	int typeID;
	size_t size;
	size_t mem;

	long int n_kpt;
	int n_bnd;
	int n_states;

	double *** pdos; //[kpt][band][state]

	nscf_data * nscf;
	pdos_state ** states;

	void * 		(*print)( pdos_data *, FILE *);
	pdos_data *	(*duplicate)( pdos_data *);
	void * 		(*free)( pdos_data *);
	size_t		(*totmem)( pdos_data *);
};

struct pdos_state
{
	int typeID;
	size_t size;
	size_t mem;

	char name[64];
	int atom_n;
	int wfc_n;

	double j, m_j;
	int l;

	void * 		(*print)( pdos_state *, FILE *);
	pdos_state *		(*duplicate)( pdos_state *);
	void * 		(*free)( pdos_state *);
	size_t		(*totmem)( pdos_state *);
};

struct data_file
{
	int typeID;
	size_t size;
	size_t mem;

	long int version;	//QE version x.nn.aa -> xnnaa

	double n_el;		//Number of electron in the system
	int n_spin;		//Identifier for spin usage in calculation
	int n_bnd;		//Number of bands

	char bravais[64];	//String containing the type of bravais lattice
	double lp_t;		//Units of the lattice parameter in the SI
	double lp;		//Lattice parameters in units of lp_t
	double c_dim[6];	//Cell dimensions

	char   a_n[64];		//Name of the units for the direct lattice
	double a_t;		//Units in SI for the direct lattice
	double a[3][3];		//Direct lattice vectors in units of a_t

	char   b_n[64];		//Name of the units for the reciprocal lattice
	double b_t;		//Units in SI for the reciprocal lattice
	double b[3][3];		//Reciprocal lattice vectors in units of b_t

	long int n_kpt;		//Number of k_points
	double kpt_t;		//Units of k_points in SI
	double ** kpt;		//Coordinates of all k_points
	double * weight;	//Weights of all k_points

	char ** egval_link;	//Link to file containing the eigenvalues data for each k_point
	char ** egvec_link;	//Link to file containing the eigenvector data for each k_point
	char **	wfc_link;	//Link to file containing the coefficient of the for each k_point
	char ** wfc_link2;	//(in case of spin-orbit)Link to file containing the coefficient of the for each k_point with other spin

	void * 		(*print)( data_file *, FILE *);
	data_file *	(*duplicate)( data_file *);
	void * 		(*free)( data_file *);
	size_t		(*totmem)( data_file *);
};

struct wfc
{
	int typeID;
	size_t size;
	size_t mem;

	int n_evc;		//Number of states per kpt
	//long int * nc;
	evc ** evc_vect;	//Vector of structs containing the dataof each state
	//double complex ** val;

	long int ngw;		//Max index for g-vect
	long int igwx;		//Number of g-vect
	bool gamma_only;
	int nbnd;		//Number of bands
	long int ik;		//Number of current kpt
	long int nk;		//Total number of kpts
	int ispin;
	int nspin;
	double scale_factor;

	void * 		(*print)( wfc *, FILE *);
	wfc *		(*duplicate)( wfc *);
	void * 		(*free)( wfc *);
	size_t		(*totmem)( wfc *);
};

struct evc
{
	int typeID;
	size_t size;
	size_t mem;

	long int nc;		//Number of g-vect (size of vector val)
	double complex * val;	//Coefficients of the expansion on plane waves

	void * 		(*print)( evc *, FILE *);
	evc *		(*duplicate)( evc *);
	void * 		(*free)( evc *);
	size_t		(*totmem)( evc *);
};

struct gkv
{
	int typeID;
	size_t size;
	size_t mem;

	long int ngkv;		//Number of g-vect used for current kpt
	long int max_ngkv;	//Max numver of g-vect used for all kpt
	bool gamma_only;
	double kpt[3];		//Coord of kpt
	int * index;		//Indexing of g-vects
	int ** grid;		//Grid of miller indexes of g-vects

	void * 		(*print)( gkv *, FILE *);
	gkv *		(*duplicate)( gkv *);
	void * 		(*free)( gkv *);
	size_t		(*totmem)( gkv *);
};

struct egv
{
	int typeID;
	size_t size;
	size_t mem;

	int n_bnd;	//Number of bands
	double e_t;	//Coversion val units to eV
	
	double * val;	//Vector energy[ band]
	double * occ;	//Vector occupation[ band]

	void * 		(*print)( egv *, FILE *);
	egv *		(*duplicate)( egv *);
	void * 		(*free)( egv *);
	size_t		(*totmem)( egv *);
};

struct cd
{
	int typeID;
	size_t size;
	size_t mem;

	long int x,y,z;

	double ****coord;
	double ***dens;

	void * 		(*print)( cd *, FILE *);
	cd *		(*duplicate)( cd *);
	void * 		(*free)( cd *);
	size_t		(*totmem)( cd *);
};




#endif /*STRUCT_H*/









