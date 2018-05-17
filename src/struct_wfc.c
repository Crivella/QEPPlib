#include <qepp/qepp_struct.h>





wfc * initialize_wfc( )
{
	wfc * to_init = malloc( sizeof( wfc));

	to_init->typeID = ID_WFC;
	to_init->size = sizeof( wfc);


	to_init->index = NULL;
	to_init->grid =  NULL;
	to_init->val =   NULL;

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

	long int igwx = to_print->igwx;

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

	if( to_print->index != NULL)
	{
		QEPP_OUT( write, "<INDEX>\n");
		for( long int i=0; i<igwx; i++)
		{
			QEPP_OUT( write, "%11d", to_print->index[i]);
			QEPP_OUT( write, "\n");
		}
		QEPP_OUT( write, "</INDEX>\n");
	}

	if( to_print->grid != NULL)
	{
		QEPP_OUT( write, "<GRID>\n");
		for( long int i=0; i<igwx; i++)
		{
			int * app = to_print->grid[i]; 
			for( int j=0; j<3; j++)
				QEPP_OUT( write, "%11d", app[j]);
			QEPP_OUT( write, "\n");
		}
		QEPP_OUT( write, "</GRID>\n");
	}

	for( int i=0; i<to_print->nbnd; i++)
	{
		QEPP_OUT( write, "  <evc.%d>\n",i+1);
		//evc * evc_app = to_print->evc_vect[i];
		double complex * app = to_print->val[i];
		for( long int j=0; j<igwx; j++)
			QEPP_OUT( write, "        %13.6E,%13.6E\n",creal(app[j]),cimag(app[j]));
		QEPP_OUT( write, "  </evc.%d>\n",i+1);
	}

	return NULL;
}

wfc * duplicate_wfc( wfc * to_dupl)
{
	if( to_dupl == NULL)
		return NULL;

	wfc * new_s = initialize_wfc( );

	new_s->ngw = to_dupl->ngw;
	new_s->igwx = to_dupl->igwx;
	new_s->gamma_only = to_dupl->gamma_only;
	new_s->nbnd = to_dupl->nbnd;
	new_s->ik = to_dupl->ik;
	new_s->nk = to_dupl->nk;
	new_s->ispin = to_dupl->ispin;
	new_s->nspin = to_dupl->nspin;
	new_s->scale_factor = to_dupl->scale_factor;
	new_s->max_igwx = to_dupl->max_igwx;
	for( int j=0; j<3; j++)
		new_s->kpt[j] = to_dupl->kpt[j];

	new_s->val =	QEPP_DUPL( to_dupl->val);
	new_s->index =	QEPP_DUPL( to_dupl->index);
	new_s->grid =	QEPP_DUPL( to_dupl->grid);

	return new_s;
}

void * free_wfc( wfc * to_free)
{
	if( to_free == NULL)
		return NULL;

	QEPP_FREE( to_free->val);
	QEPP_FREE( to_free->index);
	QEPP_FREE( to_free->grid);

	free( to_free);

	return NULL;
}




















