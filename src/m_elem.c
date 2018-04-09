#include <qepp/qepp_struct.h>

extern unsigned long int TOT_MEM;
extern unsigned int ionode;
extern int verbosity;

m_elem * initialize_m_elem( long int n_kpt, int n_bnd)
{
	m_elem * to_init;

	to_init = calloc( 1, sizeof( m_elem));
	to_init->typeID = ID_M_ELEM;
	to_init->size = sizeof( m_elem);

	to_init->n_kpt = n_kpt;
	to_init->n_bnd = n_bnd;
	
	fprintf( stdout, "Allocating %.2lf MB of RAM...\n", (double)sizeof( double)*n_kpt*n_bnd*n_bnd*(2+3)/(1024.*1024.));

	to_init->pp =	(double ****)AllocateLinearMem4( sizeof( double), n_kpt, n_bnd, n_bnd, 3);
	to_init->gap =	(double ***)AllocateLinearMem3( sizeof( double), n_kpt, n_bnd, n_bnd);
	to_init->df =	(double ***)AllocateLinearMem3( sizeof( double), n_kpt, n_bnd, n_bnd);


	to_init->print =	&print_m_elem;
	to_init->duplicate =	&duplicate_m_elem;
	to_init->free = 	&free_m_elem;

	return to_init;
}

void * print_m_elem(m_elem * to_print, FILE * write)
{
	if( to_print == NULL)
		return NULL;
	if( write == NULL)
		return NULL;

	return NULL;
}

m_elem * duplicate_m_elem(m_elem * to_dupl)
{
	if( to_dupl == NULL)
		return NULL;
	long int n_kpt=to_dupl->n_kpt;
	int n_bnd = to_dupl->n_bnd;

	m_elem * new_s=initialize_m_elem( n_kpt, n_bnd);

	new_s->pp =	(double ****)DuplicateLinearMem4( (void ****)to_dupl->pp);
	new_s->gap =	(double ***)DuplicateLinearMem3( (void ***)to_dupl->gap);
	new_s->df =	(double ***)DuplicateLinearMem3( (void ***)to_dupl->df);

	return new_s;
}

void * free_m_elem(m_elem * to_free)
{
	if(to_free == NULL)
		return NULL;

	FreeLinearMem4( (void ****)to_free->pp);
	FreeLinearMem3( (void ***)to_free->gap);
	FreeLinearMem3( (void ***)to_free->df);

	free( to_free);
	
	return NULL;
}
