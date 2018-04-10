#include <qepp/qepp_struct.h>

extern unsigned long int TOT_MEM;
extern unsigned int ionode;
extern int verbosity;

evc * initialize_evc( long int nc)
{
	evc * to_init = malloc( sizeof( evc));

	to_init->typeID = ID_EVC;
	to_init->size = sizeof( evc);
	
	to_init->nc = nc;
	to_init->val = (double complex *)AllocateLinearMem1( sizeof( double complex), nc);

	to_init->print =	&print_evc;
	to_init->duplicate =	&duplicate_evc;
	to_init->free = 	&free_evc;

	return to_init;
}

void * print_evc(evc * to_print, FILE * write)
{
	if( to_print == NULL)
		return NULL;
	if( write == NULL)
		return NULL;

	double complex app=0;
	for( long int i=0; i<to_print->nc; app=to_print->val[i++])
		QEPP_OUT( write, "%13.6E,%13.6E\n",creal(app),cimag(app));

	return NULL;
}

evc * duplicate_evc( evc * to_dupl)
{
	if( to_dupl == NULL)
		return NULL;

	long int nc = to_dupl->nc;
	evc * new_s = initialize_evc( nc);

	new_s->val =	(double complex *)DuplicateLinearMem1( (void *)to_dupl->val);

	return new_s;
}

void * free_evc( evc * to_free)
{
	if( to_free == NULL)
		return NULL;

	FreeLinearMem1( (void *)to_free->val);
	free( to_free);

	return NULL;
}




















