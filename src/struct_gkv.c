#include <qepp/qepp_struct.h>

extern unsigned long int TOT_MEM;
extern unsigned int ionode;
extern int verbosity;

gkv * initialize_gkv( long int ngkv)
{
	gkv * to_init = malloc( sizeof( gkv));

	to_init->typeID = ID_GKV;
	to_init->size = sizeof( gkv);
	
	to_init->ngkv = ngkv;
	to_init->index = (int *)AllocateLinearMem1( sizeof( long int), ngkv);
	to_init->grid = (int **)AllocateLinearMem2( sizeof( int), ngkv, 3);

	to_init->print =	&print_gkv;
	to_init->duplicate =	&duplicate_gkv;
	to_init->free = 	&free_gkv;

	return to_init;
}

void * print_gkv(gkv * to_print, FILE * write)
{
	if( to_print == NULL)
		return NULL;
	if( write == NULL)
		return NULL;

	QEPP_OUT( write, "<INFO>\n");
	QEPP_OUT( write, "%16s   =%16li\n","ngkv",to_print->ngkv);
	QEPP_OUT( write, "%16s   =%16li\n","max_ngkv",to_print->max_ngkv);
	if( to_print->gamma_only)
	{
		QEPP_OUT( write, "%16s   =%16c\n","gamma_only",'T');
	}
	else
		QEPP_OUT( write, "%16s   =%16c\n","gamma_only",'F');
	QEPP_OUT( write, "</INFO>\n");

	QEPP_OUT( write, "<KPOINT_COORD>\n");
	for( int i=0; i<3; i++)
		QEPP_OUT( write, "%16.6lf\n",to_print->kpt[i]);
	QEPP_OUT( write, "</KPOINT_COORD>\n");

	QEPP_OUT( write, "<INDEX>\n");
	for( long int i=0; i<to_print->ngkv; i++)
		QEPP_OUT( write, "%16d\n",to_print->index[i]);
	QEPP_OUT( write, "</INDEX>\n");

	QEPP_OUT( write, "<GRID>\n");
	for( long int i=0; i<to_print->ngkv; i++)
	{
		for( int j=0; j<3; j++)
			QEPP_OUT( write, "%10d ",to_print->grid[i][j]);
		QEPP_OUT( write, "\n");
	}
	QEPP_OUT( write, "</GRID>\n");

	return NULL;
}

gkv * duplicate_gkv( gkv * to_dupl)
{
	if( to_dupl == NULL)
		return NULL;

	long int ngkv = to_dupl->ngkv;
	gkv * new_s = initialize_gkv( ngkv);

	new_s->index =	(int *)DuplicateLinearMem1( (void *)to_dupl->index);
	new_s->grid =	(int **)DuplicateLinearMem2( (void **)to_dupl->grid);

	return new_s;
}

void * free_gkv( gkv * to_free)
{
	if( to_free == NULL)
		return NULL;

	FreeLinearMem1( (void *)to_free->index);
	FreeLinearMem2( (void **)to_free->grid);
	free( to_free);

	return NULL;
}

































