#include <qepp/qepp_main.h>
//#include <gsl/gsl_math.h>
//#include <gsl/gsl_integration.h>

//extern mpi_data * mpi;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////             PP             //////////////////////////////////////////////////////
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
				if( get_value( "Fermi", read, 0, 0, R_FLT, &e_fermi))
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
			if( get_value( "Fermi", read, 0, 0, R_FLT, &e_fermi))
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////       PP on opt_data       //////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Apply broadening to data
errh * apply_broadening( opt_data ** out_ptr, opt_data * data, double sigma)
{
	if( out_ptr == NULL)
		FAIL( NULL_OUT, " ");
	*out_ptr = NULL;
	if( data == NULL)
		FAIL( NULL_IN, " ");

	QEPP_PRINT( "Applying broadening of %f...\n", sigma);
	opt_data * new_data = initialize_opt_data( data->n_pt, data->n_col);
	for( int i=0; i<data->n_col+1; i++)
		strcpy( new_data->headers[i], data->headers[i]);
	//MAKE_HEADERS(new_data,EPS_HEADERS);
	
	double gauss,norm;
	//int check =0;
	double dx = data->x[1] - data->x[0];
	long int di = 6 * sigma / dx;
	if( di <= 4)
		di = 4;
	//double sigma_app=sigma;
	double x1=0,x2=0;
	long int lim,start;
	for( long int i1=0; i1<data->n_pt; i1++)
	{
		x1 = data->x[i1];
		/*
		if( -3.1288 <= x1 && x1 <= -3.1286)
			sigma = sigma_app/20;
		else
			sigma = sigma_app;
		// */
		
		norm=0;
		new_data->x[i1]=x1;
		/*
		if( !check && data->x[i1] > 2*sigma)
		{
			sigma*=2;
			check =1;
		}// */
		for( int i3=0; i3<data->n_col; i3++)
			new_data->values[i1][i3]=0;
		lim=i1+di;
		if( lim >= data->n_pt)
			lim = data->n_pt;
		start = i1-di;
		if( start < 0)
			start = 0;
		for( long int i2=start; i2<lim; i2++)
		{
			x2 = data->x[i2];
			gauss=(x1-x2)/sigma;
			gauss=exp(-gauss*gauss/2);
			norm+=gauss;
			for( int i3=0; i3<data->n_col; i3++)
				new_data->values[i1][i3]+=gauss*data->values[i2][i3];
		}
		for( int i3=0; i3<data->n_col; i3++)
			new_data->values[i1][i3]/=norm;
	}

	QEPP_PRINT( "End of broadening calculation\n");

	*out_ptr = new_data;
	SUCCESS();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Calculate img part of dielectric function from matrixelements
errh * calculate_eps( opt_data ** out_ptr, opt_data * data, double max_range, long int n_pt, double a3, double v_cell)
{
	if( out_ptr == NULL)
		FAIL( NULL_OUT, " ");
	*out_ptr = NULL;
	if( data == NULL)
		FAIL( NULL_IN, " ");

	double delta = max_range / (double)n_pt;
	n_pt++;
	double fact=1;
	char * filename="nscf_1.out";
	if( !is_file( filename) && (v_cell == 0 || !is_file( "k.dat")))
	{
		filename = get_file( filename, ".out");
	}

	if( max_range == 0 || n_pt == 0)
		FAIL( FAIL, "Invalid input parameters");
	nscf_md * app;
	if( READ( filename, &app) != SUCCESS)
		FAIL( FAIL, "Failed to read metadata from nscf file");
	if(v_cell == 0)
	{
		if( app == NULL)
			FAIL( FAIL, "Can't acquire cell volume data");
		v_cell = app->vc;
	}
	if( a3 == 0)	//Bulk
		a3=1;
	else		//Surface
		fact=0.5;

//Read k points data from k.dat or if not available nscf_1.out or return NULL
	double tot_w=0;
	long int n_kpt;
	double * weight;
	opt_data * k_pt;
	if( READ( "k.dat", &k_pt) != SUCCESS)
	{
		nscf_data * app;
		if( READ( filename, &app) != SUCCESS)
			FAIL( FAIL, "Can't acquire k-points data");
		n_kpt=app->n_kpt;
		tot_w = app->tot_weight/2;
		weight = (double *)AllocateLinearMem1( sizeof( double), n_kpt);//malloc( n_kpt * sizeof( double));
		memcpy(weight, app->weight, app->n_kpt * sizeof( double));
		FREE(app);
	}
	else
	{
		n_kpt=k_pt->n_pt;
		weight = (double *)AllocateLinearMem1( sizeof( double), n_kpt);//malloc( n_kpt * sizeof( double));
		double app;
		for( long int i=0; i<k_pt->n_pt; i++)
		{
			app = k_pt->values[i][2];
			weight[i]=app;
			tot_w+=app;
		}
	}
	for( long int i=0; i< n_kpt; i++)
		weight[i]/=tot_w;


//Start actual calc
	//Generate result sructure
	opt_data * eps = initialize_opt_data( n_pt, 4);
	MAKE_HEADERS(eps,EPS_HEADERS);
	//Initialize x and y values of result
	for( long int i=0; i< n_pt; i++)
	{
		eps->x[i]= (double)i * delta;
		for( int i1=0; i1<eps->n_col; i1++)
			eps->values[i][i1]=0;
	}
//Read matrixelements and redistribute them in
//matrix element[k][x->en][i/j/k]]
	QEPP_PRINT( "Allocating %.2lf MB of RAM for pp...\n", n_kpt*n_pt*3.*8./1024./1024.);
	double ***pp;
	pp = (double ***)AllocateLinearMem3( sizeof( double), n_kpt, n_pt, 3);
	QEPP_PRINT( "...done\n");
// data->values[x] =?   x: 0-> v_b  1-> c_b  2/3/4-> |p_x/y/z|^2  5-> e_gap  6-> Df(occ)
	long int k_app, p_app;
	for( long int i=0; i<data->n_pt; i++)
	{
		k_app=(long int)data->x[i]-1;
		p_app=(long int)ROUND(data->values[i][5]/delta);
		if(p_app < n_pt && p_app >=0)
			if(data->values[i][0] < data->values[i][1])
			for( int i1=0; i1<3; i1++)
				pp[k_app][p_app][i1]+=data->values[i][i1+2]*data->values[i][6]/pow(data->values[i][5],2);
	}

	double prefact = fact * 4.0 * pow(PI,2) * pow(2*RY_TO_EV,3) * a3 / v_cell;
	if( app != NULL)
		if( app->spin_orbit)
		{
			prefact/=2;
			WARN ("Spinorbit calculation detected");
		}

	FREE( app);
//Trasform pp in eps_2[k][x->en][i/j/k]
	for( long int i=0; i<n_kpt; i++)
		for( long int i1=1; i1<n_pt; i1++)
			for( int i2=0; i2<3; i2++)
				pp[i][i1][i2]*=prefact*weight[i]/delta; 

//Recalc all for x[i] -> (x[i]+x[i-1])/2
	for( long int i=eps->n_pt-1; i>0; i--)
	{
		eps->x[i]=(eps->x[i]+eps->x[i-1])/2.;
		for( long int i1=0; i1<n_kpt; i1++ )
			for( int i2=0; i2<3; i2++)
				eps->values[i][i2]+=(pp[i1][i][i2] + pp[i1][i-1][i2])/2.;
	}
//Calc epsTOT
	for( long int i1=0; i1<n_pt; i1++ )
	{
		for( int i2=0; i2<3; i2++)
			eps->values[i1][3]+=eps->values[i1][i2];
		eps->values[i1][3]/=3.0;
	}
//Remove 0 pt
	eps->n_pt--;
	double * app1 = (double *)AllocateLinearMem1( sizeof( double), eps->n_pt);
	memcpy( app1, eps->x+1, eps->n_pt * sizeof( double));
	FreeLinearMem1( (void *)eps->x);
	eps->x=app1;

	double ** app2 = (double **)AllocateLinearMem2( sizeof(double), eps->n_pt, eps->n_col);
	for( long int i=0; i<eps->n_pt; i++)
		memcpy( app2[i], eps->values[i+1], eps->n_col * sizeof( double));
	FreeLinearMem2( (void **)eps->values);
	eps->values=app2;

//Free all unused variables
	FREE(k_pt);
	FreeLinearMem1( (void *)weight);
	FreeLinearMem3( (void ***)pp);

	*out_ptr = eps;
	QEPP_PRINT( "End eps calculation\n");
	SUCCESS();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
errh * calculate_eps_huge_files( opt_data ** out_ptr, char * mname, double max_range, long int n_pt, double a3, double v_cell)
{
	if( out_ptr == NULL)
		FAIL( NULL_OUT, " ");
	*out_ptr = NULL;
	if( mname == NULL)
		FAIL( NULL_IN, " ");

	double delta = max_range / (double)n_pt;
	n_pt++;
	double fact=1;
	char *filename="nscf_1.out";
	if( !is_file( filename) && (v_cell == 0 || !is_file( "k.dat")))
		filename = get_file( filename, ".out");

	if( max_range == 0 || n_pt == 0)
		FAIL( FAIL, "Invalid input parameters");
	nscf_md * app;
	if( READ( filename, &app) != SUCCESS)
		FAIL( FAIL, "Failed to read metadata from nscf file");
	if(v_cell == 0)
	{
		if( app == NULL)
			FAIL( FAIL, "Can't acquire cell volume data");
		v_cell = app->vc;
	}
	if( a3 == 0)	//Bulk
		a3=1;
	else		//Surface
		fact=0.5;

//Read k points data from k.dat or if not available nscf_1.out or return NULL
	double tot_w=0;
	long int n_kpt;
	double * weight;
	opt_data * k_pt=NULL;
	if( READ( "k.dat", &k_pt) != SUCCESS)
	{
		nscf_data * app;
		if( READ( filename, &app) != SUCCESS)
			FAIL( FAIL, "Can't acquire k-points data");
		n_kpt=app->n_kpt;
		tot_w = app->tot_weight/2;
		weight = (double *)AllocateLinearMem1( sizeof( double), n_kpt);//malloc( n_kpt * sizeof( double));
		memcpy(weight, app->weight, app->n_kpt * sizeof( double));
		FREE(app);
	}
	else
	{
		n_kpt= k_pt->n_pt;
		weight = (double *)AllocateLinearMem1( sizeof( double), n_kpt);//malloc( n_kpt * sizeof( double));
		double app;
		for( long int i=0; i<k_pt->n_pt; i++)
		{
			app = k_pt->values[i][2];
			weight[i]=app;
			tot_w+=app;
		}
	}
	for( long int i=0; i< n_kpt; i++)
		weight[i]/=tot_w;

//Start actual calc
	//Generate result sructure
	FILE * read = fopen( mname , "r");
	if( read == NULL)
		FAIL( OPEN_IN_FAIL, " ");

	opt_data * eps = initialize_opt_data( n_pt, 4);
	MAKE_HEADERS(eps,EPS_HEADERS);
	//Initialize x and y values of result
	for( long int i=0; i< n_pt; i++)
	{
		eps->x[i]= (double)i * delta;
		for( int i1=0; i1<eps->n_col; i1++)
			eps->values[i][i1]=0;
	}
//Read matrixelements and redistribute them in
//matrix element[k][x->en][i/j/k]]
	QEPP_PRINT( "Allocating %.2lf MB of RAM for pp...\n", n_kpt*n_pt*3.*8./1024./1024.);
	double ***pp;
	pp = (double ***)AllocateLinearMem3( sizeof( double), n_kpt, n_pt, 3);
	QEPP_PRINT( "...done\n");
// data->values[x] =?   x: 0-> v_b  1-> c_b  2/3/4-> |p_x/y/z|^2  5-> e_gap  6-> Df(occ)
	long int k_app, p_app;
	double e_gap=0;
	int vb=0, cb=0;
	double kpp[3];
	double df=0;

	long int npt=0;
	char buffer[256];
	do {
		my_getline( buffer, 256, read);
	} while( !feof( read) && buffer[0] == '#');
	QEPP_PRINT( "Start reading from file \"%s\"...\n",mname);
	while(!feof(read))
	{
		if( (npt)%1000000==0)
			QEPP_PRINT( "Reading line num %li...\n", npt+1);
		npt++;
		sscanf( buffer, "%li %d %d %lf %lf %lf %lf %lf", &k_app, &vb, &cb, &kpp[0], &kpp[1], &kpp[2], &e_gap, &df);
		k_app--; vb--; cb--;
		p_app=(long int)ROUND(e_gap/delta);

		if(p_app < n_pt && p_app >=0)
			if(vb < cb)
			for( int i1=0; i1<3; i1++)
				pp[k_app][p_app][i1] += kpp[i1]*df/pow(e_gap,2);
		my_getline( buffer, 256, read);
	}
	QEPP_PRINT( "...%li lines read. Done\n", npt);
	fclose( read);

	double prefact = fact * 4.0 * pow(PI,2) * pow(2*RY_TO_EV,3) * a3 / v_cell;
	if( app != NULL)
		if( app->spin_orbit)
		{
			prefact/=2;
			QEPP_PRINT( "Spinorbit calculation detected\n");
		}
	FREE( app);
//Trasform pp in eps_2[k][x->en][i/j/k]
	for( long int i=0; i<n_kpt; i++)
		for( long int i1=1; i1<n_pt; i1++)
			for( int i2=0; i2<3; i2++)
				pp[i][i1][i2]*=prefact*weight[i]/delta; 

//Recalc all for x[i] -> (x[i]+x[i-1])/2
	for( long int i=eps->n_pt-1; i>0; i--)
	{
		eps->x[i]=(eps->x[i]+eps->x[i-1])/2.;
		for( long int i1=0; i1<n_kpt; i1++ )
			for( int i2=0; i2<3; i2++)
				eps->values[i][i2]+=(pp[i1][i][i2] + pp[i1][i-1][i2])/2.;
	}

//Calc epsTOT
	for( long int i1=0; i1<n_pt; i1++ )
	{
		for( int i2=0; i2<3; i2++)
			eps->values[i1][3]+=eps->values[i1][i2];
		eps->values[i1][3]/=3.0;
	}
//Remove 0 pt
	eps->n_pt--;
	double * app1 = (double *)AllocateLinearMem1( sizeof( double), eps->n_pt);
	memcpy( app1, eps->x+1, eps->n_pt * sizeof( double));
	FreeLinearMem1( (void *)eps->x);
	eps->x=app1;

	double ** app2 = (double **)AllocateLinearMem2( sizeof(double), eps->n_pt, eps->n_col);
	for( long int i=0; i<eps->n_pt; i++)
		memcpy( app2[i], eps->values[i+1], eps->n_col * sizeof( double));
	FreeLinearMem2( (void **)eps->values);
	eps->values=app2;

//Free all unused variables
	FREE(k_pt);
	FreeLinearMem1( (void *)weight);
	FreeLinearMem3( (void ***)pp);

	*out_ptr = eps;
	QEPP_PRINT( "End eps_mod calculation\n");
	SUCCESS();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
errh * calculate_eps_mod(opt_data * data, double max_range, long int n_pt, double a3, double v_cell)
{
	if( data == NULL)
	{
		//pp_lerror( "ERROR FUNCTION calculate_eps:\tPassing NULL data pointer."); pp_perror();
		return NULL;
	}
	double delta = max_range / (double)n_pt;
	n_pt++;
//	double h_delta=delta/2;
	double fact=1;
	char filename[128]="nscf_1.out";

	if( max_range == 0 || n_pt == 0)
		return NULL;
	nscf_md * app;
	if( READ( filename, &app) != SUCCESS)
		FAIL( FAIL, "Failed to acquire nscf metadata");
	nscf_data * app22;
	if( READ( filename, &app22) != SUCCESS)
		FAIL( FAIL, "Failed to acquire nscf data");
	if(v_cell == 0)
	{
		if( app == NULL)
			return NULL;
		v_cell = app->vc;
	}
	if( a3 == 0)	//Bulk
		a3=1;
	else		//Surface
		fact=0.5;

//Read k points data from k.dat or if not available nscf_1.out or return NULL
	double tot_w=0;
	long int n_kpt;
	double * weight;
	opt_data * k_pt;
	if( READ( "k.dat", &k_pt) != SUCCESS)
	{
		nscf_data * app;
		if( READ( filename, &app) != SUCCESS)
			FAIL( FAIL, "Can't acquire k-points data");
		n_kpt=app->n_kpt;
		tot_w = app->tot_weight/2;
		weight = (double *)AllocateLinearMem1( sizeof( double), n_kpt);//malloc( n_kpt * sizeof( double));
		memcpy(weight, app->weight, app->n_kpt * sizeof( double));
		FREE(app);
	}
	else
	{
		n_kpt=k_pt->n_pt;
		weight = (double *)AllocateLinearMem1( sizeof( double), n_kpt);//malloc( n_kpt * sizeof( double));
		double app;
		for( long int i=0; i<k_pt->n_pt; i++)
		{
			app = k_pt->values[i][2];
			weight[i]=app;
			tot_w+=app;
		}
	}
	for( long int i=0; i< n_kpt; i++)
		weight[i]/=tot_w;

//Start actual calc
	//Generate result sructure
	opt_data * eps = initialize_opt_data( n_pt, 4);
	MAKE_HEADERS(eps,EPS_HEADERS);
	//Initialize x and y values of result
	for( long int i=0; i< n_pt; i++)
	{
		eps->x[i]= (double)i * delta;
		for( int i1=0; i1<eps->n_col; i1++)
			eps->values[i][i1]=0;
	}
//Read matrixelements and redistribute them in
//matrix element[k][x->en][i/j/k]]
	QEPP_PRINT( "Allocating %.2lf MB of RAM for pp...\n", n_kpt*n_pt*3.*8./1024./1024.);
	double ***pp;
	pp = (double ***)AllocateLinearMem3( sizeof( double), n_kpt, n_pt, 3);
	QEPP_PRINT( "...done\n");
// data->values[x] =?   x: 0-> v_b  1-> c_b  2/3/4-> |p_x/y/z|^2  5-> e_gap  6-> Df(occ)
	long int k_app, p_app;
	double Wpoint[2][3] = {{ 0.0080, 0.5107, 0.0000}, { 0.0197, 0.2815, 0.5916}};  //TaAs
	double e_node_v[2] = { -0.026, -0.013}; //TaAs
	double e_node = e_node_v[0];
	double e_fermi = 21.2320;//find_e_fermi( app22); // app->e_fermi;
/************************************************************************************
	scf_0.00001	scf_0.1		mingap + (e_gap - e_f calcoalto)
TaAs	21.2320		21.3457		21.211 + 0.026
TaP	22.6063		22.7478		22.571 + 0.055
NbAs	14.4866		14.5931		14.516 + 0.033
NbP	15.2998		15.5891		15.285 + 0.056
*************************************************************************************/
	for( long int i=0; i<data->n_pt; i++)
	{
		k_app=(long int)data->x[i]-1;
		p_app=(long int)ROUND(data->values[i][5]/delta);
		double k[3];
		for( int j=0; j<3; j++)
			k[j] = app22->kpt[k_app][j];
		double dk[2][3];
		double dk_tot[2] = {0};
		double app_dk = 0;
		for( int n=0; n<2; n++)
		{
			for ( int n1=0; n1<2; n1++)
			{
				if( n1 == 0)
					dk[n][n1] = 0;
				for( int n2=0; n2<2; n2++)
				{
					app_dk = fabs( fabs(k[n1]) - fabs(Wpoint[n][n2]));
					if( app_dk < dk[n][n1]) dk[n][n1] = app_dk;
				}
			}
			dk[n][2] = fabs( fabs(k[2]) - fabs(Wpoint[n][2]));
		}
		for( int n=0; n<2; n++)
		{
			for ( int n1=0; n1<3; n1++)
				dk_tot[n] += pow( dk[n][n1], 2);
			dk_tot[n] = sqrt( dk_tot[n]);
		}
		if(p_app < n_pt && p_app >=0)
			if(data->values[i][0] < data->values[i][1])
			for( int i1=0; i1<3; i1++)
			{
				long int vb = (long int) data->values[i][0]-1;
				long int cb = (long int) data->values[i][1]-1;
				if( dk_tot[1] < dk_tot[0])
					e_node = e_node_v[1];	
				double e_vb = app22->energies[k_app][ vb];
				double e_cb = app22->energies[k_app][ cb];
				//double occ = 2*(app22->occup[k_app][ vb] - app22->occup[k_app][ cb]);

			e_node = 0;
				if( e_vb < e_fermi+e_node && e_cb > e_fermi+e_node)
				pp[k_app][p_app][i1] += data->values[i][i1+2]*2/pow(data->values[i][5],2);
				//pp[k_app][p_app][i1] += 2.5E-4*2/pow(data->values[i][5],2);

				/*if( data->values[i][0] <= app->n_el && data->values[i][1] > app->n_el)
					pp[k_app][p_app][i1]+=1E-3*2/pow(data->values[i][5],2);*/
				//pp[k_app][p_app][i1]+=/*data->values[i][i1+2]*/1E-3*data->values[i][6]/pow(data->values[i][5],2);
				/*if( data->values[i][6] > 0)
					pp[k_app][p_app][i1]+=data->values[i][i1+2]*2/pow(data->values[i][5],2);*/
			}
	}

	double prefact = fact * 4.0 * pow(PI,2) * pow(2*RY_TO_EV,3) * a3 / v_cell;
	if( app != NULL)
		if( app->spin_orbit)
		{
			prefact/=2;
			QEPP_PRINT( "Spinorbit calculation detected\n");
		}
	FREE( app);
	FREE( app22);
//Trasform pp in eps_2[k][x->en][i/j/k]
	for( long int i=0; i<n_kpt; i++)
		for( long int i1=1; i1<n_pt; i1++)
			for( int i2=0; i2<3; i2++)
				pp[i][i1][i2]*=prefact*weight[i]/delta; 

//Recalc all for x[i] -> (x[i]+x[i-1])/2
	for( long int i=eps->n_pt-1; i>0; i--)
	{
		eps->x[i]=(eps->x[i]+eps->x[i-1])/2.;
		for( long int i1=0; i1<n_kpt; i1++ )
			for( int i2=0; i2<3; i2++)
				eps->values[i][i2]+=(pp[i1][i][i2] + pp[i1][i-1][i2])/2.;
	}

//Calc epsTOT
	for( long int i1=0; i1<n_pt; i1++ )
	{
		for( int i2=0; i2<3; i2++)
			eps->values[i1][3]+=eps->values[i1][i2];
		eps->values[i1][3]/=3.0;
	}
//Remove 0 pt
	eps->n_pt--;
	double * app1 = (double *)AllocateLinearMem1( sizeof( double), eps->n_pt);
	memcpy( app1, eps->x+1, eps->n_pt * sizeof( double));
	FreeLinearMem1( (void *)eps->x);
	eps->x=app1;

	double ** app2 = (double **)AllocateLinearMem2( sizeof(double), eps->n_pt, eps->n_col);
	for( long int i=0; i<eps->n_pt; i++)
		memcpy( app2[i], eps->values[i+1], eps->n_col * sizeof( double));
	FreeLinearMem2( (void **)eps->values);
	eps->values=app2;

//Free all unused variables
	FREE(k_pt);
	FreeLinearMem1( (void *)weight);
	FreeLinearMem3( (void ***)pp);
	QEPP_PRINT( "End eps_mod calculation\n");
	//return eps;
	SUCCESS();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
errh * calculate_eps_mod_huge_files( opt_data ** out_ptr, char * mname, double max_range, long int n_pt, double a3, double v_cell)
{
	if( out_ptr == NULL)
		FAIL( NULL_OUT, " ");
	*out_ptr = NULL;
	if( mname == NULL)
		FAIL( NULL_IN, " ");

	double delta = max_range / (double)n_pt;
	n_pt++;
//	double h_delta=delta/2;
	double fact=1;
	char *filename="nscf_1.out";
	if( !is_file( filename) && (v_cell == 0 || !is_file( "k.dat")))
		filename = get_file( filename, ".out");

	if( max_range == 0 || n_pt == 0)
		FAIL( FAIL, "Invalid input parameters");
	nscf_data * app;
	if( READ( filename, &app) != SUCCESS)
		FAIL( FAIL, "Failed to acquire nscf data");
	if(v_cell == 0)
	{
		if( app == NULL)
			FAIL( FAIL, "Can't acquire cell volume data");
		v_cell = app->md->vc;
	}
	if( a3 == 0)	//Bulk
		a3=1;
	else		//Surface
		fact=0.5;

//Read k points data from k.dat or if not available nscf_1.out or return NULL
	double tot_w=0;
	long int n_kpt;
	double * weight;
	opt_data * k_pt;
	if( READ( "k.dat", &k_pt) != SUCCESS)
	{
		nscf_data * app11;
		if( READ( filename, &app11) != SUCCESS)
			FAIL( FAIL, "Can't acquire k-points data");		

		n_kpt=app11->n_kpt;
		tot_w = app11->tot_weight/2;
		weight = (double *)AllocateLinearMem1( sizeof( double), n_kpt);//malloc( n_kpt * sizeof( double));
		memcpy(weight, app11->weight, app11->n_kpt * sizeof( double));
		FREE(app11);
	}
	else
	{
		n_kpt=k_pt->n_pt;
		weight = (double *)AllocateLinearMem1( sizeof( double), n_kpt);//malloc( n_kpt * sizeof( double));
		double app;
		for( long int i=0; i<k_pt->n_pt; i++)
		{
			app = k_pt->values[i][2];
			weight[i]=app;
			tot_w+=app;
		}
	}
	for( long int i=0; i< n_kpt; i++)
		weight[i]/=tot_w;


//Start actual calc
	//Generate result sructure
	FILE * read = fopen( mname , "r");
	if( read == NULL)
		FAIL( OPEN_IN_FAIL, " ");

	opt_data * eps = initialize_opt_data( n_pt, 4);
	MAKE_HEADERS(eps,EPS_HEADERS);
	//Initialize x and y values of result
	for( long int i=0; i< n_pt; i++)
	{
		eps->x[i]= (double)i * delta;
		for( int i1=0; i1<eps->n_col; i1++)
			eps->values[i][i1]=0;
	}
//Read matrixelements and redistribute them in
//matrix element[k][x->en][i/j/k]]
	QEPP_PRINT( "Allocating %.2lf MB of RAM for pp...\n", n_kpt*n_pt*3.*8./1024./1024.);
	double ***pp;
	pp = (double ***)AllocateLinearMem3( sizeof( double), n_kpt, n_pt, 3);
	QEPP_PRINT( "...done\n");

// data->values[x] =?   x: 0-> v_b  1-> c_b  2/3/4-> |p_x/y/z|^2  5-> e_gap  6-> Df(occ)
	long int k_app, p_app;
	double e_gap=0;
	int vb=0, cb=0;
	double kpp[3];
	double df=0;
	
	int n_symmetry=2;
	double symmetry[2][3][3]={
		{{0,-1,0},{1,0,0},{0,0,1}},			//ccw 90 degree rotation on z-axis
		{{1,0,0},{0,1,0},{0,0,-1}},			//reflection xy z -> -z
	};
	double comp_point[4][2][3]={
		{//TaAs
			{0.007800,0.510300,0.000000},
			{0.019800,0.281800,0.174200},
		},

		{//TaP
			{0.007700,0.517400,0.000000},
			{0.016100,0.274100,0.173700},
		},

		{//NbAs
			{0.002600,0.485900,0.000000},
			{0.006400,0.279000,0.169400},
		},

		{//NbP
			{0.002839,0.491752,0.000000}, //   0.002900,0.492100,0.000000
			{0.004900,0.271000,0.169200}, //   0.004700,0.271000,0.169200
		},
	};
	int symm_ord[n_symmetry];
	double * app_vect=malloc( 3 * sizeof(double));
	double *** app_matrix=malloc( n_symmetry * sizeof(double *));
	for( int i=0; i<n_symmetry; i++)
	{
		app_matrix[i]=malloc( 3 * sizeof(double *));
		for( int j=0; j<3; j++)
		{
			app_matrix[i][j]=malloc( 3 * sizeof(double));
			for(int k=0; k<3; k++)
				app_matrix[i][j][k]=symmetry[i][j][k];
		}
		symm_ord[i]=symm_order(app_matrix[i]);
	}
	int maxsymm=1;
	for( int n=0; n<n_symmetry; n++)
		maxsymm*=symm_ord[n];
	double tocomp[2][maxsymm][3];

	int material=0;
	if( strstr( app->md->formula, "TaAs") != NULL)		material=0;
	else if( strstr( app->md->formula, "TaP") != NULL)	material=1;
	else if( strstr( app->md->formula, "NbAs") != NULL)	material=2;
	else if( strstr( app->md->formula, "NbP") != NULL)	material=3;
	else
	{
		WARN( "\nFormula %s not recognized\n",app->md->formula);
	}
	QEPP_PRINT( "Material number %d recognized...\n", material);

	for( int n=0; n<2; n++)
	{
		for(int i=0; i<3; i++)
			app_vect[i]=comp_point[material][n][i];
		for(int i1=0; i1<symm_ord[0]; i1++)
		{
			for( int i2=0; i2<symm_ord[1]; i2++)
			{
				memcpy( tocomp[n][i2+i1*symm_ord[1]], app_vect, 3*sizeof(double));
				matrix_vector(app_matrix[1],app_vect,app_vect);
			}
			matrix_vector(app_matrix[0],app_vect,app_vect);
		}
	} /*
	QEPP_PRINT( "\n");
	for( int n1=0;n1<2; n1++)
		for( int n=0; n<maxsymm; n++)
			QEPP_PRINT( "Point n %d-%2d: %g %g %g\n", n1,n, tocomp[n1][n][0], tocomp[n1][n][1], tocomp[n1][n][2]);// */
	QEPP_PRINT( "\n");

	double e_node = 0;
	/*
	double e_fermi_v[4][3]= {	//For 0xx_0.06_new_plot
		{21.2328,21.3457,21.211 + 0.026},
		{22.6001,22.7478,22.571 + 0.055},
		{14.4856,14.5931,14.516 + 0.033},
		{15.2998,15.5891,15.285 + 0.056}    //15.3561
	};// */
	/*
	double e_fermi_v[4][1]= {	//For xxx_60.60.20_plot
		{21.2325-21.2286+21.2448},
		{22.5953-22.5929+22.5992},
		{14.4866-14.4822+14.4926},
		{15.2998-15.2961+15.3026}    //15.3561
	};// */
	//*
	double e_fermi_v[4][1]= {	//For xxx_60.60.20_plot
		{21.2665},
		{22.623},
		{14.5504},
		{15.3411}    //15.3561
	};// */
	//double e_fermi = 15.2998;//find_e_fermi( app); // app->md->e_fermi;
	int type=0;
	double e_fermi = e_fermi_v[material][type];
	//e_fermi = app->md->e_fermi;
	QEPP_PRINT( "E_f set to: %g\n", e_fermi);
/************************************************************************************
	scf_0.00001	scf_0.1		mingap + (e_gap - e_f calcoalto)
TaAs	21.2320		21.3457		21.211 + 0.026 = 21.237
TaP	22.6063		22.7478		22.571 + 0.055 = 22.626
NbAs	14.4866		14.5931		14.516 + 0.033 = 14.549
NbP	15.2998		15.5891		15.285 + 0.056 = 15.341
*************************************************************************************/

	long int npt=0;
	skip_comments( read, "#");
	char buffer[256];
	my_getline( buffer, 256, read);
	/*do {
		my_getline( buffer, 256, read);
		//QEPP_PRINT( "%s\n",buffer);
	} while( !feof( read) && buffer[0] == '#');*/

#define outname_num 7
	
	/*
	long int k_app_prev=-1;
	double min_dk=0.06;
		double e_node_v[4][2]={
		{-0.026,-0.013},
		{-0.055,0.021},
		{-0.033,0.004},
		{-0.056,0.026},
	};
	*/
	/*
	int check[2]={0};
	int vb_app=app->md->n_el-1, cb_app=app->md->n_el;
	char *outname_list[outname_num]={"W1", "W2", "NO_Wa", "NO_Wb", "NO_Wc", "NO_Wd", "NO_We"};
	FILE * W[outname_num]={0};
	if( strcmp( mname, "matrixelements") == 0)
	{
		char buff[256];
		for( int i=0; i<outname_num; i++)
		{
			sprintf( buff, "%s_%.2lf.dat", outname_list[i], min_dk);
			W[i] = fopen( buff, "w");
			QEPP_OUT( W[i], "#k_app\t   vb   cb            pp_x            pp_y            pp_z      e_gap       df\n");
		}
	}
	*/

	long int gap_min=0, p_min=0;
	double gap_min_d=10000;
	while(!feof(read))
	{
		npt++;
		if( npt%1000000==0 || npt==0)
			QEPP_PRINT( "Reading line num %li...\n", npt);
		sscanf( buffer, "%li %d %d %lf %lf %lf %lf %lf", &k_app, &vb, &cb, &kpp[0], &kpp[1], &kpp[2], &e_gap, &df);

		k_app--; vb--; cb--;
		p_app=(long int)ROUND(e_gap/delta);
		//QEPP_PRINT( "%li %li %d %d %g %g %g %g\n",k_app,p_app,vb,cb,kpp[0],kpp[1],kpp[2],e_gap);

		if(p_app < n_pt && p_app >=0)
		//If Transition energy is positive
		if(vb < cb)
		{
			//Check closest Weyl node
			/*if( k_app_prev != k_app)
			{
				double app_dk;
				for( int n=0; n<2; n++)
				{
					check[n]=0;
					for( int n1=0; n1<maxsymm; n1++)
					{
						app_dk = delta_k( app->kpt[k_app], tocomp[n][n1]);
						if( app_dk < min_dk)
						{
							check[n]=1;
							break;
						}
					}
					e_node=e_node_v[material][n];
				}
			}
			k_app_prev = k_app;*/
			e_node = 0;

			double e_vb = app->energies[k_app][ vb];
			double e_cb = app->energies[k_app][ cb];

			//Condition for transition at 0K
			if( e_vb < e_fermi+e_node && e_cb > e_fermi+e_node)
			{
				if( e_gap < gap_min_d)
				{
					gap_min_d=e_gap;
					gap_min=k_app;
					p_min = npt;
				}
				/*
				if( W[0] != NULL)
				{
					//if(!check[0] && !check[1])
					//	QEPP_PRINT( "Transition not near Weyl at kpt #%li %d -> %d\n",k_app+1,vb+1,cb+1);
					if( check[0])
						QEPP_OUT( W[0], "%li\t%5d%5d%16.8lf%16.8lf%16.8lf%11.5lf%9.5lf\n",
							k_app+1,vb+1,cb+1, kpp[0], kpp[1], kpp[2], e_gap, df);
					else if( check[1])
						QEPP_OUT( W[1], "%li\t%5d%5d%16.8lf%16.8lf%16.8lf%11.5lf%9.5lf\n",
							k_app+1,vb+1,cb+1, kpp[0], kpp[1], kpp[2], e_gap, df);
					else if( vb==vb_app && cb==cb_app)
						QEPP_OUT( W[2], "%li\t%5d%5d%16.8lf%16.8lf%16.8lf%11.5lf%9.5lf\n",
						k_app+1,vb+1,cb+1, kpp[0], kpp[1], kpp[2], e_gap, df);
					else if( vb==vb_app-1 && cb==cb_app)
						QEPP_OUT( W[3], "%li\t%5d%5d%16.8lf%16.8lf%16.8lf%11.5lf%9.5lf\n",
						k_app+1,vb+1,cb+1, kpp[0], kpp[1], kpp[2], e_gap, df);
					else if( vb==vb_app && cb==cb_app+1)
						QEPP_OUT( W[4], "%li\t%5d%5d%16.8lf%16.8lf%16.8lf%11.5lf%9.5lf\n",
						k_app+1,vb+1,cb+1, kpp[0], kpp[1], kpp[2], e_gap, df);
					else if( vb==vb_app-2 && cb==cb_app)
						QEPP_OUT( W[5], "%li\t%5d%5d%16.8lf%16.8lf%16.8lf%11.5lf%9.5lf\n",
						k_app+1,vb+1,cb+1, kpp[0], kpp[1], kpp[2], e_gap, df);
					else if( vb==vb_app && cb==cb_app+2)
						QEPP_OUT( W[6], "%li\t%5d%5d%16.8lf%16.8lf%16.8lf%11.5lf%9.5lf\n",
						k_app+1,vb+1,cb+1, kpp[0], kpp[1], kpp[2], e_gap, df);
				}// */

				//e_gap = e_cb - e_vb;
				//Run along x, y, z
				for( int i1=0; i1<3; i1++)
					pp[k_app][p_app][i1] += kpp[i1]*2/pow(e_gap,2);
			}
		}
		my_getline( buffer, 256, read);
	}
	QEPP_PRINT( "Read %li lines from %s\n", npt, mname);
	QEPP_PRINT( "\t%g min gap at point number %li from line %li of file %s\n", gap_min_d, gap_min+1, p_min, mname);
	double app_v[3];
	app_v[0] = app->kpt[gap_min][1];
	app_v[1] = app->kpt[gap_min][0];
	app_v[2] = app->kpt[gap_min][2];
	QEPP_PRINT( "delta_K:\n  W1:  ");
	for( int i=0; i<3; i++)
		QEPP_PRINT( "%lf  ", fabs(app->kpt[gap_min][i]) - fabs(comp_point[material][0][i]));
	QEPP_PRINT( "    or    ");
	for( int i=0; i<3; i++)
		QEPP_PRINT( "%lf  ", fabs(app_v[i]) - fabs(comp_point[material][0][i]));
	QEPP_PRINT( "\n  W2:  ");
	for( int i=0; i<3; i++)
		QEPP_PRINT( "%lf  ", fabs(app->kpt[gap_min][i]) - fabs(comp_point[material][1][i]));
	QEPP_PRINT( "    or    ");
	for( int i=0; i<3; i++)
		QEPP_PRINT( "%lf  ", fabs(app_v[i]) - fabs(comp_point[material][1][i]));
	QEPP_PRINT( "\n");
	PRINT_KPT( app, gap_min);
	fclose( read);
	/*if( W[0] != NULL)
		for( int i=0; i<outname_num; i++)
			fclose( W[i]);*/
	for( int i=0; i<n_symmetry; i++)
	{
		for( int j=0; j<3; j++)
			free(app_matrix[i][j]);
		free(app_matrix[i]);
	}
	free(app_matrix);
	free(app_vect);
#undef outname

	double prefact = fact * 4.0 * pow(PI,2) * pow(2*RY_TO_EV,3) * a3 / v_cell;
	if( app != NULL)
		if( app->md->spin_orbit)
		{
			prefact/=2.;
			WARN( "Spinorbit calculation detected");
		}
	FREE( app);

//Trasform pp in eps_2[k][x->en][i/j/k]
	for( long int i=0; i<n_kpt; i++)
		for( long int i1=1; i1<n_pt; i1++)
			for( int i2=0; i2<3; i2++)
				pp[i][i1][i2]*=prefact*weight[i]/delta; 

//Recalc all for x[i] -> (x[i]+x[i-1])/2
	for( long int i=eps->n_pt-1; i>0; i--)
	{
		eps->x[i]=(eps->x[i]+eps->x[i-1])/2.;
		for( long int i1=0; i1<n_kpt; i1++ )
			for( int i2=0; i2<3; i2++)
				eps->values[i][i2]+=(pp[i1][i][i2] + pp[i1][i-1][i2])/2.;
	}

//Calc epsTOT
	for( long int i1=0; i1<n_pt; i1++ )
	{
		for( int i2=0; i2<3; i2++)
			eps->values[i1][3]+=eps->values[i1][i2];
		eps->values[i1][3]/=3.0;
	}
//Remove 0 pt
	eps->n_pt--;
	//double * app1 = malloc( eps->n_pt * sizeof( double));
	double * app1 = (double *)AllocateLinearMem1( sizeof( double), eps->n_pt);
	memcpy( app1, eps->x+1, eps->n_pt * sizeof( double));
	//free(eps->x);
	FreeLinearMem1( (void *)eps->x);
	eps->x=app1;
/*
	//double ** app2 = malloc( eps->n_pt * sizeof( double *));
	double ** app2 = (double **)AllocateLinearMem1( sizeof(double *), eps->n_pt);
	memcpy( app2, eps->values+1, eps->n_pt * sizeof( double *));
	free(eps->values[0]);
	free(eps->values);
	eps->values=app2;
*/
	double ** app2 = (double **)AllocateLinearMem2( sizeof(double), eps->n_pt, eps->n_col);
	for( long int i=0; i<eps->n_pt; i++)
		memcpy( app2[i], eps->values[i+1], eps->n_col * sizeof( double));
	FreeLinearMem2( (void **)eps->values);
	eps->values=app2;

//Free all unused variables
	FREE(k_pt);
	FreeLinearMem1( (void *)weight);
	FreeLinearMem3( (void ***)pp);

	*out_ptr = eps;
	QEPP_PRINT( "End eps_mod calculation\n");
	SUCCESS();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Apply Kramers_Kronig to eps_2 and calculate eps_1 and EELS
//Principal value using gsl
/*double apply_kk_f( double x, void * p)
{
	double coeff = 1.;
	if( x<0)
		coeff = -1.;

	double ** v = (double **)p;

	long int n_pt = v[1][0];

	double start = v[0][0];
	double de = v[0][1] - v[0][0];

	long int n = floor( (x - start + de/2) / de);
//QEPP_PRINT( "NPT: %li, start: %lf, de: %lf, omega: %lf, n: %li\n", n_pt, start, de, x, n);
	if( n<0)
		n*=-1;
	if( n < n_pt)
		return coeff * v[1][n+1];
	else
		return 0;
}

opt_data * apply_kk(opt_data * data)
{
	if( data == NULL)
	{
		pp_lerror( "ERROR FUNCTION apply_kk:\tPassing NULL data pointer"); pp_perror();
		return NULL;
	}

	long int n_pt = data->n_pt;
	opt_data * new_s = initialize_opt_data( n_pt, 4);
	//MAKE_HEADERS(new_s,EELS_HEADERS);

	double PV, err;
	gsl_function F;
	F.function = &apply_kk_f;
	double ** par = malloc( 2 * sizeof( double *));
	double * app = malloc( (n_pt+1) * sizeof( double));
	double omax = data->x[n_pt-1];
	app[0] = n_pt;
	par[0] = data->x;
	
	//F.params = NULL;
	gsl_integration_workspace * ws = gsl_integration_workspace_alloc( 100000);

	double omega=0;
	for( long int k=0; k<n_pt; k++)
	{

		omega=data->x[k];
		new_s->x[k] = omega;
		for( int i=0; i<4; i++)
		{
			for( long int j=0; j<n_pt; j++)
				app[j+1] = data->values[j][i];
			par[1] = app;
			F.params = (void *)par;
			gsl_integration_qawc( &F, -omax*5, omax*5, omega, 0, .0001, 20000, ws, &PV, &err);
			//QEPP_PRINT( "%lf: %lf -> %lf\n",-omega, F.function(-omega, par), PV);
			//QEPP_PRINT( "err: %lf\n", err);
			new_s->values[k][i] = PV/M_PI + 1;
//getchar();
		}
	}

	gsl_integration_workspace_free( ws);
	free( app);
	free( par);

	//new_s = apply_broadening( new_s, omax/3);
	

	return new_s;
}*/


//Mathematical_Notes,Miskolc,Vol1,N1,2000,pp.35-41
/*opt_data * apply_kk(opt_data * data)
{
	if( data == NULL)
	{
		pp_lerror( "ERROR FUNCTION apply_kk:\tPassing NULL data pointer"); pp_perror();
		return NULL;
	}

	long int n_pt = data->n_pt;
	int n_col = data->n_col;
	opt_data * new_s = initialize_opt_data( n_pt, n_col);
	//MAKE_HEADERS(new_s,EELS_HEADERS);

	double start = data->x[0];
	double de = data->x[1] - start;
	double omega;
	double PV=0;

	for( long int k=0; k<n_pt; k++)
	{
		omega = data->x[k];
		new_s->x[k] = omega;
		long int n = (omega-start)/de;
		for( int j=0; j<n_col; j++)
		{
			long int n1=0, n2=0;
			double c1 = 1;
			PV=0;
			for( long int i=0; i<n_pt; i++)
			{
				if( i != k)
				{
					n1 = n - i;
					n2 = n + i;
					if( n1 < 0)
					{
						n1 *= -1;
						c1 = -1;
					}
					else
					//{
						c1 = 1;
					if( n1 < n_pt)
						PV += data->values[n1][j] / (-1 * data->x[n1]) * de * c1;
					if( n2 < n_pt)
						PV += data->values[n2][j] / ( 1 * data->x[n2]) * de;
					//}
				}
				else
				{
					if( i > 0)
					{
						if( i < n_pt-1)
							PV += (data->values[i+1][j] - data->values[i-1][j]);
						else
							PV += (data->values[n_pt-1][j] - data->values[n_pt-2][j]);
					}
					else
						PV += (data->values[1][j] - data->values[0][j]);
				}
			}
			new_s->values[k][j] = PV / PI + 1;
		}
	}
	

	return new_s;
}*/

//Script clog
opt_data * apply_kk(opt_data * data)
{
	if( data == NULL)
	{
		//pp_lerror( "ERROR FUNCTION apply_kk:\tPassing NULL data pointer"); pp_perror();
		return NULL;
	}

	long int n_pt = data->n_pt;
	int n_col = data->n_col;
	opt_data * new_s = initialize_opt_data( n_pt, n_col);
	//MAKE_HEADERS(new_s,EELS_HEADERS);

	double x=0, E=0, EV=0;
	double complex PV, B, C, D, F, H;
	double complex omega=0;

	QEPP_PRINT( "Starting Kramers_Kronig calculation for eps1...\n");
	for( long int k=0; k<n_pt; k++) //Calculate real part starting from Im part for each x
	{
		x = data->x[k];
		new_s->x[k] = x;
		if( k%5000 == 0)
			QEPP_PRINT( "Reached point num %li/%li\n...",k+1,n_pt);
		
		for( int j=0; j<n_col; j++) //Apply to every data column of the struct
		{
			PV = 0;
			for( int a=1; a>-2; a-=2) //2 sums for resonant and antiresonant part
			{
				omega = a * (x + I * 0.05); //Add small immaginary part to avoid singularity on real axes

				F = omega;
				EV = data->x[0];
				B = clog( 1. - EV/F);
				double xi=0, xi1=0;
				for( long int i=0; i<n_pt-1; i++)
				{
					xi  = data->x[i];
					xi1 =  data->x[i+1];
					E = xi1 - xi;
					D = xi1 - omega;
					H = omega - xi;
					C = clog( 1. - E/H);
					PV += data->values[i][j] * (C*D/E + B*F/EV);
					B=C; F=H; EV=E;
				}
				PV += data->values[n_pt-1][j] * (1. + B*F/EV);
			}
			PV /= PI;
			PV += 1;
			new_s->values[k][j] = creal( PV);
		}
	}
	QEPP_PRINT( "...done\n");

	return new_s;
}

opt_data * apply_kk2(opt_data * data)
{
	if( data == NULL)
	{
		//pp_lerror( "ERROR FUNCTION apply_kk:\tPassing NULL data pointer"); pp_perror();
		return NULL;
	}

	long int n_pt = data->n_pt;
	int n_col = data->n_col;
	opt_data * new_s = initialize_opt_data( n_pt, n_col);
	//MAKE_HEADERS(new_s,EELS_HEADERS);

	double x=0, E=0, EV=0;
	double complex PV, B, C, D, F, H;
	double complex omega=0;

	QEPP_PRINT( "Starting Kramers_Kronig calculation for eps1...\n");
	for( long int k=0; k<n_pt; k++) //Calculate real part starting from Im part for each x
	{
		x = data->x[k];
		new_s->x[k] = x;
		if( k%5000 == 0)
			QEPP_PRINT( "Reached point num %li/%li\n...",k+1,n_pt);
		
		for( int j=0; j<n_col; j++) //Apply to every data column of the struct
		{
			PV = 0;
			for( int a=1; a>-2; a-=2) //2 sums for resonant and antiresonant part
			{
				omega = 1 * (x + I * 0.05); //Add small immaginary part to avoid singularity on real axes

				F = omega;
				EV = data->x[0];
				B = clog( 1. - EV/F);
				double xi=0, xi1=0;
				for( long int i=0; i<n_pt-1; i++)
				{
					xi  = data->x[i];
					xi1 =  data->x[i+1];
					E = xi1 - xi;
					D = xi1 - omega;
					H = omega - xi;
					C = clog( 1. - E/H);
					PV += data->values[i][j] * (C*D/E + B*F/EV);
					B=C; F=H; EV=E;
				}
				PV += data->values[n_pt-1][j] * (1. + B*F/EV);
			}
			PV /= (PI*-1);
			new_s->values[k][j] = creal( PV);
		}
	}
	QEPP_PRINT( "...done\n");

	return new_s;
}

opt_data * apply_kk_im(opt_data * data)
{
	if( data == NULL)
	{
		//pp_lerror( "ERROR FUNCTION apply_kk_im:\tPassing NULL data pointer"); pp_perror();
		return NULL;
	}

	long int n_pt = data->n_pt;
	int n_col = data->n_col;
	opt_data * new_s = initialize_opt_data( n_pt, n_col);
	//MAKE_HEADERS(new_s,EELS_HEADERS);

	double x=0;
	double complex PV, x1, x2;
	double complex GAMMA = I * 0.0001;
	double complex omega = 0;

	QEPP_PRINT( "Starting Kramers_Kronig calculation for real part starting from Im...\n");
	for( long int k=0; k<n_pt; k++)
	{
		x = data->x[k];
		new_s->x[k] = x;
		if( k%5000 == 0)
			QEPP_PRINT( "Reached point num %li/%li\n...",k+1,n_pt);
		omega = x + GAMMA * x/fabs(x);
		for( int j=0; j<n_col; j++)
		{
			PV = 0;
			x1 = data->x[0];
			for( long int k1=0; k1<n_pt; k1++)
			{
				x2 = data->x[k1];
				PV += data->values[k1][j] / (x2 - omega) * (x2 - x1);
				x1 = x2;
			}
			new_s->values[k][j] = creal( PV) / PI;
		}
	}

	QEPP_PRINT( "...done\n");

	return new_s;
}

opt_data * apply_kk_im2(opt_data * data)
{
	if( data == NULL)
	{
		//pp_lerror( "ERROR FUNCTION apply_kk_im:\tPassing NULL data pointer"); pp_perror();
		return NULL;
	}

	long int n_pt = data->n_pt;
	int n_col = data->n_col;
	opt_data * new_s = initialize_opt_data( n_pt, n_col);
	//MAKE_HEADERS(new_s,EELS_HEADERS);

	double x=0;
	double complex PV, x1, x2;
	double complex GAMMA = I * 0.0001;
	double complex omega = 0;

	QEPP_PRINT( "Starting Kramers_Kronig calculation for real part starting from Im...\n");
	for( long int k=0; k<n_pt; k++)
	{
		x = data->x[k];
		new_s->x[k] = x;
		if( k%5000 == 0)
			QEPP_PRINT( "Reached point num %li/%li\n...",k+1,n_pt);
		omega = x + GAMMA * x/fabs(x);
		for( int j=0; j<n_col; j++)
		{
			PV = 0;
			x1 = data->x[0];
			for( long int k1=0; k1<n_pt; k1++)
			{
				x2 = data->x[k1];
				PV += x2 * data->values[k1][j] / (cpow(x2,2) - cpow(omega,2)) * (x2 - x1);
				x1 = x2;
			}
			new_s->values[k][j] = creal( PV) / PI;
		}
	}

	QEPP_PRINT( "...done\n");

	return new_s;
}

opt_data * apply_kk_real(opt_data * data)
{
	if( data == NULL)
	{
		//pp_lerror( "ERROR FUNCTION apply_kk_real:\tPassing NULL data pointer"); pp_perror();
		return NULL;
	}

	long int n_pt = data->n_pt;
	int n_col = data->n_col;
	opt_data * new_s = initialize_opt_data( n_pt, n_col);
	//MAKE_HEADERS(new_s,EELS_HEADERS);

	double x=0;
	double complex PV, x1, x2;
	double complex GAMMA = I * 0.0001;
	double complex omega = 0;

	QEPP_PRINT( "Starting Kramers_Kronig calculation for Imaginary part starting from Re...\n");
	QEPP_PRINT( "(n_pt=%li   n_col=%d)\n",n_pt,n_col);
	for( long int k=0; k<n_pt; k++)
	{
		x = data->x[k];
		new_s->x[k] = x;
		if( k%5000 == 0)
			QEPP_PRINT( "Reached point num %li/%li\n...",k+1,n_pt);
		omega = x + GAMMA;
		for( int j=0; j<n_col; j++)
		{
			PV = 0;
			x1 = data->x[0];
			for( long int k1=0; k1<n_pt; k1++)
			{
				x2 = data->x[k1];
				PV += data->values[k1][j] / (x2 - omega) * (x2 - x1);
				x1 = x2;
			}
			new_s->values[k][j] = creal( PV) / (PI * (-1));
		}
	}

	QEPP_PRINT( "...done\n");

	return new_s;
}

//*
opt_data * apply_kk_real2(opt_data * data)
{
	if( data == NULL)
	{
		//pp_lerror( "ERROR FUNCTION apply_kk_real:\tPassing NULL data pointer"); pp_perror();
		return NULL;
	}

	long int n_pt = data->n_pt;
	int n_col = data->n_col;
	opt_data * new_s = initialize_opt_data( n_pt, n_col);
	//MAKE_HEADERS(new_s,EELS_HEADERS);

	double x=0;
	double complex PV, x1, x2;
	double complex GAMMA = I * 0.0001;
	double complex omega = 0;

	QEPP_PRINT( "Starting Kramers_Kronig calculation for Imaginary part starting from Re...\n");
	for( long int k=0; k<n_pt; k++)
	{
		x = data->x[k];
		new_s->x[k] = x;
		if( k%1000 == 0)
			QEPP_PRINT( "Reached point num %li/%li\n...",k+1,n_pt);
		if( x >= 0)
		{
			omega = x + GAMMA;
			for( int j=0; j<n_col; j++)
			{
				PV = 0;
				x1 = data->x[0];
				for( long int k1=0; k1<n_pt; k1++)
				{
					x2 = data->x[k1];
					PV += data->values[k1][j] / (cpow(x2,2) - cpow(omega,2)) * (x2 - x1);
					x1 = x2;
				}
				new_s->values[k][j] = creal( PV) * 1. * x / (PI * (-1));
			}
		}
	}

	QEPP_PRINT( "...done\n");

	return new_s;
}
// */

errh * generate_chebychev_zeroes( opt_data * data, int n_ref, double *out_ptr[2])
{
	if( out_ptr == NULL)
		FAIL( NULL_OUT, " ");
	if( data == NULL)
		FAIL( NULL_IN, " ");
	if( n_ref <= 0)
		FAIL( FAIL, "Negative number of points not allowed");

	double w_max = data->x[ data->n_pt-1];
	double w_min = data->x[0];

	for( int i=0; i<n_ref; i++)
	{
		double a = sqrt( ( (pow(w_max,2)-pow(w_min,2)) * cos((2*i+1)*PI/(2*n_ref)) + pow(w_max,2)+pow(w_min,2)) / 2);
		out_ptr[0][i] = data->x[data->n_pt-1];
		out_ptr[1][i] = data->values[data->n_pt-1][0];
		for( long int j=0; j<data->n_pt; j++)
			if( data->x[j] > a)
			{
				out_ptr[0][i] = data->x[j];
				out_ptr[1][i] = data->values[j][0];
				break;
			}
	}
	
	SUCCESS();
}

errh * apply_MSKK( opt_data ** out_ptr, opt_data * data, int mode, int n_ref, double * ref[2])
{
	if( out_ptr == NULL)
		FAIL( NULL_OUT, " ");
	*out_ptr = NULL;
	if( data == NULL)
		FAIL( NULL_IN, " ");
	QEPP_PRINT( "Starting Multi-Subtractive-KK calculaton.\n");

	long int n_pt = data->n_pt;
	int n_col = data->n_col;
	opt_data * new_s = NULL;
#ifdef __MPI
	if( mpi->world_rank == 0)
#endif
		new_s = initialize_opt_data( n_pt, n_col);

	double x=0;
	double PV=0;

	QEPP_PRINT( "Starting Kramers_Kronig calculation for Imaginary part starting from Re...\n");

	long int low_lim, high_lim;
#ifdef __MPI
	long int n_div = ceil( n_pt / mpi->world_size);
	low_lim = mpi->world_rank * n_div-1;
	high_lim = (mpi->world_rank+1) * n_div;

	if( low_lim < 0)
		low_lim = 0;
	if( high_lim > n_pt)
		high_lim = n_pt;
	MPI_Barrier( MPI_COMM_WORLD);
	//char buff[256]; sprintf( buff, "Process %d calculating from %li to %li\n", mpi->world_rank, low_lim+1, high_lim);
	QEPP_PRINT( "Process %d calculating from %li to %li\n", mpi->world_rank, low_lim+1, high_lim);
	MPI_Barrier( MPI_COMM_WORLD);
#else
	low_lim=0;
	high_lim=n_pt;
#endif

	for( long int i=0; i<n_pt; i++)
	{
		x = data->x[i];
#ifdef __MPI
		if( mpi->world_rank == 0)
#endif
			new_s->x[i] = x;
		double xsq2 = pow( x, 2);

		if( i%1000 == 0)
		{
			//char buff[256]; sprintf( buff, "Reached point num %li/%li...",i+1,n_pt);
			QEPP_PRINT( "Reached point num %li/%li...", i+1,n_pt);
		}
		if( x >= 0)
		{
			for( int j=0; j<n_col; j++) //For each column...
			{
				//Calculate prefactor (Lagrange interpolation polynomial)
				double pref = 0;
#ifdef __MPI
				if( mpi->world_rank == 0)
				{
#endif
					double p_app;
					for( int k=0; k<n_ref; k++)
					{
						double wref = ref[0][k];
						double wref2 = pow( wref, 2);
						p_app = ref[1][k];
						if( mode == 0)
							p_app /= wref;
						if( p_app != 0)
						{
							double wn, wn2;
							for( int k1=0; k1<k; k1++)
							{
								wn = ref[0][k1];
								wn2 = pow( wn,2);
								p_app *= (xsq2 - wn2) / ( wref2 - wn2);
							}
							for( int k1=k+1; k1<n_ref; k1++)
							{
								wn = ref[0][k1];
								wn2 = pow( wn,2);
								p_app *= (xsq2 - wn2) / ( wref2 - wn2);
							}
						}
						pref += p_app;
					}
#ifdef __MPI
				}
#endif
				//Calculate Principal value
				double complex PV_c = 0;
				double complex x1, x2, x2sq;
				double complex GAMMA = I * 0.005;
				x1 = data->x[low_lim];
				double complex app;
				for( long int k1=low_lim+1; k1<high_lim; k1++)
				{
					x2 = data->x[k1];
					x2sq = cpow( x2 + GAMMA, 2) ;
					app = data->values[k1-1][j] / (x2sq - xsq2) * (x2 - x1);
					if( mode == 1)
						app *= x2;
					for( int k=0; k<n_ref; k++)
						app /= ( x2sq - cpow( ref[0][k], 2));
					PV_c += app;
					x1 = x2;
				}
				PV = creal( PV_c);
				mp_sum( &PV, MPI_COMM_WORLD, 1);


				//Calculate PV prefact
#ifdef __MPI
				if( mpi->world_rank == 0)
				{
#endif
					double PV_pref = 1;
					double wref;
					for( int k=0; k<n_ref; k++)
					{
						wref = ref[0][k];
						if( x == wref)
						{
							PV_pref = 0;
							break;
						}
						PV_pref *= (xsq2 - pow( wref, 2));
					}
					if( mode == 0)
						new_s->values[i][j] = x * ( pref - 1./PI * PV_pref * PV );
					else
						new_s->values[i][j] = ( pref + 1./PI * PV_pref * PV );
#ifdef __MPI
				}
#endif
			}
		}
	}

	QEPP_PRINT( "...done\n");

	*out_ptr = new_s;
	SUCCESS();
}

#ifdef __GSLa
double f ( double x, void * params)
{
	void ** p = (void **)params;
	int ** p_int = (int **)p;
	double ** p_double = (double **)p;
	long int x_i = x;

	int n_ref = *p_int[0];
	double * n = p_double[1];
	double * x_v = p_double[2];
	double * y = p_double[3];
	double x_calc2 = *p_double[4];

	double x2 = pow( x_v[x_i], 2);
	double denom = (x2 - x_calc2);

	for( int i=0; i<n_ref; i++)
		denom *= (x2 - pow(n[i], 2));
	if( denom == 0)
		return 0;
	return y[x_i] / denom;

}

opt_data * apply_MSKK_real(opt_data * data, int n_ref, double * ref[2])
{
	if( data == NULL)
	{
		pp_lerror( "ERROR FUNCTION apply_kk_real:\tPassing NULL data pointer"); pp_perror();
		return NULL;
	}
	QEPP_PRINT( "Starting Multi-Subtractive-KK calculaton.\n");

	long int n_pt = data->n_pt;
	int n_col = data->n_col;
	opt_data * new_s = initialize_opt_data( n_pt, n_col);

	double x=0;
	double PV=0;

	QEPP_PRINT( "Starting Kramers_Kronig calculation for Imaginary part starting from Re...\n");

	double * app_y[n_col];
	gsl_integration_cquad_workspace * wsp = gsl_integration_cquad_workspace_alloc( 1000);
	for( int j=0; j<n_col; j++)
		app_y[j] = (double *)AllocateLinearMem1( sizeof( double), n_pt);//malloc( n_pt * sizeof( double));
	for( long int i=0; i<n_pt; i++)
		for( int j=0; j<n_col; j++)
			app_y[j][i] = data->values[i][j];
	double dx_avg = (data->x[n_pt-1] - data->x[0])/(n_pt-1);
	double app_err;
	double abserr=0;
	size_t app_eval;
	size_t n_eval=0;

	for( long int i=0; i<n_pt; i++)
	{
		x = data->x[i];
		new_s->x[i] = x;
		double xsq2 = pow( x, 2);
		if( i%1000 == 0)
			QEPP_PRINT( "Reached point num %li/%li\n...",i+1,n_pt);
		if( x >= 0)
		{
			for( int j=0; j<n_col; j++) //For each column...
			{
				//Calculate prefactor (Lagrange interpolation polynomial)
				double pref = 0;
				double p_app;
				for( int k=0; k<n_ref; k++)
				{
					double wref = ref[0][k];
					double wref2 = pow( wref, 2);
					p_app = ref[1][k] / wref;
					if( p_app != 0)
					{
						double wn, wn2;
						for( int k1=0; k1<k; k1++)
						{
							wn = ref[0][k1];
							wn2 = pow( wn,2);
							p_app *= (xsq2 - wn2) / ( wref2 - wn2);
						}
						for( int k1=k+1; k1<n_ref; k1++)
						{
							wn = ref[0][k1];
							wn2 = pow( wn,2);
							p_app *= (xsq2 - wn2) / ( wref2 - wn2);
						}
					}

					pref += p_app;
				}
				//Calculate Principal value
				int n_ref_pass = n_ref;
				void ** params = calloc( 6, sizeof( void *));
				params[0] = &n_ref_pass;
				params[1] = ref[0];
				params[2] = data->x;
				params[3] = app_y[j];
				params[4] = &xsq2;
				params[5] = &dx_avg;

				gsl_function F;
				F.function = &f;
				F.params = params;

				gsl_integration_cquad( &F, 0, n_pt-1, 0, 1.E-1, wsp, &PV, &app_err, &app_eval);
				abserr += app_err;
				n_eval += app_eval;
				PV *= dx_avg;

				//Calculate PV prefact
				double PV_pref = 1;
				double wref;
				for( int k=0; k<n_ref; k++)
				{
					wref = ref[0][k];
					if( x == wref)
					{
						PV_pref = 0;
						break;
					}
					PV_pref *= (xsq2 - pow( wref, 2));
				}
				new_s->values[i][j] = x * ( pref - 1./PI * PV_pref * PV );
			}
		}
	}

	QEPP_PRINT( "Integration concluded with %li integration calls.\n", (long int)n_eval);
	QEPP_PRINT( "Average per pooint: eval->%lf,  abserr->%g\n", (double)n_eval/n_pt, (double)abserr/n_pt);
	for( int i=0; i<n_col; i++)
		FreeLinearMem1( (void *)app_y[i]);
	gsl_integration_cquad_workspace_free( wsp);

	QEPP_PRINT( "...done\n");

	return new_s;
}

#else  //__non GSL solutions
#ifdef __MPI //__MPI
opt_data * apply_MSKK_real(opt_data * data, int n_ref, double * ref[2])
{
	if( data == NULL)
	{
		//if( mpi->world_rank == 0)
		//	pp_lerror( "ERROR FUNCTION apply_kk_real:\tPassing NULL data pointer"); pp_perror();
		return NULL;
	}
	QEPP_PRINT( "Starting Multi-Subtractive-KK calculaton.\n");

	long int n_pt = data->n_pt;
	int n_col = data->n_col;
	opt_data * new_s = NULL;
	if( mpi->world_rank == 0)
		new_s = initialize_opt_data( n_pt, n_col);

	double x=0;
	double PV=0;

	QEPP_PRINT( "Starting Kramers_Kronig calculation for Imaginary part starting from Re...\n");

	long int n_div = ceil( n_pt / mpi->world_size);
	long int low_lim = mpi->world_rank * n_div-1;
	//if( mpi->world_rank > 0)
	//	low_lim++;
	long int high_lim = (mpi->world_rank+1) * n_div;
	if( low_lim < 0)
		low_lim = 0;
	if( high_lim > n_pt)
		high_lim = n_pt;
	MPI_Barrier( MPI_COMM_WORLD);
	QEPP_PRINT( "Process %d calculating from %li to %li\n", mpi->world_rank, low_lim+1, high_lim);
	MPI_Barrier( MPI_COMM_WORLD);
	for( long int i=0; i<n_pt; i++)
	{
		x = data->x[i];
		if( mpi->world_rank == 0)
			new_s->x[i] = x;
		double xsq2 = pow( x, 2);
		if( i%1000 == 0)
			QEPP_PRINT( "Reached point num %li/%li\n...",i+1,n_pt);
		if( x >= 0)
		{
			for( int j=0; j<n_col; j++) //For each column...
			{
				//Calculate prefactor (Lagrange interpolation polynomial)
				double pref = 0;
				if( mpi->world_rank == 0)
				{
					double p_app;
					for( int k=0; k<n_ref; k++)
					{
						double wref = ref[0][k];
						double wref2 = pow( wref, 2);
						p_app = ref[1][k] / wref;
						if( p_app != 0)
						{
							double wn, wn2;
							for( int k1=0; k1<k; k1++)
							{
								wn = ref[0][k1];
								wn2 = pow( wn,2);
								p_app *= (xsq2 - wn2) / ( wref2 - wn2);
							}
							for( int k1=k+1; k1<n_ref; k1++)
							{
								wn = ref[0][k1];
								wn2 = pow( wn,2);
								p_app *= (xsq2 - wn2) / ( wref2 - wn2);
							}
						}

						pref += p_app;
					}
				}
				//Calculate Principal value
				double complex PV_c = 0;
				double complex x1, x2, x2sq;
				double complex GAMMA = I * 0.005;
				x1 = data->x[low_lim];
				double complex app;
				for( long int k1=low_lim+1; k1<high_lim; k1++)
				{
					x2 = data->x[k1];
					x2sq = cpow( x2 + GAMMA, 2) ;
					app = data->values[k1-1][j] / (x2sq - xsq2) * (x2 - x1);
					for( int k=0; k<n_ref; k++)
						app /= ( x2sq - cpow( ref[0][k], 2));
					PV_c += app;
					x1 = x2;
				}
				PV = creal( PV_c);
				mp_sum( &PV, MPI_COMM_WORLD, 1);

				//Calculate PV prefact
				if( mpi->world_rank == 0)
				{
					double PV_pref = 1;
					double wref;
					for( int k=0; k<n_ref; k++)
					{
						wref = ref[0][k];
						if( x == wref)
						{
							PV_pref = 0;
							break;
						}
						PV_pref *= (xsq2 - pow( wref, 2));
					}
					new_s->values[i][j] = x * ( pref - 1./PI * PV_pref * PV );
				}
			}
		}
	}
	QEPP_PRINT( "...done\n");

	return new_s;
}
#else  //__MPI not defined
opt_data * apply_MSKK_real(opt_data * data, int n_ref, double * ref[2])
{
	if( data == NULL)
	{
		//pp_lerror( "ERROR FUNCTION apply_kk_real:\tPassing NULL data pointer"); pp_perror();
		return NULL;
	}
	QEPP_PRINT( "Starting Multi-Subtractive-KK calculaton.\n");

	long int n_pt = data->n_pt;
	int n_col = data->n_col;
	opt_data * new_s = initialize_opt_data( n_pt, n_col);

	double x=0;
	double PV=0;

	QEPP_PRINT( "Starting Kramers_Kronig calculation for Imaginary part starting from Re...\n");

	for( long int i=0; i<n_pt; i++)
	{
		x = data->x[i];
		new_s->x[i] = x;
		double xsq2 = pow( x, 2);
		if( i%1000 == 0)
			QEPP_PRINT( "Reached point num %li/%li\n...",i+1,n_pt);
		if( x >= 0)
		{
			for( int j=0; j<n_col; j++) //For each column...
			{
				//Calculate prefactor (Lagrange interpolation polynomial)
				double pref = 0;
				double p_app;
				for( int k=0; k<n_ref; k++)
				{
					double wref = ref[0][k];
					double wref2 = pow( wref, 2);
					p_app = ref[1][k] / wref;
					if( p_app != 0)
					{
						double wn, wn2;
						for( int k1=0; k1<k; k1++)
						{
							wn = ref[0][k1];
							wn2 = pow( wn,2);
							p_app *= (xsq2 - wn2) / ( wref2 - wn2);
						}
						for( int k1=k+1; k1<n_ref; k1++)
						{
							wn = ref[0][k1];
							wn2 = pow( wn,2);
							p_app *= (xsq2 - wn2) / ( wref2 - wn2);
						}
					}

					pref += p_app;
				}
				//Calculate Principal value
				double complex PV_c = 0;
				double complex x1, x2, x2sq;
				double complex GAMMA = I * 0.005;
				x1 = data->x[0];
				double complex app;
				for( long int k1=1; k1<n_pt; k1++)
				{
					x2 = data->x[k1];
					x2sq = cpow( x2 + GAMMA, 2) ;
					app = data->values[k1-1][j] / (x2sq - xsq2) * (x2 - x1);
					for( int k=0; k<n_ref; k++)
						app /= ( x2sq - cpow( ref[0][k], 2));
					PV_c += app;
					x1 = x2;
				}
				PV = creal( PV_c);

				//Calculate PV prefact
				double PV_pref = 1;
				double wref;
				for( int k=0; k<n_ref; k++)
				{
					wref = ref[0][k];
					if( x == wref)
					{
						PV_pref = 0;
						break;
					}
					PV_pref *= (xsq2 - pow( wref, 2));
				}
				new_s->values[i][j] = x * ( pref - 1./PI * PV_pref * PV );
			}
		}
	}

	QEPP_PRINT( "...done\n");

	return new_s;
}
#endif //__MPI
#endif //__GSL

#ifdef __MPI //__MPI
opt_data * apply_MSKK_im(opt_data * data, int n_ref, double * ref[2])
{
	if( data == NULL)
	{
		//if( mpi->world_rank == 0)
		//	pp_lerror( "ERROR FUNCTION apply_kk_real:\tPassing NULL data pointer"); pp_perror();
		return NULL;
	}
	QEPP_PRINT( "Starting Multi-Subtractive-KK calculaton.\n");

	long int n_pt = data->n_pt;
	int n_col = data->n_col;
	opt_data * new_s = NULL;
	if( mpi->world_rank == 0)
		new_s = initialize_opt_data( n_pt, n_col);

	double x=0;
	double PV=0;

	QEPP_PRINT( "Starting Kramers_Kronig calculation for Imaginary part starting from Re...\n");

	long int n_div = ceil( n_pt / mpi->world_size);
	long int low_lim = mpi->world_rank * n_div-1;
	//if( mpi->world_rank > 0)
	//	low_lim++;
	long int high_lim = (mpi->world_rank+1) * n_div;
	if( low_lim < 0)
		low_lim = 0;
	if( high_lim > n_pt)
		high_lim = n_pt;
	MPI_Barrier( MPI_COMM_WORLD);
	QEPP_PRINT( "Process %d calculating from %li to %li\n", mpi->world_rank, low_lim+1, high_lim);
	MPI_Barrier( MPI_COMM_WORLD);
	for( long int i=0; i<n_pt; i++)
	{
		x = data->x[i];
		if( mpi->world_rank == 0)
			new_s->x[i] = x;
		double xsq2 = pow( x, 2);
		if( i%1000 == 0)
			QEPP_PRINT( "Reached point num %li/%li\n...",i+1,n_pt);
		if( x >= 0)
		{
			for( int j=0; j<n_col; j++) //For each column...
			{
				//Calculate prefactor (Lagrange interpolation polynomial)
				double pref = 0;
				if( mpi->world_rank == 0)
				{
					double p_app;
					for( int k=0; k<n_ref; k++)
					{
						double wref = ref[0][k];
						double wref2 = pow( wref, 2);
						p_app = ref[1][k];
						if( p_app != 0)
						{
							double wn, wn2;
							for( int k1=0; k1<k; k1++)
							{
								wn = ref[0][k1];
								wn2 = pow( wn,2);
								p_app *= (xsq2 - wn2) / ( wref2 - wn2);
							}
							for( int k1=k+1; k1<n_ref; k1++)
							{
								wn = ref[0][k1];
								wn2 = pow( wn,2);
								p_app *= (xsq2 - wn2) / ( wref2 - wn2);
							}
						}

						pref += p_app;
					}
				}
				//Calculate Principal value
				double complex PV_c = 0;
				double complex x1, x2, x2sq;
				double complex GAMMA = I * 0.005;
				x1 = data->x[low_lim];
				double complex app;
				for( long int k1=low_lim+1; k1<high_lim; k1++)
				{
					x2 = data->x[k1];
					x2sq = cpow( x2 + GAMMA, 2) ;
					app = x2 * data->values[k1-1][j] / (x2sq - xsq2) * (x2 - x1);
					for( int k=0; k<n_ref; k++)
						app /= ( x2sq - cpow( ref[0][k], 2));
					PV_c += app;
					x1 = x2;
				}
				PV = creal( PV_c);
				mp_sum( &PV, MPI_COMM_WORLD, 1);

				//Calculate PV prefact
				if( mpi->world_rank == 0)
				{
					double PV_pref = 1;
					double wref;
					for( int k=0; k<n_ref; k++)
					{
						wref = ref[0][k];
						if( x == wref)
						{
							PV_pref = 0;
							break;
						}
						PV_pref *= (xsq2 - pow( wref, 2));
					}
					new_s->values[i][j] = ( pref + 1./PI * PV_pref * PV );
				}
			}
		}
	}
	QEPP_PRINT( "done\n");

	return new_s;
}
#else  //__MPI not defined
opt_data * apply_MSKK_im(opt_data * data, int n_ref, double * ref[2])
{
	if( data == NULL)
	{
		//pp_lerror( "ERROR FUNCTION apply_kk_real:\tPassing NULL data pointer"); pp_perror();
		return NULL;
	}
	QEPP_PRINT( "Starting Multi-Subtractive-KK calculaton.\n");

	long int n_pt = data->n_pt;
	int n_col = data->n_col;
	opt_data * new_s = initialize_opt_data( n_pt, n_col);

	double x=0;
	double PV=0;

	QEPP_PRINT( "Starting Kramers_Kronig calculation for Imaginary part starting from Re...\n");

	for( long int i=0; i<n_pt; i++)
	{
		x = data->x[i];
		new_s->x[i] = x;
		double xsq2 = pow( x, 2);
		if( i%1000 == 0)
			QEPP_PRINT( "Reached point num %li/%li\n...",i+1,n_pt);
		if( x >= 0)
		{
			for( int j=0; j<n_col; j++) //For each column...
			{
				//Calculate prefactor (Lagrange interpolation polynomial)
				double pref = 0;
				double p_app;
				for( int k=0; k<n_ref; k++)
				{
					double wref = ref[0][k];
					double wref2 = pow( wref, 2);
					p_app = ref[1][k];
					if( p_app != 0)
					{
						double wn, wn2;
						for( int k1=0; k1<k; k1++)
						{
							wn = ref[0][k1];
							wn2 = pow( wn,2);
							p_app *= (xsq2 - wn2) / ( wref2 - wn2);
						}
						for( int k1=k+1; k1<n_ref; k1++)
						{
							wn = ref[0][k1];
							wn2 = pow( wn,2);
							p_app *= (xsq2 - wn2) / ( wref2 - wn2);
						}
					}

					pref += p_app;
				}
				//Calculate Principal value
				double complex PV_c = 0;
				double complex x1, x2, x2sq;
				double complex GAMMA = I * 0.005;
				x1 = data->x[0];
				double complex app;
				for( long int k1=1; k1<n_pt; k1++)
				{
					x2 = data->x[k1];
					x2sq = cpow( x2 + GAMMA, 2) ;
					app = data->values[k1-1][j] / (x2sq - xsq2) * (x2 - x1);
					for( int k=0; k<n_ref; k++)
						app /= ( x2sq - cpow( ref[0][k], 2));
					PV_c += app;
					x1 = x2;
				}
				PV = creal( PV_c);

				//Calculate PV prefact
				double PV_pref = 1;
				double wref;
				for( int k=0; k<n_ref; k++)
				{
					wref = ref[0][k];
					if( x == wref)
					{
						PV_pref = 0;
						break;
					}
					PV_pref *= (xsq2 - pow( wref, 2));
				}
				new_s->values[i][j] = ( pref + 1./PI * PV_pref * PV );
			}
		}
	}

	QEPP_PRINT( "...done\n");

	return new_s;
}
#endif //__MPI

errh * calculate_opt_properties( opt_data ** out_ptr, opt_data * eps1, opt_data * eps2)
{
	if( out_ptr == NULL)
		FAIL( NULL_OUT, " ");
	*out_ptr = NULL;
	if( eps1 == NULL || eps2 == NULL)
		FAIL( NULL_IN, " ");

	long int n_pt = eps1->n_pt;
	if( n_pt != eps2->n_pt)
		FAIL( FAIL, "Mismatch between n_pt of the 2 structs");

	opt_data * new_s = initialize_opt_data( n_pt, 3+eps1->n_col);
	strcpy( new_s->headers[0], "E[eV]");
	strcpy( new_s->headers[1], "EELS");
	strcpy( new_s->headers[2], "Re[R_index]");
	strcpy( new_s->headers[3], "Im[R_index]");
	strcpy( new_s->headers[4], "Reflect_x");
	if( eps1->n_col > 1)
		strcpy( new_s->headers[5], "Reflect_y");
	if( eps1->n_col > 2)
		strcpy( new_s->headers[6], "Reflect_z");
	if( eps1->n_col > 3)
		strcpy( new_s->headers[7], "Reflect_tot");

	double e1=0, e2=0, x=0;
	double complex ec=0, eta=0;
	for( long int i=0; i<n_pt; i++)
	{
		x = eps1->x[i];
		if( fabs(x - eps2->x[i]) > 1.E-4)
		{
			FREE( new_s);
			FAIL( FAIL, "Mismatch between x coordinates");
		}
		new_s->x[i] = x;
		//Calculate EELS from eps_tot
		if( eps1->n_col > 3)
		{
			e1 = eps1->values[i][3];
			e2 = eps2->values[i][3];
			new_s->values[i][0] = e2 / ( pow( e2, 2) + pow( e1, 2));
		}

		//Calculate refractive index from eps_x
		e1 = eps1->values[i][0];
		e2 = eps2->values[i][0];
		ec = e1 + I * e2;
		eta = csqrt( ec);
		new_s->values[i][1] = creal( eta);
		new_s->values[i][2] = cimag( eta);

		//Calculate R from eps_x/y/z
		for( int j=0; j< eps1->n_col; j++)
		{
			e1 = eps1->values[i][j];
			e2 = eps2->values[i][j];
			ec = e1 + I * e2;
			eta = csqrt( ec);
			new_s->values[i][j+3] = pow( cabs( (eta-1)/(eta+1) ),2);
		}
		/*//Calculate R from eps_y
		e1 = eps1->values[i][1];
		e2 = eps2->values[i][1];
		ec = e1 + I * e2;
		eta = csqrt( ec);
		new_s->values[i][4] = pow( cabs( (eta-1)/(eta+1) ),2);
		//Calculate R from eps_z
		e1 = eps1->values[i][2];
		e2 = eps2->values[i][2];
		ec = e1 + I * e2;
		eta = csqrt( ec);
		new_s->values[i][5] = pow( fabs( (eta-1)/(eta+1) ),2);// */
	}

	*out_ptr = new_s;
	SUCCESS();
}

opt_data * calculate_eps_sl( opt_data * eps[4], const double d1_L)
{
	for( int i=0; i<4; i++)
		if( eps[i] == NULL)
		{
			//pp_lerror( "ERROR FUNCTION calculate_eps_sl:\tPassing NULL data pointer"); pp_perror();
			return NULL;
		}
	if( d1_L < 0 || d1_L > 1)
		return NULL;

	double d2_L = 1 - d1_L;

	long int n_pt = eps[0]->n_pt;
	for( int i=1; i<4; i++)
		if( n_pt != eps[i]->n_pt)
			return NULL;

	opt_data * new_s = initialize_opt_data( n_pt, 7);
	strcpy( new_s->headers[0], "E[eV]");
	strcpy( new_s->headers[1], "Re[paral]");
	strcpy( new_s->headers[2], "Im[paral]");
	strcpy( new_s->headers[3], "Re[perp]");
	strcpy( new_s->headers[4], "Im[perp]");
	strcpy( new_s->headers[5], "EELS");
	strcpy( new_s->headers[6], "refr_para");
	strcpy( new_s->headers[7], "refr_perp");

	/*strcpy( new_s->headers[8], "Im-refr_perp");
	strcpy( new_s->headers[9], "MOD-refr_perp");
	strcpy( new_s->headers[10], "MOD-eps_perp");*/

	double x=0;
	double complex eps_xy[2], eps_zz[2], eps_sl_xy, eps_sl_zz, eps_sl, app;
	for( long int i=0; i<n_pt; i++)
	{
		x = eps[0]->x[i];
		new_s->x[i] = x;

		//eps[0] --> Re(material1)
		//eps[1] --> Im(material1)
		//eps[2] --> Re(material2)
		//eps[3] --> Im(material2)
		eps_xy[0] = 1./2. *     (eps[0]->values[i][0] + eps[0]->values[i][1]) + \
			    1./2. * I * (eps[1]->values[i][0] + eps[1]->values[i][1]);
		eps_xy[1] = 1./2. *     (eps[2]->values[i][0] + eps[2]->values[i][1]) + \
			    1./2. * I * (eps[3]->values[i][0] + eps[3]->values[i][1]);

		//PRINT_COMPLEX( eps_xy[0]); PRINT_COMPLEX( eps[0]->values[i][0]); PRINT_COMPLEX( eps[0]->values[i][1]); getchar();

		eps_zz[0] =     eps[0]->values[i][2] + \
			    I * eps[1]->values[i][2];
		eps_zz[1] =     eps[2]->values[i][2] + \
			    I * eps[3]->values[i][2];

		eps_sl_xy = d1_L *   eps_xy[0]   + d2_L *   eps_xy[1];
		eps_sl_zz = d1_L * 1/eps_zz[0]   + d2_L * 1/eps_zz[1];
		eps_sl_zz = 1. / eps_sl_zz;
		eps_sl = (2*eps_sl_xy + eps_sl_zz)/3;
		new_s->values[i][0] = creal( eps_sl_xy);
		new_s->values[i][1] = cimag( eps_sl_xy);
		new_s->values[i][2] = creal( eps_sl_zz);
		new_s->values[i][3] = cimag( eps_sl_zz);

		new_s->values[i][4] = cimag( eps_sl) / (eps_sl * conj( eps_sl));
		app = csqrt( eps_sl_xy);
		new_s->values[i][5] = creal( app);
		app = csqrt( eps_sl_zz);
		//PRINT_COMPLEX( eps_sl_zz); PRINT_COMPLEX( app); 
		new_s->values[i][6] = creal( app);

		/*new_s->values[i][7] = cimag( app);
		new_s->values[i][8] = app * conj(app);
		new_s->values[i][9] = eps_sl_zz * conj( eps_sl_zz);*/
	}

	return new_s;
}

errh * calculate_eps_2d( opt_data ** out_ptr, opt_data * eps1, opt_data * eps2, const double d, const double L)
{
	if( out_ptr == NULL)
		FAIL( NULL_OUT, " ");
	*out_ptr = NULL;
	if( eps2 == NULL)
		FAIL( NULL_IN, " ");
	if( d < 0 || d/L > 1)
		FAIL( FAIL, "Invalid input parameters");

	long int n_pt = eps2->n_pt;

	if( eps1 == NULL)
		eps1 = apply_kk( eps2);

	opt_data * new_s = initialize_opt_data( n_pt, 8);
	strcpy( new_s->headers[0], "E[eV]");
	//strcpy( new_s->headers[1], "Re[eps2d//]");
	strcpy( new_s->headers[1], "       Im[ε2d//]");
	//strcpy( new_s->headers[3], "Re[eps2d|_]");
	strcpy( new_s->headers[2], "       Im[ε2d|_]");
	strcpy( new_s->headers[3], "    Re[σ//](1/Ω)");
	strcpy( new_s->headers[4], "    Re[σ|_](1/Ω)");
	strcpy( new_s->headers[5], "     Re[σ_tilde]");
	strcpy( new_s->headers[6], "               R");
	strcpy( new_s->headers[7], "               T");
	strcpy( new_s->headers[8], "               A");

	/*strcpy( new_s->headers[8], "Im-refr_perp");
	strcpy( new_s->headers[9], "MOD-refr_perp");
	strcpy( new_s->headers[10], "MOD-eps_perp");*/

	double x=0;
	double complex eps_sl_para, eps_sl_orto, eps_2d_para, eps_2d_orto, sigma_para, sigma_orto, sigma_tilde;
	double complex app;
	double R, T, A;
	for( long int i=0; i<n_pt; i++)
	{
		x = eps2->x[i];
		new_s->x[i] = x;

		eps_sl_para = 1./2. *     (eps1->values[i][0] + eps1->values[i][1]) + \
			      1./2. * I * (eps2->values[i][0] + eps2->values[i][1]);
		eps_sl_orto =     eps1->values[i][2] + \
			      I * eps2->values[i][2];

		eps_2d_para = 1. + L/d * (    eps_sl_para - 1.);
		eps_2d_orto = 1. + L/d * ( 1./eps_sl_orto - 1.);
		eps_2d_orto = 1. / eps_2d_orto;

		sigma_para = -I * (    eps_sl_para - 1.) * L * 134.5 * x;
		sigma_orto = -I * ( 1. - 1./eps_sl_orto) * L * 134.5 * x;

		sigma_tilde = sigma_para / ( 8.85419E-12 * 2.99E8);

		app = (sigma_tilde/2.) / (1. + sigma_tilde/2.);
		app = app * conj(app);
		R = creal( app);

		app = 1. + sigma_tilde/2.;
		app = app * conj(app);
		T = 1./creal(app);

		A = creal( sigma_tilde) / creal(app);

		new_s->values[i][0] = cimag(eps_2d_para);
		new_s->values[i][1] = cimag(eps_2d_orto);
		new_s->values[i][2] = creal(sigma_para);
		new_s->values[i][3] = creal(sigma_orto);
		new_s->values[i][4] = creal( sigma_tilde);
		new_s->values[i][5] = R;
		new_s->values[i][6] = T;
		new_s->values[i][7] = A;
	}

	*out_ptr = new_s;
	SUCCESS();
}


errh * qepp_compute_cd_datafile( cd ** out_ptr, data_file * data, long int x, long int y, long int z, 
	long int sel_kpt, int sel_bnd, int multiplier)
{
	if( out_ptr == NULL)
		FAIL( NULL_OUT, " ");
	*out_ptr = NULL;
	if( data == NULL)
		FAIL( NULL_IN, " ");
	cd * res= NULL;

	res = initialize_cd( x, y, z);

	long int n_kpt = data->n_kpt;

	wfc * w = NULL, * w2 = NULL;
	gkv * g = NULL;

	double a[3], vol;
	vector_vector( data->a[0], data->a[1], a);
	scalar_vector( a, data->a[2], &vol);

	QEPP_PRINT( "Volume: %g %s\n", vol, data->a_n); 
	vol *= pow( data->a_t, 3);
	QEPP_PRINT( "Volume: %g (m^-3)\n",vol);

	double coord[3]={0};
	double delta[3]={ data->c_dim[0]*multiplier, data->c_dim[1]*multiplier, data->c_dim[2]*multiplier};
	for( int n=1; n<3; n++)
	{
		if( delta[n] == 0)
			delta[n] = delta[0];
		else
			delta[n] *= delta[0];
	}
	double integrated = 0;
	double dv = 1;
	double app[3] = {x,y,z};
	for( int n=0; n<3; n++)
	{
		delta[n] *= data->a_t;
		dv *= delta[n];
		delta[n] /= (app[n] - 1);
	}
	dv /= x*y*z;

	double weight;
	int nbnd;
	int n_el;
	int fac=2;
	if( data->n_spin == 1)
		fac=1;
	int min_bnd=0, max_bnd;

	long int min_kpt=0, max_kpt=n_kpt;
	if( sel_kpt > 0)
	{
		min_kpt = sel_kpt-1;
		max_kpt = sel_kpt;
	}
	for( long int k=min_kpt; k<max_kpt; k++)
	{
		if( READ( data->wfc_link[k], &w) != SUCCESS)
			FAIL( FAIL, "Failed to read wavefunction from file: %s", data->wfc_link[k]);
		if( data->n_spin == 4)
		{
			if( READ( data->wfc_link2[k], &w2) != SUCCESS)
				FAIL( FAIL, "Failed to read wavefunction from file: %s", data->wfc_link2[k]);
		}
		nbnd = w->nbnd;
		n_el = ROUND( data->n_el);
		if( n_el % 2 != 0)
			WARN( "Uneven number of electrons");
		max_bnd = n_el / fac;
		if( nbnd < max_bnd)
			WARN( "Number of KS states is lower than the number of electrons");

		if( sel_bnd > 0)
		{
			min_bnd = sel_bnd - 1;
			max_bnd = sel_bnd;
		}
		
		if( READ( data->egvec_link[k], &g) != SUCCESS)
			FAIL( FAIL, "Failed to read g vectors from file: %s", data->egvec_link[k]);

		double KG[3];
		double G[g->ngkv][3];
		weight = data->weight[k];
		for( long int j=0; j<g->ngkv; j++)
		{
			for( int n=0; n<3; n++)
			{
				G[j][n] = 0;
				for( int n1=0; n1<3; n1++)
					G[j][n] += g->grid[j][n1] * data->b[n1][n] * data->b_t;
			}
		}


		if( w->igwx != g->ngkv)
			WARN( "w->igwx differ from g->ngkv!!!!\n");

		long int start=0;
		long int end=w->igwx;
		SET_PW_MPI( start, end);

		QEPP_PRINT( "Computing kpt #%li...\n", k+1);
#ifdef __MPI
		QEPP_PRINT( "Process %d calculating PW from %li to %li/%li\n", mpi->world_rank, start, end,w->igwx);
		QEPP_PRINT( "\n");
#endif

		double complex * c_app, * c_app2 = NULL;
		for( int bnd=min_bnd; bnd<max_bnd; bnd++)
		{
			c_app = w->evc_vect[bnd]->val;
			if( data->n_spin == 4)
				c_app2 = w2->evc_vect[bnd]->val;			

			for( long int n1=0; n1<x; n1++)
			{
				coord[0] = delta[0] * n1;
				for( long int n2=0; n2<y; n2++)
				{
					coord[1] = delta[1] * n2;
					for( long int n3=0; n3<z; n3++)
					{
						coord[2] = delta[2] * n3;
						double complex n_app = 0, n_app2 =0, exp_app;

						double complex cc =0, cc2=0;
						double exponential=0;

						cc = 0; cc2=0;

						for( long int ng=start; ng<end; ng++)
						{
							exponential = 0;
							for( int gg=0; gg<3; gg++)
							{
								KG[gg] = G[ng][gg] + data->kpt[k][gg] * data->b_t;
								exponential += KG[gg] * coord[gg];
							}
							exp_app = cexp( I * exponential);
							cc += c_app[ng] * exp_app;
							if( data->n_spin == 4)
								cc2 += c_app2[ng] * exp_app;
						}
						mp_sum( &cc, MPI_COMM_WORLD, 1);

						n_app += cc * conj( cc);     

						n_app /= vol;     
						n_app *= weight * fac;     
						res->dens[n1][n2][n3] = creal( n_app);

						integrated += creal(n_app);

						if( data->n_spin == 4)
						{
							n_app2 += cc2 * conj( cc2);
							n_app2 /= vol;
							n_app2 *= weight * fac;
							res->dens[n1][n2][n3] += creal( n_app2);
							integrated += creal(n_app2);
						}

						memcpy( res->coord[n1][n2][n3], coord, 3*sizeof( double));
					}
				}
			}
		}

		FREE( w);
		FREE( w2);
		FREE( g);
	}
	

	integrated *= dv;
	QEPP_PRINT( "Integrated density: %g\n",integrated);


	*out_ptr = res;
	SUCCESS();
}

errh * qepp_compute_dos_nscf( opt_data ** out_ptr, nscf_data * data, double min, double max, double delta)
{
	opt_data * res = NULL;
	if( out_ptr == NULL)
		FAIL( NULL_OUT, " ");
	*out_ptr = NULL;
	if( data == NULL)
		FAIL( NULL_IN, " ");

	QEPP_PRINT( "\nStarting DOS calculation...\n");

	long int n_pt = (max-min)/delta + 0.500001;
	if( n_pt<0)
	{
		WARN( "min is greather than max, inverting them...");
		double app = min;
		min = max; 
		max = app;
		n_pt = (max-min)/delta + 0.500001;
	}

	res = initialize_opt_data( n_pt, 1);
	strcpy( res->headers[0], "Energy (eV)");
	strcpy( res->headers[1], "DOS");
	for( long int i=0; i<n_pt; i++)
	{
		res->x[i] = min + i*delta;
		res->values[i][0] = 0;
	}

	long int n_kpt = data->n_kpt;
	long int app_i;
	int n_bnd = data->n_bnd;
	double weight;
	double * en;
	for( long int i=0; i<n_kpt; i++)
	{
		weight = data->weight[i];
		en = data->energies[i];
		for( int j=0; j<n_bnd; j++)
		{
			app_i = floor((en[j] - min) / delta);
			if( app_i >= 0 && app_i < n_pt)
				res->values[app_i][0] += weight;
		}
	}
	for( long int i=0; i<n_pt; i++)
		res->values[i][0] /= delta;

	QEPP_PRINT( "End DOS calculation\n");

	*out_ptr = res;
	SUCCESS();
}

errh * qepp_compute_dos_datafile( opt_data ** out_ptr, data_file * data, double min, double max, double delta)
{
	opt_data * res = NULL;
	if( out_ptr == NULL)
		FAIL( NULL_OUT, " ");
	*out_ptr = NULL;
	if( data == NULL)
		FAIL( NULL_IN, " ");

	*out_ptr = res;
	SUCCESS();	
}

errh * qepp_compute_matrixelements( opt_data ** out_ptr, data_file * data)
{
	opt_data * res = NULL;
	if( out_ptr == NULL)
		FAIL( NULL_OUT, " ");
	*out_ptr = NULL;
	if( data == NULL)
		FAIL( NULL_IN, " ");

	*out_ptr = res;
	SUCCESS();
}


errh * qepp_compute_matrixelements_huge( void ** dump, data_file * data)
{
	if( data == NULL)
		FAIL( NULL_IN, " ");

	wfc * wfc1 = NULL;
	wfc * wfc2 = NULL;
	gkv * gkv1 = NULL;
	egv * egv1 = NULL;
	const char * filename = "matrixelements";
	FILE * out = NULL;

	long int n_kpt = data->n_kpt;
	int n_bnd = data->n_bnd;

	long int start_k=0;
	int start_b1=0;
	if( is_file( (char *)filename))
	{
		WARN( "File %s already exist... attempting restart mode\n", filename);
		out = fopen( filename, "a+");
		fseek( out, 0, SEEK_END);
		char c;
		do
		{
			fseek( out, -2, SEEK_CUR);
			c = fgetc( out);
		} while( c != '\n');
		double app;
		my_fscanf_double( out, &app);
		start_k = app-1;
		my_fscanf_double( out, &app);
		start_b1 = app;
	}
	else
		out = fopen( filename, "w");
	if( out == NULL)
		FAIL( FAIL, "Cannout open file matrixelements for write");

	double complex * app = (double complex *)AllocateLinearMem1( sizeof( double complex), 3);

	//double tpiba = 2  * PI / (data->lp);
	double tpiba2 = pow( data->a_t, 2);

	for( long int i=start_k; i<n_kpt; i++)
	{
		if( READ( &wfc1, data->wfc_link[i]) != SUCCESS)
			FAIL( FAIL, "Failed to read wavefunction from file: %s", data->wfc_link[i]);
		if( data->n_spin == 4)
		{
			if( READ( &wfc2, data->wfc_link2[i]) != SUCCESS)
				FAIL( FAIL, "Failed to read wavefunction from file: %s", data->wfc_link2[i]);
		}

		if( READ( data->egval_link[i], &egv1) != SUCCESS)
			FAIL( FAIL, "Failed to read eigenvalues from file: %s", data->egval_link[i]);

		if( READ( data->egvec_link[i], &gkv1) != SUCCESS)
			FAIL( FAIL, "Failed to read eigenvectors from file: %s", data->egvec_link[i]);

		double KG;
		double G[gkv1->ngkv][3];
		//double weight = data->weight[i];

		//Change G from cryst to cart
		for( long int j=0; j<gkv1->ngkv; j++)
		{
			for( int n=0; n<3; n++)
			{
				G[j][n] = 0;
				for( int n1=0; n1<3; n1++)
					G[j][n] += gkv1->grid[j][n1] * data->b[n1][n] * data->b_t;
			}
		}

		long int start=0;
		long int end=wfc1->igwx;
		SET_PW_MPI( start, end);

		QEPP_PRINT( "Computing kpt #%li...\n", i+1);
#ifdef __MPI
		QEPP_ALL_PRINT( "Process %d calculating PW from %li to %li/%li\n", mpi->world_rank, start, end,wfc1->igwx);
		QEPP_PRINT("\n");
#endif

		for( int j1=start_b1; j1<n_bnd; j1++)
		{
			start_b1=0;
			for( int j2=j1+1; j2<n_bnd; j2++)
			{
				double gap = (egv1->val[j2] - egv1->val[j1]) * egv1->e_t;
				double occ = 2*(egv1->occ[j1] - egv1->occ[j2]);

				if( occ > 1.E-4)
				{
					double complex *app1, *app2, app3;
					double mel[3];
					for( int dir=0; dir<3; dir++)
						app[dir]=0;
					for( long int ng=start; ng<end;ng++)
					{
						app1 = wfc1->evc_vect[j1]->val;
						app2 = wfc1->evc_vect[j2]->val;
						app3 = conj(app1[ng]) * app2[ng];
						if( data->n_spin == 4)
						{
							app1 = wfc2->evc_vect[j1]->val;
							app2 = wfc2->evc_vect[j2]->val;
							app3 += conj(app1[ng]) * app2[ng];
						}
						for( int dir=0; dir<3; dir++)
						{
							KG = G[ng][dir] + data->kpt[i][dir] * data->b_t;
							app[dir] += app3*KG;
						}
					}
					mp_sum( app, MPI_COMM_WORLD);


					for( int dir=0; dir<3; dir++)
						mel[dir] = tpiba2 * creal( app[dir] * conj( app[dir]));
					QEPP_OUT( out, " %5li%5d%5d%16.8e%16.8E%16.8E%16.4lf%16.4lf\n", 
						i+1, j1+1, j2+1, mel[0], mel[1], mel[2], gap, occ);
				}
			}
			fflush( out);
		}

		FREE( wfc1);
		FREE( wfc2);
		FREE( egv1);
		FREE( gkv1);
	}

	FreeLinearMem1( (void *)app);

	fclose( out);	
	SUCCESS();
}

errh * qepp_compute_berry_phase( double complex ** out_ptr, data_file * data)
{
	long int n_kpt;
	int n_bnd;
	wfc * wfc1 = NULL;
	wfc * wfc2 = NULL;
	wfc * wfc3 = NULL;
	wfc * wfc4 = NULL;
	gkv * gkv1 = NULL;
	gkv * gkv3 = NULL;
	double complex * res = NULL;
	if( out_ptr == NULL)
		FAIL( NULL_OUT, " ");
	*out_ptr = NULL;
	if( data == NULL)
		FAIL( NULL_IN, " ");

	n_kpt = data->n_kpt;
	n_bnd = data->n_bnd;		

	res = (double complex *)AllocateLinearMem1( sizeof( double complex), n_bnd);
	for( int i=0; i<n_bnd; i++)
		res[i] = 1;


	if( READ( &wfc3, data->wfc_link[0]) != SUCCESS)
		FAIL( FAIL, "Failed to read wavefunction from file: %s", data->wfc_link[0]);
	if( data->n_spin == 4)
	{
		if( READ( &wfc4, data->wfc_link2[0]) != SUCCESS)
			FAIL( FAIL, "Failed to read wavefunction from file: %s", data->wfc_link2[0]);
	}
	if( READ( &gkv3, data->egvec_link[0]) != SUCCESS)
		FAIL( FAIL, "Failed to read g vectors from file: %s", data->egvec_link[0]);

	FILE * out = fopen( "qepp_berry.dat", "w");
	double complex berry1 = 1;
	double berry2 = 0;
	for( long int i=0; i<n_kpt; i++)
	{
		wfc1 = wfc3;
		wfc2 = wfc4;
		gkv1 = gkv3;

		if( READ( &wfc3, data->wfc_link[(i+1)%n_kpt]) != SUCCESS)
			FAIL( FAIL, "Failed to read wavefunction from file: %s", data->wfc_link[(i+1)%n_kpt]);
		if( data->n_spin == 4)
		{
			if( READ( &wfc4, data->wfc_link2[(i+1)%n_kpt]) != SUCCESS)
				FAIL( FAIL, "Failed to read wavefunction from file: %s", data->wfc_link2[(i+1)%n_kpt]);
		}
		if( READ( &gkv3, data->egvec_link[(i+1)%n_kpt]) != SUCCESS)
			FAIL( FAIL, "Failed to read g vectors from file: %s", data->egvec_link[(i+1)%n_kpt]);	

		long int start=0;
		long int end=wfc1->igwx;
		SET_PW_MPI( start, end);

		QEPP_PRINT( "Computing kpt #%li...\n", i+1);
#ifdef __MPI
		QEPP_ALL_PRINT( "Process %d calculating PW from %li to %li/%li\n", mpi->world_rank, start, end,wfc1->igwx);
		QEPP_PRINT( "\n");
#endif

		//double app1=1;
		for( int j=0; j<n_bnd; j++)
		{
			double complex app2=0;
			long int ng2=0;
			long int ng2_app=0;
			for( long int ng1=start; ng1<end;ng1++)
			{
				for( ng2=ng2_app; ng2<wfc3->igwx; ng2++)
				{
					if( gkv1->index[ng1] == gkv3->index[ng2])
					{
						ng2_app = ng2;
						app2 += conj( wfc1->evc_vect[j]->val[ng1]) * wfc3->evc_vect[j]->val[ng2];
						if( data->n_spin == 4)
							app2 += conj( wfc2->evc_vect[j]->val[ng1]) * wfc4->evc_vect[j]->val[ng2];
						break;
					}
					if( gkv3->index[ng2] > gkv1->index[ng1])
						break;
				}
			}
			mp_sum( &app2, MPI_COMM_WORLD, 1);

			berry1 *= app2;
			double app_b = atan2( cimag( app2), creal( app2));
			berry2 -= app_b;
			double app123 = berry2 - TWOPI*floor((berry2+PI)/TWOPI);
			QEPP_OUT( out, "ik= %5li, bnd= %5d, overlap= %12.4E%12.4E -> prod= %12.4E%12.4E, atan= %12.4E, b1= %12.4E %12.4E\n",
				i+1,j+1,creal(app2),cimag(app2),creal(berry1),cimag(berry1),app_b, -cimag( clog( berry1)), app123);
			res[j] *= app2;
		}

		FREE( wfc1);
		FREE( wfc2);
		FREE( gkv1);
	}
	berry2 -= TWOPI*floor((berry2+PI)/TWOPI);
	QEPP_OUT( out, "%g +i %g:  (clog)%g  <->  (atan)%g\n", creal(berry1), cimag(berry1), -cimag(clog(berry1)), berry2);
	fclose( out);

	*out_ptr = res;
	SUCCESS();
}

errh * qepp_compute_berry_phase2( double complex ** out_ptr, data_file * data, long int * klist, long int num)
{
	long int n_kpt;
	int n_bnd;
	wfc * wfc1 = NULL;
	wfc * wfc2 = NULL;
	wfc * wfc3 = NULL;
	wfc * wfc4 = NULL;
	gkv * gkv1 = NULL;
	gkv * gkv3 = NULL;
	double complex * res = NULL;
	if( out_ptr == NULL)
		FAIL( NULL_OUT, " ");
	*out_ptr = NULL;
	if( data == NULL)
		FAIL( NULL_IN, " ");

	n_kpt = num;
	n_bnd = data->n_bnd;
	if( klist == NULL)
	{
		n_kpt = data->n_kpt;
		klist = AllocateLinearMem1( sizeof( long int), n_kpt);
		for( long int i=0; i<n_kpt; i++)
			klist[i] = i;
	}		
	res = (double complex *)AllocateLinearMem1( sizeof( double complex), n_bnd);
	for( int i=0; i<n_bnd; i++)
	{
		res[i] = 1;
		if( isnan( creal(res[i]))) QEPP_PRINT( "%lf\n", data->kpt[n_kpt+1000000][0]);
	}


	if( READ( &wfc3, data->wfc_link[0]) != SUCCESS)
		FAIL( FAIL, "Failed to read wavefunction from file: %s", data->wfc_link[0]);
	if( data->n_spin == 4)
	{
		if( READ( &wfc4, data->wfc_link2[0]) != SUCCESS)
			FAIL( FAIL, "Failed to read wavefunction from file: %s", data->wfc_link2[0]);
	}
	if( READ( &gkv3, data->egvec_link[0]) != SUCCESS)
		FAIL( FAIL, "Failed to read g vectors from file: %s", data->egvec_link[0]);

	long int k3;

	double complex berry1 = 1;
	double berry2 = 0;
	for( long int i=0; i<n_kpt; i++)
	{
		k3=klist[(i+1)%n_kpt];
		wfc1 = wfc3;
		wfc2 = wfc4;
		gkv1 = gkv3;

		if( READ( &wfc3, data->wfc_link[k3]) != SUCCESS)
			FAIL( FAIL, "Failed to read wavefunction from file: %s", data->wfc_link[k3]);
		if( data->n_spin == 4)
		{
			if( READ( &wfc4, data->wfc_link2[k3]) != SUCCESS)
				FAIL( FAIL, "Failed to read wavefunction from file: %s", data->wfc_link2[k3]);
		}
		if( READ( &gkv3, data->egvec_link[k3]) != SUCCESS)
			FAIL( FAIL, "Failed to read g vectors from file: %s", data->egvec_link[k3]);		

		/*double G1[gkv1->ngkv][3];
		for( long int j=0; j<gkv1->ngkv; j++)
		{
			for( int n=0; n<3; n++)
			{
				G1[j][n] = 0;
				for( int n1=0; n1<3; n1++)
					G1[j][n] += gkv1->grid[j][n1] * data->b[n1][n] * data->b_t;
			}
		}
		double G3[gkv3->ngkv][3];
		for( long int j=0; j<gkv3->ngkv; j++)
		{
			for( int n=0; n<3; n++)
			{
				G3[j][n] = 0;
				for( int n1=0; n1<3; n1++)
					G3[j][n] += gkv3->grid[j][n1] * data->b[n1][n] * data->b_t;
			}
		}*/

		long int start=0;
		long int end=wfc1->igwx;
		SET_PW_MPI( start, end);

		QEPP_PRINT( "Computing kpt #%li...\n", i+1);
#ifdef __MPI
		QEPP_ALL_PRINT( "Process %d calculating PW from %li to %li/%li\n", mpi->world_rank, start, end,wfc1->igwx);
		QEPP_PRINT( "\n");
#endif

		//double app1=1;
		for( int j=0; j<n_bnd; j++)
		{
			double complex app2=0;
			long int ng2=0;
			long int ng2_app=0;
			long int match=0;
			for( long int ng1=start; ng1<end;ng1++)
			{
				for( ng2=ng2_app; ng2<wfc3->igwx; ng2++)
				{
					if( gkv1->index[ng1] == gkv3->index[ng2])
					{
						match++;
						ng2_app = ng2;
						app2 += conj(wfc1->evc_vect[j]->val[ng1]) * wfc3->evc_vect[j]->val[ng2];
						if( data->n_spin == 4)
							app2 += conj(wfc2->evc_vect[j]->val[ng1]) * wfc4->evc_vect[j]->val[ng2];
						break;
					}
					if( gkv3->index[ng2] > gkv1->index[ng1])
						break;
				}
			}
			mp_sum( &app2, MPI_COMM_WORLD, 1);

			
			berry1 *= app2;
			berry2 -= atan2( cimag( app2), creal( app2));
			double complex ccc = app2* res[j];
			if( creal( ccc) == 0 || cimag( ccc) == 0)
				QEPP_PRINT( "j= %d:  res[j](%E +I* %E) * app2(%E +I* %E) = ccc(%E +I* %E)  \n", j, \
				creal( res[j]), cimag( res[j]), creal( app2), cimag( app2), creal( ccc), cimag( ccc));
			if( match > 0)
				res[j] *= app2;
		}

		FREE( wfc1);
		FREE( wfc2);
		FREE( gkv1);
	}
	berry2 -= TWOPI*floor((berry2+PI)/TWOPI);

	for( int i=0; i<n_bnd; i++)
	{
		QEPP_PRINT( "%d:   %E %E\n", i, creal( res[i]), cimag( res[i]));
		res[i] = clog( res[i]);
	}

	QEPP_PRINT( "%g +i %g:  (clog)%g  <->  (atan)%g\n", creal(berry1), cimag(berry1), -cimag(clog(berry1)), fmod( berry2, 2*PI));

	//FreeLinearMem1( (void *)app_v);
	FreeLinearMem1( (void *)klist);

	*out_ptr = res;
	SUCCESS();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
errh * calculate_jdos_huge_files( opt_data ** out_ptr, char * mname, double max_range, long int n_pt, double a3, double v_cell)
{
	if( out_ptr == NULL)
		FAIL( NULL_OUT, " ");
	*out_ptr = NULL;
	if( mname == NULL)
		FAIL( NULL_IN, " ");

	double delta = max_range / (double)n_pt;
	n_pt++;
	double fact=1;
	char *filename="nscf_1.out";
	if( !is_file( filename) && (v_cell == 0 || !is_file( "k.dat")))
		filename = get_file( filename, ".out");

	if( max_range == 0 || n_pt == 0)
		FAIL( FAIL, "Invalid input parameters");
	nscf_md * app;
	if( READ( filename, &app) != SUCCESS)
		FAIL( FAIL, "Failed to read metadata from nscf file");
	if(v_cell == 0)
	{
		if( app == NULL)
			FAIL( FAIL, "Can't acquire cell volume data");
		v_cell = app->vc;
	}
	if( a3 == 0)	//Bulk
		a3=1;
	else		//Surface
		fact=0.5;

//Read k points data from k.dat or if not available nscf_1.out or return NULL
	double tot_w=0;
	long int n_kpt;
	double * weight;
	opt_data * k_pt=NULL;
	if( READ( "k.dat", &k_pt) != SUCCESS)
	{
		nscf_data * app;
		if( READ( filename, &app) != SUCCESS)
			FAIL( FAIL, "Can't acquire k-points data");
		n_kpt=app->n_kpt;
		tot_w = app->tot_weight/2;
		weight = (double *)AllocateLinearMem1( sizeof( double), n_kpt);//malloc( n_kpt * sizeof( double));
		memcpy(weight, app->weight, app->n_kpt * sizeof( double));
		FREE(app);
	}
	else
	{
		n_kpt= k_pt->n_pt;
		weight = (double *)AllocateLinearMem1( sizeof( double), n_kpt);//malloc( n_kpt * sizeof( double));
		double app;
		for( long int i=0; i<k_pt->n_pt; i++)
		{
			app = k_pt->values[i][2];
			weight[i]=app;
			tot_w+=app;
		}
	}
	for( long int i=0; i< n_kpt; i++)
		weight[i]/=tot_w;

//Start actual calc
	//Generate result sructure
	FILE * read = fopen( mname , "r");
	if( read == NULL)
		FAIL( OPEN_IN_FAIL, " ");

	opt_data * eps = initialize_opt_data( n_pt, 1);
	MAKE_HEADERS(eps,EPS_HEADERS);
	//Initialize x and y values of result
	for( long int i=0; i< n_pt; i++)
	{
		eps->x[i]= (double)i * delta;
		for( int i1=0; i1<eps->n_col; i1++)
			eps->values[i][i1]=0;
	}
//Read matrixelements and redistribute them in
//matrix element[k][x->en][i/j/k]]
	QEPP_PRINT( "Allocating %.2lf MB of RAM for pp...\n", n_kpt*n_pt*8./1024./1024.);
	double **pp;
	pp = (double **)AllocateLinearMem2( sizeof( double), n_kpt, n_pt);
	QEPP_PRINT( "...done\n");
// data->values[x] =?   x: 0-> v_b  1-> c_b  2/3/4-> |p_x/y/z|^2  5-> e_gap  6-> Df(occ)
	long int k_app, p_app;
	double e_gap=0;
	int vb=0, cb=0;
	double kpp[3];
	double df=0;

	long int npt=0;
	char buffer[256];
	do {
		my_getline( buffer, 256, read);
	} while( !feof( read) && buffer[0] == '#');
	QEPP_PRINT( "Start reading from file \"%s\"...\n",mname);
	while(!feof(read))
	{
		if( (npt)%1000000==0)
			QEPP_PRINT( "Reading line num %li...\n", npt+1);
		npt++;
		sscanf( buffer, "%li %d %d %lf %lf %lf %lf %lf", &k_app, &vb, &cb, &kpp[0], &kpp[1], &kpp[2], &e_gap, &df);
		k_app--; vb--; cb--;
		p_app=(long int)ROUND(e_gap/delta);

		if(p_app < n_pt && p_app >=0)
			if(vb < cb)
				pp[k_app][p_app] += df;
		my_getline( buffer, 256, read);
	}
	QEPP_PRINT( "...%li lines read. Done\n", npt);
	fclose( read);

	double prefact = fact * 4.0 * pow(PI,2) * pow(2*RY_TO_EV,3) * a3 / v_cell;
	if( app != NULL)
		if( app->spin_orbit)
		{
			prefact/=2;
			QEPP_PRINT( "Spinorbit calculation detected\n");
		}
	FREE( app);
//Trasform pp in eps_2[k][x->en][i/j/k]
	for( long int i=0; i<n_kpt; i++)
		for( long int i1=1; i1<n_pt; i1++)
				pp[i][i1]*=prefact*weight[i]/delta; 

//Recalc all for x[i] -> (x[i]+x[i-1])/2
	for( long int i=eps->n_pt-1; i>0; i--)
	{
		eps->x[i]=(eps->x[i]+eps->x[i-1])/2.;
		for( long int i1=0; i1<n_kpt; i1++ )
				eps->values[i][0]+=(pp[i1][i] + pp[i1][i-1])/2.;
	}


//Remove 0 pt
	eps->n_pt--;
	double * app1 = (double *)AllocateLinearMem1( sizeof( double), eps->n_pt);
	memcpy( app1, eps->x+1, eps->n_pt * sizeof( double));
	FreeLinearMem1( (void *)eps->x);
	eps->x=app1;

	double ** app2 = (double **)AllocateLinearMem2( sizeof(double), eps->n_pt, eps->n_col);
	for( long int i=0; i<eps->n_pt; i++)
		memcpy( app2[i], eps->values[i+1], eps->n_col * sizeof( double));
	FreeLinearMem2( (void **)eps->values);
	eps->values=app2;

//Free all unused variables
	FREE(k_pt);
	FreeLinearMem1( (void *)weight);
	FreeLinearMem2( (void **)pp);

	*out_ptr = eps;
	QEPP_PRINT( "End jdos calculation\n");
	SUCCESS();
}

























