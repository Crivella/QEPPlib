#include <qepp/qepp_function.h>

errh * print_band_structure_nscf( nscf_data * data, char * filename)
{
	if( data == NULL)
		FAIL( NULL_IN, " ");
	FILE * write=NULL;
	if( filename != NULL)
		write = fopen(filename,"w");
	if( write == NULL)
		write = stdout;

	double x = 0, e_fermi;
	if( isnan(data->md->e_fermi))
	{
		parse_errh( find_e_fermi( &e_fermi, data));
		if( isnan( e_fermi))
		{
			FILE * read = fopen("scf_1.out","r");
			if( read != NULL)
			{
				if( qepp_get_value( "Fermi", read, 0, 0, R_FLT, &e_fermi))
					e_fermi = 0;
				fclose(read);
			}
			else
				e_fermi=0;
		}
	}
	else
		e_fermi = data->md->e_fermi;

	//e_fermi=0;
	for( long int i=0; i<data->n_kpt; i++)
	{
		if(i>0)
			x+=delta_k(data->kpt[i],data->kpt[i-1]);
		QEPP_OUT( write, "%lf\t",x);
		for(int i2=0; i2<data->n_bnd; i2++)
			QEPP_OUT( write, "%lf ",data->energies[i][i2]-e_fermi);
		QEPP_OUT( write, "\n");
	}

	fclose(write);
	SUCCESS();
}

errh * print_band_structure_band(band_data * data, char * filename)
{
	if( data == NULL)
		FAIL( NULL_IN, " ");
	FILE * write=NULL;
	if( filename != NULL)
		write = fopen(filename,"w");
	if( write == NULL)
		write = stdout;
	double x=0,e_fermi = -1E4;
	if( isnan( e_fermi))
	{
		FILE * read = fopen("scf_1.out","r");
		if( read != NULL)
		{
			if( qepp_get_value( "Fermi", read, 0, 0, R_FLT, &e_fermi))
				e_fermi = 0;
			fclose(read);
		}
		else
			e_fermi=0;
	}

	for( long int i=0; i<data->n_kpt; i++)
	{
		if(i>0)
			x+=delta_k(data->kpt[i],data->kpt[i-1]);
		QEPP_OUT( write, "%lf\t", x);
		for(int i2=0; i2<data->n_bnd; i2++)
			QEPP_OUT( write, "%lf ", data->bands[i][i2]-e_fermi);
		QEPP_OUT( write, "\n");
	}

	fclose(write);
	SUCCESS();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Find smallest gap (return the respective index for the given vector)
errh * find_smallest_gap( long int * out_ptr, nscf_data * data, double * center, double rad)
{
	if( out_ptr == NULL)
		FAIL( NULL_OUT, " ");
	if( data == NULL)
		FAIL( NULL_IN, " ");
	long int pos=-1;
	double app=0, app_dk=0;
	double min = 1.E5;
	int vb=0;
	if( !data->md->spin_orbit)
		vb = ceil(data->md->n_el/2)-1;
	else
		vb = data->md->n_el-1;
	int cb = vb+1;
	for( long int i=0; i < data->n_kpt; i++)
	{
		if(rad == 0 || center == NULL)
			app_dk=-1;
		else
			app_dk=delta_k(center,data->kpt[i]);
		if( app_dk < rad)
		{
			app = data->energies[i][cb] - data->energies[i][vb];
			if(app < min)
			{
				min=app;
				pos=i;
			}
		}
	}
	*out_ptr = pos;
	SUCCESS();
}

errh * find_smallest_gap2( long int * out_ptr, nscf_data * data, double * center, double rad)
{
	if( out_ptr == NULL)
		FAIL( NULL_OUT, " ");
	if( data == NULL)
		FAIL( NULL_IN, " ");

	long int pos=-1;
	double app=0, app_dk=0;
	double min = 1.E5;
	int vb=0;
	if( !data->md->spin_orbit)
		vb = ceil(data->md->n_el/2);
	else
		vb = data->md->n_el-1;
	int cb = vb+2;
	for( long int i=0; i < data->n_kpt; i++)
	{
		if(rad == 0 || center == NULL)
			app_dk=-1;
		else
			app_dk=delta_k(center,data->kpt[i]);
		if( app_dk < rad)
		{
			app = data->energies[i][cb] - data->energies[i][vb];
			if(app < min)
			{
				min=app;
				pos=i;
			}
		}
	}
	
	*out_ptr = pos;
	SUCCESS();
}

errh * find_smallest_gap3( long int * out_ptr, nscf_data * data, double * center, double rad)
{
	if( out_ptr == NULL)
		FAIL( NULL_OUT, " ");
	if( data == NULL)
		FAIL( NULL_IN, " ");

	long int pos=-1;
	double app=0, app_dk=0;
	double min = 1.E5;
	int vb=0;
	if( !data->md->spin_orbit)
		vb = ceil(data->md->n_el/2);
	else
		vb = data->md->n_el;
	int cb = vb+1;
	for( long int i=0; i < data->n_kpt; i++)
	{
		if(rad == 0 || center == NULL)
			app_dk=-1;
		else
			app_dk=delta_k(center,data->kpt[i]);
		if( app_dk < rad)
		{
			app = data->energies[i][cb] - data->energies[i][vb];
			if(app < min)
			{
				min=app;
				pos=i;
			}
		}
	}

	*out_ptr = pos;
	SUCCESS();
}

errh * find_max_v_bnd( long int * out_ptr, nscf_data * data, double * center, double rad)
{
	if( out_ptr == NULL)
		FAIL( NULL_OUT, " ");
	if( data == NULL)
		FAIL( NULL_IN, " ");

	long int pos = -1;
	double app = 0;
	double app_dk = -1;
	double max=-1.E5;
	int vb=0;
	if( !data->md->spin_orbit)
		vb = ceil(data->md->n_el/2)-1;
	else
		vb = data->md->n_el-1;
	for( long int i=0; i < data->n_kpt; i++)
	{
		if(rad == 0 || center == NULL)
			app_dk=-1;
		else
			app_dk=delta_k( center, data->kpt[i]);
		if( app_dk < rad)
		{
			app = data->energies[i][vb];
			if(app > max)
			{
				max = app;
				pos = i;
			}
		}
	}

	*out_ptr = pos;
	SUCCESS();
}

errh * find_min_c_bnd( long int * out_ptr, nscf_data * data, double * center, double rad)
{
	if( out_ptr == NULL)
		FAIL( NULL_OUT, " ");
	if( data == NULL)
		FAIL( NULL_IN, " ");

	long int pos = -1;
	double app = 0;
	double app_dk = -1;
	double min=1.E5;
	int vb=0;
	if( !data->md->spin_orbit)
		vb = ceil(data->md->n_el/2)-1;
	else
		vb = data->md->n_el-1;
	int cb = vb+1;
	for( long int i=0; i < data->n_kpt; i++)
	{
		if(rad == 0 || center == NULL)
			app_dk=-1;
		else
			app_dk=delta_k( center, data->kpt[i]);
		if( app_dk < rad)
		{
			app = data->energies[i][cb];
			if(app < min)
			{
				min = app;
				pos = i;
			}
		}
	}

	*out_ptr = pos;
	SUCCESS();
}

errh * find_min_opt_gap( long int * out_ptr, nscf_data * data, double * center, double rad)
{
	if( out_ptr == NULL)
		FAIL( NULL_OUT, " ");
	if( data == NULL)
		FAIL( NULL_IN, " ");

	long int pos = -1;
	double app_dk = -1;
	double min=1.E5;
	double ef = data->md->e_fermi;
//ef=14.5931;
	int vb=0;
	if( !data->md->spin_orbit)
		vb = ceil(data->md->n_el/2)-1;
	else
		vb = data->md->n_el-1;
	int cb = vb+1;
	for( long int i=0; i < data->n_kpt; i++)
	{
		if(rad == 0 || center == NULL)
			app_dk=-1;
		else
			app_dk=delta_k( center, data->kpt[i]);
		if( app_dk < rad)
		{
			double evb = data->energies[i][vb];
			double ecb = data->energies[i][cb];
			if(evb <= ef && ecb >= ef)
			{
				double gap = ecb - evb;
				if( gap < min)
				{
					min = gap;
					pos = i;
				}
			}
		}
	}

	*out_ptr = pos;
	SUCCESS();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Find fermi energy
errh * find_e_fermi( double * out_ptr, nscf_data * data)
{
	if( out_ptr == NULL)
		FAIL( NULL_OUT, " ");
	*out_ptr = NAN;
	if( data == NULL)
		FAIL( NULL_IN, " ");
	double e_fermi=NAN;
	double range=0.49, center=0.5;
	for( long int i=0; i < data->n_kpt; i++)
		for( int i2=0; i2 < data->n_bnd; i2++)
			if(data->occup[i][i2]>=center-range && data->occup[i][i2]<=center+range)
			{
				if(data->energies[i][i2] > e_fermi)
					e_fermi=data->energies[i][i2];
				break;
			}

	*out_ptr = e_fermi;
	SUCCESS();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Give tot number of electrons as sum_k[sum_band(occup*weight)]
errh * tot_elect( double * out_ptr, nscf_data * data)
{
	if( out_ptr == NULL)
		FAIL( NULL_OUT, " ");
	*out_ptr = -1;
	if( data == NULL)
		FAIL( NULL_IN, " ");
	double n_elect=0;
	for ( long int i=0; i<data->n_kpt; i++)
		for( int i2=0; i2<data->n_bnd; i2++)
			n_elect+=data->occup[i][i2]*data->weight[i];
	n_elect/=data->tot_weight;

	*out_ptr = n_elect;
	SUCCESS();
}






