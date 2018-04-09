#include <qepp/qepp_struct.h>

extern unsigned long int TOT_MEM;
extern unsigned int ionode;
extern int verbosity;

data_set * initialize_data_set( int n_pt)
{
	data_set * to_init = malloc( sizeof( data_set));
	to_init->typeID = ID_DATA_SET;
	to_init->size = sizeof( data_set);

	to_init->n_pt = n_pt;
	to_init->x = calloc( n_pt, sizeof( double));
	to_init->y = calloc( n_pt, sizeof( double));

	to_init->print =	&print_data_set;
	to_init->duplicate =	&duplicate_data_set;
	to_init->free = 	&free_data_set;

	return to_init;
}

void * print_data_set( data_set * to_print, FILE * write)
{
	if( to_print == NULL)
		return NULL;
	if( write == NULL)
		return NULL;

	for( int i=0; i<to_print->n_pt; i++)
		QEPP_OUT( write, "%16.8lf%16.8lf\n", to_print->x[i], to_print->y[i]);

	return NULL;
}

data_set * duplicate_data_set( data_set * to_dupl)
{
	if( to_dupl == NULL)
		return NULL;

	data_set * res = initialize_data_set( to_dupl->n_pt);
	memcpy( res->x, to_dupl->x, to_dupl->n_pt * sizeof( double));
	memcpy( res->y, to_dupl->y, to_dupl->n_pt * sizeof( double));

	return res;
}

void * free_data_set( data_set * to_free)
{
	if( to_free == NULL)
		return NULL;
	free( to_free->x);
	free( to_free->y);
	free( to_free);

	return NULL;
}


































