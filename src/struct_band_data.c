#include <qepp/qepp_struct.h>

extern unsigned long int TOT_MEM;
extern unsigned int ionode;
extern int verbosity;

band_data * initialize_band_data( int n_bnd, long int n_kpt)
{
	band_data * to_init = malloc( sizeof( band_data));

	to_init->typeID = ID_BAND_DATA;
	to_init->size = sizeof( band_data);
	to_init->n_kpt = n_kpt;
	to_init->n_bnd = n_bnd;

	to_init->kpt =		(double **)AllocateLinearMem2( sizeof( double), n_kpt, 3);
	to_init->bands =	(double **)AllocateLinearMem2( sizeof( double), n_kpt, n_bnd);

	to_init->print =	&print_band_data;
	to_init->duplicate =	&duplicate_band_data;
	to_init->free = 	&free_band_data;

	return to_init;
}

void * print_band_data(band_data * to_print, FILE * write)
{
	if( to_print == NULL)
		return NULL;
	if( write == NULL)
		return NULL;

	int n_bnd = to_print->n_bnd;
	long int n_kpt = to_print->n_kpt;

	for( long int i=0; i<n_kpt; i++)
	{
		for( int j=0; j<3; j++)
			QEPP_OUT( write, "%10.6lf", to_print->kpt[i][j]);
		for( int j=0; j<n_bnd; j++)
		{
			if( j%8==0)
				QEPP_OUT( write, "\n  ");
			QEPP_OUT( write, "%9.3f", to_print->bands[i][j]);
		}
		QEPP_OUT( write, "\n\n");
	}

	return NULL;
}

band_data * duplicate_band_data( band_data * to_dupl)
{
	if( to_dupl == NULL)
		return NULL;

	int n_bnd = to_dupl->n_bnd;
	long int n_kpt = to_dupl->n_kpt;
	band_data * new_s = initialize_band_data( n_bnd, n_kpt);

	new_s->kpt =	(double **)DuplicateLinearMem2( (void **)to_dupl->kpt);
	new_s->bands =	(double **)DuplicateLinearMem2( (void **)to_dupl->bands);

	return new_s;
}

void * free_band_data( band_data * to_free)
{
	if( to_free == NULL)
		return NULL;

	FreeLinearMem2( (void **)to_free->kpt);
	FreeLinearMem2( (void **)to_free->bands);
	free( to_free);

	return NULL;
}




















