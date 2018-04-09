#include <qepp/qepp_struct.h>

extern unsigned long int TOT_MEM;
extern unsigned int ionode;
extern int verbosity;

opt_data * initialize_opt_data( long int n_pt, int n_col)
{
	opt_data * to_init;

	to_init = calloc( 1, sizeof(opt_data));
	to_init->typeID = ID_OPT_DATA;
	to_init->size = sizeof(opt_data);

	to_init->n_pt = n_pt;
	to_init->n_col= n_col;

	to_init->x =		(double *)AllocateLinearMem1( sizeof( double), n_pt);
	to_init->values =	(double **)AllocateLinearMem2( sizeof( double), n_pt, n_col);
	to_init->headers =	(char **)AllocateLinearMem2( sizeof( char), n_col+1, MAX_H_SIZE);
	for( int i=0; i<n_col+1; i++)
		to_init->headers[i][0] = '\0';


	to_init->print =	&print_opt_data;
	to_init->duplicate =	&duplicate_opt_data;
	to_init->free = 	&free_opt_data;

	return to_init;
}

void * print_opt_data(opt_data * to_print, FILE * write)
{
	if( to_print == NULL)
		return NULL;
	if( write == NULL)
		return NULL;

	//Print headers
	QEPP_OUT(write,"#");
	for( int i=0; i<to_print->n_col+1; i++)
		if(to_print->headers[i][0] != '\0')
		{
			if(i==0)
				QEPP_OUT(write,"%12s",to_print->headers[i]);
			else
				QEPP_OUT(write,"%16s",to_print->headers[i]);
		}
		else
		{
			if(i==0)
				QEPP_OUT(write,"%15s","noname");
			else
				QEPP_OUT(write,"%16s","noname");
		}
	QEPP_OUT(write,"\n");
	
	//Print to_print
	for( long int i=0; i<to_print->n_pt; i++)
	{
		QEPP_OUT(write,"%13.8f",to_print->x[i]);
		for( int i2=0; i2<to_print->n_col; i2++)
			QEPP_OUT(write,"%16g",to_print->values[i][i2]);
		QEPP_OUT(write,"\n");
	}

	return NULL;
}

opt_data * duplicate_opt_data(opt_data * to_dupl)
{
	if( to_dupl == NULL)
		return NULL;
	long int n_pt=to_dupl->n_pt;
	int n_col=to_dupl->n_col;

	opt_data * new_s=initialize_opt_data( n_pt, n_col);

	new_s->x =	(double *)DuplicateLinearMem1( (void *)to_dupl->x);
	new_s->values =	(double **)DuplicateLinearMem2( (void **)to_dupl->values);
	new_s->headers =(char **)DuplicateLinearMem2( (void **)to_dupl->headers);

	return new_s;
}

void * free_opt_data(opt_data * to_free)
{
	if(to_free == NULL)
		return NULL;

	FreeLinearMem1( (void *)to_free->x);
	FreeLinearMem2( (void **)to_free->values);
	FreeLinearMem2( (void **)to_free->headers);
	

	free(to_free);

	return NULL;
}























