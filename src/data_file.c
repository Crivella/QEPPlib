#include <qepp/qepp_struct.h>

extern unsigned long int TOT_MEM;
extern unsigned int ionode;
extern int verbosity;

data_file * initialize_data_file()
{
	data_file * to_init = malloc( sizeof( data_file));
	to_init->typeID = ID_DATA_FILE;
	to_init->size = sizeof( data_file);

	to_init->kpt = 		NULL;
	to_init->weight = 	NULL;
	to_init->egval_link = 	NULL;
	to_init->egvec_link = 	NULL;
	to_init->wfc_link = 	NULL;
	to_init->wfc_link2 = NULL;

	to_init->print =	&print_data_file;
	to_init->duplicate =	&duplicate_data_file;
	to_init->free = 	&free_data_file;

	return to_init;
}

void * print_data_file( data_file * to_print, FILE * write)
{
	if( to_print == NULL)
		return NULL;
	if( write == NULL)
		return NULL;

	QEPP_OUT( write, "N_el:\t%.1f\n", to_print->n_el);
	QEPP_OUT( write, "Bravais:\t%s\n", to_print->bravais);

	return NULL;
}

data_file * duplicate_data_file( data_file * to_dupl)
{
	if( to_dupl == NULL)
		return NULL;

	data_file * new_s = initialize_data_file();
	memcpy( new_s, to_dupl, sizeof( data_file));	

	new_s->kpt =		(double **)DuplicateLinearMem2( (void **)to_dupl->kpt);
	new_s->weight =		(double *)DuplicateLinearMem1( (void *)to_dupl->weight);
	new_s-> egval_link =	(char **)DuplicateLinearMem2( (void **)to_dupl->egval_link);
	new_s-> egvec_link =	(char **)DuplicateLinearMem2( (void **)to_dupl->egvec_link);
	new_s-> wfc_link =	(char **)DuplicateLinearMem2( (void **)to_dupl->wfc_link);

	return new_s;
}

void * free_data_file( data_file * to_free)
{
	if( to_free == NULL)
		return NULL;
	if( to_free->kpt != NULL)
		FreeLinearMem2( (void **)to_free->kpt);
	if( to_free->weight != NULL)
		FreeLinearMem1( (void *)to_free->weight);
	if( to_free->egval_link != NULL)
		FreeLinearMem2( (void **)to_free->egval_link);
	if( to_free->egvec_link != NULL)
		FreeLinearMem2( (void **)to_free->egvec_link);
	if( to_free->wfc_link != NULL)
		FreeLinearMem2( (void **)to_free->wfc_link);
	if( to_free->wfc_link2 != NULL)
		FreeLinearMem2( (void **)to_free->wfc_link2);

	free( to_free);

	return NULL;
}


































