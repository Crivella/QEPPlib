#include <qepp/qepp_struct.h>

extern unsigned long int TOT_MEM;
extern unsigned int ionode;
extern int verbosity;

band_pp * initialize_band_pp( long int n_kpt, int n_bnd)
{
	band_pp * to_init;
	
	to_init = calloc( 1, sizeof( band_pp));
	to_init->typeID = ID_BAND_PP;
	to_init->size = sizeof( band_pp);

	to_init->n_kpt = n_kpt;
	to_init->n_bnd = n_bnd;

	to_init->n_bnd_occ =	(int *)AllocateLinearMem1( sizeof( int), n_kpt);
	to_init->kpt =		(double **)AllocateLinearMem2( sizeof( double), n_kpt, 3);
	to_init->pp =		(double ****)AllocateLinearMem4( sizeof( double), n_kpt, n_bnd, n_bnd, 3);


	to_init->print =	&print_band_pp;
	to_init->duplicate =	&duplicate_band_pp;
	to_init->free = 	&free_band_pp;

	return to_init;
}

void * print_band_pp( band_pp * to_print, FILE * write)
{
	if( to_print == NULL)
		return NULL;
	if( write == NULL)
		return NULL;

	long int n_kpt = to_print->n_kpt;
	int n_bnd = to_print->n_bnd;
	QEPP_OUT( write, "nbnd=%4d, nks=%4li\n",n_bnd,n_kpt);
	for( long int i=0; i<n_kpt; i++)
	{
		for( int j=0; j<3; j++)
			QEPP_OUT( write, "%10.6lf", to_print->kpt[i][j]);
		QEPP_OUT( write, "%8d\n", to_print->n_bnd_occ[i]);
		int n_bnd_occ = to_print->n_bnd_occ[i];
		int lim = n_bnd - n_bnd_occ;
		for( int j=0; j<3; j++)
		{
			QEPP_OUT( write, "  %d\n", j+1);
			for( int k=0; k<lim; k++)
			{
				for( int n=0; n<n_bnd_occ; n++)
				{
					QEPP_OUT( write, "%15.8lf", to_print->pp[i][n][k][j]);
					if( (n+1)%5==0)
						QEPP_OUT( write, "\n");
				}
				QEPP_OUT( write, "\n");
			}
		}
	}


	return NULL;
}

band_pp * duplicate_band_pp( band_pp * to_dupl)
{
	if( to_dupl == NULL)
		return NULL;

	long int n_kpt = to_dupl->n_kpt;
	int n_bnd = to_dupl->n_bnd;

	band_pp * new_s = initialize_band_pp( n_kpt, n_bnd);

	new_s->n_bnd_occ =	(int *)DuplicateLinearMem1( (void *)to_dupl->n_bnd_occ);
	new_s->kpt =		(double **)DuplicateLinearMem2( (void **)to_dupl->kpt);
	new_s->pp =		(double ****)DuplicateLinearMem4( (void ****)to_dupl->pp);

	return new_s;
}

void * free_band_pp( band_pp * to_free)
{
	if(to_free == NULL)
		return NULL;

	FreeLinearMem1( (void *)to_free->n_bnd_occ);
	FreeLinearMem2( (void **)to_free->kpt);
	FreeLinearMem4( (void ****)to_free->pp);

	free( to_free);


	return NULL;
}















