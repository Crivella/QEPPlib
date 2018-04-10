#include <qepp/qepp_struct.h>

extern unsigned long int TOT_MEM;
extern unsigned int ionode;
extern int verbosity;

fit_params * initialize_fit_params( int nparams)
{
	fit_params * to_init = NULL;
	to_init = malloc( sizeof( fit_params));
	to_init->typeID = ID_FIT_PARAMS;
	to_init->size = sizeof( fit_params);

	to_init->nparams = nparams;

	to_init->params =	(double *)AllocateLinearMem1( sizeof( double), nparams);
	to_init->min =		(double *)AllocateLinearMem1( sizeof( double), nparams);
	to_init->max =		(double *)AllocateLinearMem1( sizeof( double), nparams);
	to_init->delta =	(double *)AllocateLinearMem1( sizeof( double), nparams);

	to_init->offset=0;

	to_init->print =	&print_fit_params;
	to_init->duplicate =	&duplicate_fit_params;
	to_init->free = 	&free_fit_params;

	return to_init;
}

void * print_fit_params( fit_params * to_print, FILE * write)
{
	if( to_print == NULL)
		return NULL;
	if( write == NULL)
		return NULL;

	for( int i=0; i<to_print->nparams; i++)
		QEPP_OUT( write, "(%2d)   ->%16.8lf\n",i,to_print->params[i]);
	QEPP_OUT( write, "offset ->%16.8lf\n",to_print->offset);

	return NULL;
}

fit_params * duplicate_fit_params( fit_params * to_dupl)
{
	if( to_dupl == NULL)
		return NULL;

	fit_params * res = initialize_fit_params( to_dupl->nparams);

	res->params =	(double *)DuplicateLinearMem1( (void *)to_dupl->params);
	res->min =	(double *)DuplicateLinearMem1( (void *)to_dupl->min);
	res->max =	(double *)DuplicateLinearMem1( (void *)to_dupl->max);
	res->delta =	(double *)DuplicateLinearMem1( (void *)to_dupl->delta);

	res->offset = to_dupl->offset;

	return res;
}

void * free_fit_params(fit_params * to_free)
{
	if( to_free == NULL)
		return NULL;

	FreeLinearMem1( (void *)to_free->params);
	FreeLinearMem1( (void *)to_free->min);
	FreeLinearMem1( (void *)to_free->max);
	FreeLinearMem1( (void *)to_free->delta);

	free( to_free);

	return NULL;
}





















