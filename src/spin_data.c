#include <qepp/qepp_struct.h>

extern unsigned long int TOT_MEM;
extern unsigned int ionode;
extern int verbosity;

spin_data * initialize_spin_data( int n_bnd, long int n_kpt)
{
	spin_data * to_init = malloc( sizeof( spin_data));

	to_init->typeID = ID_SPIN_DATA;
	to_init->size = sizeof( spin_data);
	to_init->n_kpt = n_kpt;
	to_init->n_bnd = n_bnd;

	to_init->m =		(double ****)AllocateLinearMem4( sizeof(double), n_kpt, n_bnd, 3, 2);
	to_init->cd =		(double ***)AllocateLinearMem3( sizeof(double), n_kpt, n_bnd, 2);
	to_init->elicity =	(double ***)AllocateLinearMem3( sizeof(double), n_kpt, n_bnd, 2);
	to_init->chirality =	(double ***)AllocateLinearMem3( sizeof(double), n_kpt, n_bnd, 2);
	//to_init->chir2 =	(double ***)AllocateLinearMem3( sizeof(double), n_kpt, n_bnd, 2);
	to_init->m_tot =	(double ***)AllocateLinearMem3( sizeof(double), n_kpt, n_bnd, 2);

	to_init->print =	&print_spin_data;
	to_init->duplicate =	&duplicate_spin_data;
	to_init->free = 	&free_spin_data;

	return to_init;
}

void * print_spin_data(spin_data * to_print, FILE * write)
{
	if( to_print == NULL)
		return NULL;
	if( write == NULL)
		return NULL;

	int n_bnd = to_print->n_bnd;
	long int n_kpt = to_print->n_kpt;

	double * ptr = NULL;
	QEPP_OUT( write, "#%6s%5s%8s (%8s,%8s) (%8s,%8s) (%8s,%8s) (%8s,%8s) (%8s,%8s) (%8s,%8s)\n",
		"kpt","bnd","ctemp","Re_x","Im_x","Re_y","Im_y","Re_z","Im_z","m_tot","m_tot i","Re_elic","Im_elic","Re_chir","Im_chir");
	for( long int i=0; i<n_kpt; i++)
	{
		for( int j=0; j<n_bnd; j++)
		{
			QEPP_OUT( write, "%7li%5d%8.4lf",i+1,j+1,to_print->cd[i][j][0]);
			for( int k=0; k<3; k++)
			{
				ptr = to_print->m[i][j][k];
				QEPP_OUT( write, " (%8.5lf,%8.5lf)",ptr[0],ptr[1]);
			}
			//QEPP_OUT( write, "%12.8lf",to_print->m[i][j][3][0]);
			//ptr = to_print->elicity[i][j];
			//QEPP_OUT( write, " (%8.5lf,%8.5lf)", ptr[0],ptr[1]);
			ptr = to_print->m_tot[i][j];
			QEPP_OUT( write, " (%8.5lf,%8.5lf)", ptr[0], ptr[1]);
			ptr = to_print->elicity[i][j];
			QEPP_OUT( write, " (%8.5lf,%8.5lf)", ptr[0],ptr[1]);
			ptr = to_print->chirality[i][j];
			QEPP_OUT( write, " (%8.5lf,%8.5lf)", ptr[0],ptr[1]);
			//ptr = to_print->chir2[i][j];
			//QEPP_OUT( write, " (%8.5lf,%8.5lf)", ptr[0],ptr[1]);
			QEPP_OUT( write, "\n");
		}
	}

	return NULL;
}

spin_data * duplicate_spin_data( spin_data * to_dupl)
{
	if( to_dupl == NULL)
		return NULL;

	int n_bnd = to_dupl->n_bnd;
	long int n_kpt = to_dupl->n_kpt;
	spin_data * res = initialize_spin_data( n_bnd, n_kpt);

	res->m =		(double ****)DuplicateLinearMem4( (void ****)to_dupl->m);
	res->cd =		(double ***)DuplicateLinearMem3( (void ***)to_dupl->cd);
	res->elicity =		(double ***)DuplicateLinearMem3( (void ***)to_dupl->elicity);
	res->chirality =	(double ***)DuplicateLinearMem3( (void ***)to_dupl->chirality);
	//res->chir2 =		(double ***)DuplicateLinearMem3( (void ***)to_dupl->chir2);
	res->m_tot =		(double ***)DuplicateLinearMem3( (void ***)to_dupl->m_tot);

	return res;
}

void * free_spin_data( spin_data * to_free)
{
	if( to_free == NULL)
		return NULL;

	FreeLinearMem4( (void ****)to_free->m);
	FreeLinearMem3( (void ***)to_free->cd);
	FreeLinearMem3( (void ***)to_free->elicity);
	FreeLinearMem3( (void ***)to_free->chirality);
	//FreeLinearMem3( (void ***)to_free->chir2);
	FreeLinearMem3( (void ***)to_free->m_tot);

	free( to_free);

	return NULL;
}




















