#include <qepp/qepp_struct.h>

extern unsigned long int TOT_MEM;
extern unsigned int ionode;
extern int verbosity;

nscf_md * initialize_nscf_md()
{
	nscf_md * to_init;

	to_init = calloc( 1, sizeof( nscf_md));
	to_init->typeID = ID_NSCF_MD;
	to_init->size = sizeof( nscf_md);

	to_init->spin_orbit = 0;

	to_init->print =	&print_nscf_md;
	to_init->duplicate =	&duplicate_nscf_md;
	to_init->free = 	&free_nscf_md;

	return to_init;
}

void * print_nscf_md(nscf_md * to_print, FILE * write)
{
	if( to_print == NULL)
		return NULL;
	if( write == NULL)
		return NULL;

	QEPP_OUT( write, "Name:\t\t%16s\nn_kpt:\t\t%16li\ne_fermi:\t%16.8f\nv_cel:\t\t%16.8f\nn_el:\t\t%16.8f\nn_bnd:\t\t%16i\n",
		to_print->formula, to_print->n_kpt, to_print->e_fermi, to_print->vc, to_print->n_el, to_print->n_bnd);

	return NULL;
}

nscf_md * duplicate_nscf_md( nscf_md * to_dupl)
{
	if( to_dupl == NULL)
		return NULL;

	nscf_md * new_s=initialize_nscf_md();

	memcpy(new_s,to_dupl,to_dupl->size);

	return new_s;
}

void * free_nscf_md(nscf_md * to_free)
{
	if(to_free == NULL)
		return NULL;

	free(to_free);


	return NULL;
}

































