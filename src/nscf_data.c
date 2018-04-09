#include <qepp/qepp_struct.h>

extern unsigned long int TOT_MEM;
extern unsigned int ionode;
extern int verbosity;

nscf_data * initialize_nscf_data( long int n_kpt, int n_bnd)
{
	nscf_data * to_init;
	
	to_init = calloc( 1, sizeof( nscf_data));
	to_init->typeID = ID_NSCF_DATA;
	to_init->size = sizeof( nscf_data);

	to_init->n_kpt = n_kpt;
	to_init->n_bnd = n_bnd;
	to_init->tot_weight = 0;

	to_init->kpt =		(double **)AllocateLinearMem2( sizeof( double), n_kpt, 3);
	to_init->energies =	(double **)AllocateLinearMem2( sizeof( double), n_kpt, n_bnd);
	to_init->occup =	(double **)AllocateLinearMem2( sizeof( double), n_kpt, n_bnd);
	to_init->weight =	(double *)AllocateLinearMem1( sizeof( double), n_kpt);

	to_init->print =	&print_nscf_data;
	to_init->duplicate =	&duplicate_nscf_data;
	to_init->free = 	&free_nscf_data;

	return to_init;
}

void * print_nscf_data(nscf_data * to_print, FILE * write)
{
	if( to_print == NULL)
		return NULL;
	if( write == NULL)
		return NULL;

	for( long int i=0; i < to_print->n_kpt; i++)
	{
		QEPP_OUT(write,"\n");
		for( int i2=0; i2<3; i2++)
			QEPP_OUT(write,"%.4f ",to_print->kpt[i][i2]);
		QEPP_OUT(write,"\n\n");
		for( int i2=0; i2 < to_print->n_bnd; i2++)
		{
			QEPP_OUT(write,"%.4f",to_print->energies[i][i2]);
			if((i2+1)%8==0 || i2 == to_print->n_bnd - 1)
				QEPP_OUT(write,"\n");
			else
				QEPP_OUT(write," ");
		}
		QEPP_OUT(write,"\n\n");
		for( int i2=0; i2 < to_print->n_bnd; i2++)
		{
			QEPP_OUT(write,"%.4f",to_print->occup[i][i2]);
			if((i2+1)%8==0 || i2 == to_print->n_bnd - 1)
				QEPP_OUT(write,"\n");
			else
				QEPP_OUT(write," ");
		}
	}
	for( long int i=0; i < to_print->n_kpt; i++)
		QEPP_OUT(write,"%.7f\n",to_print->weight[i]);

	return NULL;
}

nscf_data * duplicate_nscf_data(nscf_data *to_dupl)
{
	if( to_dupl == NULL)
		return NULL;
	long int n_kpt=to_dupl->n_kpt;
	int n_bnd = to_dupl->n_bnd;

	nscf_data * new_s=initialize_nscf_data( n_kpt, n_bnd);

	new_s->md=DUPLICATE(to_dupl->md);

	new_s->kpt =		(double **)DuplicateLinearMem2( (void **)to_dupl->kpt);
	new_s->energies =	(double **)DuplicateLinearMem2( (void **)to_dupl->energies);
	new_s->occup =		(double **)DuplicateLinearMem2( (void **)to_dupl->occup);
	new_s->weight =		(double *)DuplicateLinearMem1( (void *)to_dupl->weight);

	return new_s;	
}

void * free_nscf_data(nscf_data * to_free)
{
	if(to_free == NULL)
		return NULL;

	FreeLinearMem2( (void **)to_free->kpt);
	FreeLinearMem2( (void **)to_free->energies);
	FreeLinearMem2( (void **)to_free->occup);
	FreeLinearMem1( (void *)to_free->weight);

	FREE(to_free->md);
	free(to_free);
	

	return NULL;
}

size_t totmem_nscf_data( nscf_data * a)
{
	size_t mem = 0;
	mem+=sizeof( a) + sizeof( *a);

	return mem;
}




















