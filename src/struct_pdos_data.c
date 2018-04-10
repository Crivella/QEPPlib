#include <qepp/qepp_struct.h>

extern unsigned long int TOT_MEM;
extern unsigned int ionode;
extern int verbosity;

pdos_data * initialize_pdos_data( long int n_kpt, int n_bnd, int n_states)
{
	pdos_data * to_init;
	
	to_init = calloc( 1, sizeof( pdos_data));
	to_init->typeID = ID_PDOS_DATA;
	to_init->size = sizeof( pdos_data);

	to_init->n_kpt = n_kpt;
	to_init->n_bnd = n_bnd;
	to_init->n_states = n_states;

	fprintf( stdout, "Allocating  %.2lf MB of RAM for pdos_data...\n", n_kpt*n_bnd*n_states*sizeof( double)/(1024.*1024.));
	to_init->pdos =		(double ***)AllocateLinearMem3( sizeof( double), n_kpt, n_bnd, n_states);

	to_init->states = malloc( n_states * sizeof( pdos_state *));
	for( int i=0; i<n_states; i++)
		to_init->states[i] = initialize_pdos_state();

	to_init->nscf = NULL;

	to_init->print =	&print_pdos_data;
	to_init->duplicate =	&duplicate_pdos_data;
	to_init->free = 	&free_pdos_data;

	return to_init;
}

void * print_pdos_data(pdos_data * to_print, FILE * write)
{
	if( to_print == NULL)
		return NULL;
	if( write == NULL)
		return NULL;

	//to fill------------------------------------------------------------

	return NULL;
}

pdos_data * duplicate_pdos_data(pdos_data *to_dupl)
{
	if( to_dupl == NULL)
		return NULL;
	long int n_kpt=to_dupl->n_kpt;
	int n_bnd = to_dupl->n_bnd;
	int n_states = to_dupl->n_states;

	pdos_data * new_s=initialize_pdos_data( n_kpt, n_bnd, n_states);

	//to fill---------------------------------------------------------------

	return new_s;	
}

void * free_pdos_data(pdos_data * to_free)
{
	if(to_free == NULL)
		return NULL;

	FreeLinearMem3( (void ***)to_free->pdos);

	for( int i=0; i<to_free->n_states; i++)
		FREE( to_free->states[i]);

	FREE( to_free->nscf);

	free( to_free);

	return NULL;
}

size_t totmem_pdos_data( pdos_data * a)
{
	size_t mem = 0;
	mem+=sizeof( a) + sizeof( *a);

	return mem;
}




















