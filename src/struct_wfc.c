#include <qepp/qepp_struct.h>

extern unsigned long int TOT_MEM;
extern unsigned int ionode;
extern int verbosity;

wfc * initialize_wfc( int n_evc)
{
	wfc * to_init = malloc( sizeof( wfc));

	to_init->typeID = ID_WFC;
	to_init->size = sizeof( wfc);

	to_init->n_evc = n_evc;
	to_init->evc_vect = calloc( n_evc, sizeof( evc *));

	to_init->nbnd = n_evc;
	//for( int i=0; i<n_evc; i++)
	//	to_init->evc_vect[i] = malloc( sizeof( evc));

	to_init->print =	&print_wfc;
	to_init->duplicate =	&duplicate_wfc;
	to_init->free = 	&free_wfc;

	return to_init;
}

void * print_wfc(wfc * to_print, FILE * write)
{
	if( to_print == NULL)
		return NULL;
	if( write == NULL)
		return NULL;

	QEPP_OUT( write, "<INFO>\n");
	QEPP_OUT( write, "%16s   =%16li\n","ngw",to_print->ngw);
	QEPP_OUT( write, "%16s   =%16li\n","igwx",to_print->igwx);
	if( to_print->gamma_only)
	{
		QEPP_OUT( write, "%16s   =%16c\n","gamma_only",'T');
	}
	else
		QEPP_OUT( write, "%16s   =%16c\n","gamma_only",'F');
	QEPP_OUT( write, "%16s   =%16d\n","nbnd",to_print->nbnd);
	QEPP_OUT( write, "%16s   =%16li\n","ik",to_print->ik);
	QEPP_OUT( write, "%16s   =%16li\n","nk",to_print->nk);
	QEPP_OUT( write, "%16s   =%16d\n","ispin",to_print->ispin);
	QEPP_OUT( write, "%16s   =%16d\n","nspin",to_print->nspin);
	QEPP_OUT( write, "%16s   =%16lf\n","scale_factor",to_print->scale_factor);
	QEPP_OUT( write, "</INFO>\n");

	for( int i=0; i<to_print->n_evc; i++)
	{
		QEPP_OUT( write, "  <evc.%d>\n",i+1);
		evc * evc_app = to_print->evc_vect[i];
		double complex app=evc_app->val[0];
		for( long int i=0; i<evc_app->nc; app=evc_app->val[i++])
			QEPP_OUT( write, "        %13.6E,%13.6E\n",creal(app),cimag(app));
		QEPP_OUT( write, "  </evc.%d>\n",i+1);
	}

	return NULL;
}

wfc * duplicate_wfc( wfc * to_dupl)
{
	if( to_dupl == NULL)
		return NULL;

	int n_evc = to_dupl->n_evc;
	wfc * new_s = initialize_wfc( n_evc);

	for( int i=0; i<n_evc; i++)
		new_s->evc_vect[i] = DUPLICATE( to_dupl->evc_vect[i]);

	return new_s;
}

void * free_wfc( wfc * to_free)
{
	if( to_free == NULL)
		return NULL;

	int n_evc = to_free->n_evc;
	for( int i=0; i<n_evc; i++)
		//if( to_free->evc_vect[i] != NULL)
		FREE( to_free->evc_vect[i]);
	free( to_free->evc_vect);
	free( to_free);

	return NULL;
}




















