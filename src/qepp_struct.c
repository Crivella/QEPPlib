#include <qepp/qepp_struct.h>
#include <qepp/qepp_read.h>

extern unsigned long int TOT_MEM;
extern int ionode;
extern int verbosity;

void * free_array_ptr(double  **to_free, long int size)
{
	if( to_free == NULL)
		return NULL;

	for( int i=0; i<size; i++)
		free(to_free[i]);
	
	return NULL;
}

void make_headers(opt_data * data, char ** h, int num_h)
{
	for( int i=0; i<data->n_col+1 && i<num_h; i++)
		if(h[i] != NULL)
			strcpy(data->headers[i],h[i]);

	return ;
}

errh * print_kpt( nscf_data * data, long int point, char * outname)
{
	if( data == NULL)
		FAIL( NULL_IN, " ");
	FILE * write=NULL;
	if(outname != NULL)
		write = fopen(outname, "w");
	if( write == NULL)
		write=stdout;

	if( point >= data->n_kpt)
		FAIL( FAIL, "Selected number for point is greater than max number of points in file");
	for( int i=0; i<3; i++)
		QEPP_OUT( write, "%8.4f", data->kpt[point][i]);
	for( int i=0; i<data->n_bnd; i++)
	{
		if( i%8 == 0)
			QEPP_OUT( write, "\n");
		QEPP_OUT( write, "%9.4f",data->energies[point][i]);
	}
	QEPP_OUT( write, "\n");
	for( int i=0; i<data->n_bnd; i++)
	{
		if( i%8 == 0)
			QEPP_OUT( write, "\n");
		QEPP_OUT( write, "%9.4f",data->occup[point][i]);
	}
	QEPP_OUT( write, "\n");

	SUCCESS();
}

errh * nscf_data_getband( band_data ** out_ptr, nscf_data * data)
{
	if( out_ptr == NULL)
		FAIL( NULL_OUT, " ");
	band_data * res;
	long int n_kpt = data->n_kpt;
	int n_bnd = data->n_bnd;

	res = initialize_band_data( n_bnd, n_kpt);

	for( long int i=0; i<n_kpt; i++)
	{
		memcpy( res->kpt[i], data->kpt[i], 3 * sizeof( double));
		memcpy( res->bands[i], data->energies[i], n_bnd * sizeof( double));
	}

	*out_ptr = res;
	SUCCESS();
}

errh * spin_data_getband( band_data ** out_ptr, spin_data * data, int dir, nscf_data * nscf)
{
	if( out_ptr == NULL)
		FAIL( NULL_OUT, " ");
	if( data == NULL)
		FAIL( NULL_IN, " ");
	if( dir<0 || dir>3)
		FAIL( FAIL, "Invalid parameters");

	band_data * res;
	long int n_kpt = data->n_kpt;
	int n_bnd = data->n_bnd;

	res = initialize_band_data( n_bnd, n_kpt);

	for( long int i=0; i<n_kpt; i++)
	{
		for( int j=0; j<n_bnd; j++)
		{
			double * ptr = data->m[i][j][dir];
			double app = sqrt( pow(ptr[0],2) + pow(ptr[1],2));
			res->bands[i][j]=app;
			if( nscf != NULL)
				memcpy( res->kpt[i], nscf->kpt[i], 3 * sizeof( double));
		}
	}

	*out_ptr = res;
	SUCCESS();
}

errh * band_pp_getopt( opt_data ** out_ptr, band_pp * data)
{
	if( out_ptr == NULL)
		FAIL( NULL_OUT, " ");
	int nbnd = data->n_bnd;
	long int nkpt = data->n_kpt;
	
	FILE * read = NULL;
	char filename[128];
	int check=0;
	if( (read = fopen( "nscf_1.out", "r")) != NULL)
		strcpy( filename, "nscf_1.out");
	else if( (read = fopen( "bands_1.out", "r")) != NULL) {
		strcpy( filename, "bands_1.out"); check=1;}
	if( read != NULL)
		fclose( read);
	else
		FAIL( FAIL, "No valid nscf output found");

	nscf_data * app;// = READ( filename, ID_NSCF_DATA);
	READ( filename, &app);
	//int nbnd_occ = app->md->n_el;
	long int nl = 0; //nbnd*(nbnd-(nbnd_occ-3))*npt;
	for( long int i=0; i<nkpt; i++)
	{
		int nbndocc =data->n_bnd_occ[i];
		int lim = nbnd - nbndocc;
		nl += nbndocc*lim;
	}

	opt_data * res = initialize_opt_data( nl, 7);

	long int npt=0;
	for( long int i=0; i<nkpt; i++)
	{
		int nbndocc =data->n_bnd_occ[i];
		int lim = nbnd - nbndocc;
		for( int j=0; j<nbndocc; j++)
		{
			for( int k=0; k<lim; k++)
			{
				res->x[npt] = i+1;
				res->values[npt][0] = j+1;
				res->values[npt][1] = k+nbndocc+1;
				for( int i1=0; i1<3; i1++)
					res->values[npt][i1+2] = data->pp[i][j][k][i1];
				res->values[npt][5] = app->energies[i][nbndocc+k] - app->energies[i][j];
				if( check == 0)
					res->values[npt][6] = 2*( app->occup[i][nbndocc+k] - app->occup[i][j]);
				else
					res->values[npt][6] = 2;
				npt++;
			}
		}
	}

	*out_ptr = res;
	SUCCESS();
}






















