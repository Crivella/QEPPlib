#include <qepp/qepp_struct.h>





pseudo * initialize_pseudo( )
{
	pseudo * to_init = malloc( sizeof( pseudo));
	to_init->typeID = ID_PSEUDO;
	to_init->size = sizeof( pseudo);

	to_init->grid = NULL;
	to_init->rab = NULL;
	to_init->rho_atc = NULL;
	to_init->vloc = NULL;

	to_init->kkbeta = NULL;
	to_init->beta = NULL;
	to_init->lll = NULL;
	to_init->cri = NULL;
	to_init->cut_rad = NULL;
	to_init->dij = NULL;

	to_init->chi = NULL;
	to_init->els = NULL;
	to_init->lchi = NULL;
	to_init->occ = NULL;

	to_init->rho_at = NULL;


	to_init->print =	&print_pseudo;
	to_init->duplicate =	&duplicate_pseudo;
	to_init->free = 	&free_pseudo;

	return to_init;
}

void * print_pseudo( pseudo * to_print, FILE * write)
{
	if( to_print == NULL)
		return NULL;
	if( write == NULL)
		return NULL;

	unsigned long int mesh = to_print->mesh;
	unsigned int n_beta    = to_print->n_beta;
	//unsigned int natwf     = to_print->natwf;

	QEPP_OUT( write, "%6s%9s%9s%9s", "index", "grid", "rab", "vloc");
	for( unsigned int i=0; i<n_beta; i++)
	{
		QEPP_OUT( write, "%5s(%2u)", "beta", i);
	}
	QEPP_OUT( write, "\n");
	for( unsigned long int i=0; i<mesh; i++)
	{
		QEPP_OUT( write, "%6lu", i);
		QEPP_OUT( write, "%9.4lf", to_print->grid[i]);
		QEPP_OUT( write, "%9.4lf", to_print->rab[i]);
		QEPP_OUT( write, "%9.4lf", to_print->vloc[i]);
		for( unsigned int j=0; j<n_beta; j++)
		{
			QEPP_OUT( write, "%9.4lf", to_print->beta[j][i]);
		}

		QEPP_OUT( write, "\n");
	}

	QEPP_OUT( write, "\n");
	QEPP_OUT( write, "\n");

	QEPP_OUT( write, "D_ij\n");
	for( unsigned int i=0; i<n_beta; i++)
	{
		for( unsigned int j=0; j<n_beta; j++)
			QEPP_OUT( write, "%10.5lf", to_print->dij[i][j]);
		QEPP_OUT( write, "\n");
	}

	return NULL;
}

pseudo * duplicate_pseudo( pseudo * to_dupl)
{
	if( to_dupl == NULL)
		return NULL;

	pseudo * new_s = initialize_pseudo( );

	new_s->mesh    = to_dupl->mesh;
	new_s->n_beta  = to_dupl->n_beta;
	new_s->natwf  = to_dupl->natwf;

	new_s->grid    = QEPP_DUPL( to_dupl->grid);
	new_s->rab     = QEPP_DUPL( to_dupl->rab);
	new_s->rho_atc = QEPP_DUPL( to_dupl->rho_atc);
	new_s->vloc    = QEPP_DUPL( to_dupl->vloc);

	new_s->kkbeta  = QEPP_DUPL( to_dupl->kkbeta);
	new_s->beta    = QEPP_DUPL( to_dupl->beta);
	new_s->lll     = QEPP_DUPL( to_dupl->lll);
	new_s->cri     = QEPP_DUPL( to_dupl->cri);
	new_s->cut_rad = QEPP_DUPL( to_dupl->cut_rad);
	new_s->dij     = QEPP_DUPL( to_dupl->dij);

	new_s->chi     = QEPP_DUPL( to_dupl->chi);
	new_s->els     = QEPP_DUPL( to_dupl->els);
	new_s->lchi    = QEPP_DUPL( to_dupl->lchi);
	new_s->occ     = QEPP_DUPL( to_dupl->occ);

	new_s->rho_at     = QEPP_DUPL( to_dupl->rho_at);

	return new_s;
}

void * free_pseudo( pseudo * to_free)
{
	if( to_free == NULL)
		return NULL;

	QEPP_FREE( to_free->grid);
	QEPP_FREE( to_free->rab);
	QEPP_FREE( to_free->rho_atc);
	QEPP_FREE( to_free->vloc);

	QEPP_FREE( to_free->kkbeta);
	QEPP_FREE( to_free->beta);
	QEPP_FREE( to_free->lll);
	QEPP_FREE( to_free->cri);
	QEPP_FREE( to_free->cut_rad);
	QEPP_FREE( to_free->dij);

	QEPP_FREE( to_free->chi);
	QEPP_FREE( to_free->els);
	QEPP_FREE( to_free->lchi);
	QEPP_FREE( to_free->occ);

	QEPP_FREE( to_free->rho_at);

	free( to_free);

	return NULL;
}


































