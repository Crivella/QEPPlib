#include <qepp/qepp_struct.h>

extern unsigned long int TOT_MEM;
extern unsigned int ionode;
extern int verbosity;

pdos_state * initialize_pdos_state()
{
	pdos_state * to_init;
	
	to_init = calloc( 1, sizeof( pdos_state));
	to_init->typeID = ID_PDOS_STATE;
	to_init->size = sizeof( pdos_state);

	to_init->print =	&print_pdos_state;
	to_init->duplicate =	&duplicate_pdos_state;
	to_init->free = 	&free_pdos_state;

	return to_init;
}

void * print_pdos_state(pdos_state * to_print, FILE * write)
{
	if( to_print == NULL)
		return NULL;
	if( write == NULL)
		return NULL;

	QEPP_OUT( write, "atom   %d (%s), wfc  %d (j=%.1lf l=%d m_j= %.1lf)\n", 
			to_print->atom_n, to_print->name, to_print->wfc_n, to_print->j, to_print->l, to_print->m_j);
	//to fill------------------------------------------------------------

	return NULL;
}

pdos_state * duplicate_pdos_state(pdos_state *to_dupl)
{
	if( to_dupl == NULL)
		return NULL;

	pdos_state * new_s=initialize_pdos_state();

	//to fill---------------------------------------------------------------

	return new_s;	
}

void * free_pdos_state(pdos_state * to_free)
{
	if(to_free == NULL)
		return NULL;

	free(to_free);

	return NULL;
}

size_t totmem_pdos_state( pdos_state * a)
{
	size_t mem = 0;
	mem+=sizeof( a) + sizeof( *a);

	return mem;
}




















