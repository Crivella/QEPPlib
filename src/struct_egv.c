#include <qepp/qepp_struct.h>

extern unsigned long int TOT_MEM;
extern unsigned int ionode;
extern int verbosity;

egv * initialize_egv( int n_bnd)
{
	egv * to_init = malloc( sizeof( egv));

	to_init->typeID = ID_EGV;
	to_init->size = sizeof( egv);
	
	to_init->n_bnd = n_bnd;
	to_init->val = (double *)AllocateLinearMem1( sizeof( double), n_bnd);
	to_init->occ = (double *)AllocateLinearMem1( sizeof( double), n_bnd);

	to_init->print =	&print_egv;
	to_init->duplicate =	&duplicate_egv;
	to_init->free = 	&free_egv;

	return to_init;
}

void * print_egv(egv * to_print, FILE * write)
{
	if( to_print == NULL)
		return NULL;
	if( write == NULL)
		return NULL;

	QEPP_OUT( write, "<EGV>\n");

	QEPP_OUT( write, "<EIGENVALUES>\n");
	for( int i=0; i<to_print->n_bnd; i++)
		QEPP_OUT( write, "%16.8lf\n",to_print->val[i]);
	QEPP_OUT( write, "</EIGENVALUES>\n");

	QEPP_OUT( write, "<OCCUPATIONS>\n");
	for( int i=0; i<to_print->n_bnd; i++)
		QEPP_OUT( write, "%16.8lf\n",to_print->occ[i]);
	QEPP_OUT( write, "</OCCUPATIONS>\n");

	return NULL;
}

egv * duplicate_egv( egv * to_dupl)
{
	if( to_dupl == NULL)
		return NULL;

	long int n_bnd = to_dupl->n_bnd;
	egv * new_s = initialize_egv( n_bnd);

	new_s->val =	(double *)DuplicateLinearMem1( (void *)to_dupl->val);
	new_s->occ =	(double *)DuplicateLinearMem1( (void *)to_dupl->occ);

	return new_s;
}

void * free_egv( egv * to_free)
{
	if( to_free == NULL)
		return NULL;

	FreeLinearMem1( (void *)to_free->val);
	FreeLinearMem1( (void *)to_free->occ);
	free( to_free);

	return NULL;
}

































