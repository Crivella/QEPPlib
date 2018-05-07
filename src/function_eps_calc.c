#include <qepp/qepp_function.h>

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
	if( !qepp_is_file( filename) && (v_cell == 0 || !qepp_is_file( "k.dat")))
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
		weight = (double *)QEPP_ALLOC( sizeof( double), n_kpt);//malloc( n_kpt * sizeof( double));
		memcpy(weight, app->weight, app->n_kpt * sizeof( double));
		FREE(app);
	}
	else
	{
		n_kpt=k_pt->n_pt;
		weight = (double *)QEPP_ALLOC( sizeof( double), n_kpt);//malloc( n_kpt * sizeof( double));
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
	pp = (double ***)QEPP_ALLOC( sizeof( double), n_kpt, n_pt, 3);
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
	double * app1 = (double *)QEPP_ALLOC( sizeof( double), eps->n_pt);
	memcpy( app1, eps->x+1, eps->n_pt * sizeof( double));
	QEPP_FREE( (void *)eps->x);
	eps->x=app1;

	double ** app2 = (double **)QEPP_ALLOC( sizeof(double), eps->n_pt, eps->n_col);
	for( long int i=0; i<eps->n_pt; i++)
		memcpy( app2[i], eps->values[i+1], eps->n_col * sizeof( double));
	QEPP_FREE( (void **)eps->values);
	eps->values=app2;

//Free all unused variables
	FREE(k_pt);
	QEPP_FREE( (void *)weight);
	QEPP_FREE( (void ***)pp);

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
	if( !qepp_is_file( filename) && (v_cell == 0 || !qepp_is_file( "k.dat")))
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
		weight = (double *)QEPP_ALLOC( sizeof( double), n_kpt);//malloc( n_kpt * sizeof( double));
		memcpy(weight, app->weight, app->n_kpt * sizeof( double));
		FREE(app);
	}
	else
	{
		n_kpt= k_pt->n_pt;
		weight = (double *)QEPP_ALLOC( sizeof( double), n_kpt);//malloc( n_kpt * sizeof( double));
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
	pp = (double ***)QEPP_ALLOC( sizeof( double), n_kpt, n_pt, 3);
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
		qepp_getline( buffer, 256, read);
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
		qepp_getline( buffer, 256, read);
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
	double * app1 = (double *)QEPP_ALLOC( sizeof( double), eps->n_pt);
	memcpy( app1, eps->x+1, eps->n_pt * sizeof( double));
	QEPP_FREE( (void *)eps->x);
	eps->x=app1;

	double ** app2 = (double **)QEPP_ALLOC( sizeof(double), eps->n_pt, eps->n_col);
	for( long int i=0; i<eps->n_pt; i++)
		memcpy( app2[i], eps->values[i+1], eps->n_col * sizeof( double));
	QEPP_FREE( (void **)eps->values);
	eps->values=app2;

//Free all unused variables
	FREE(k_pt);
	QEPP_FREE( (void *)weight);
	QEPP_FREE( (void ***)pp);

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
		weight = (double *)QEPP_ALLOC( sizeof( double), n_kpt);//malloc( n_kpt * sizeof( double));
		memcpy(weight, app->weight, app->n_kpt * sizeof( double));
		FREE(app);
	}
	else
	{
		n_kpt=k_pt->n_pt;
		weight = (double *)QEPP_ALLOC( sizeof( double), n_kpt);//malloc( n_kpt * sizeof( double));
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
	pp = (double ***)QEPP_ALLOC( sizeof( double), n_kpt, n_pt, 3);
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
	double * app1 = (double *)QEPP_ALLOC( sizeof( double), eps->n_pt);
	memcpy( app1, eps->x+1, eps->n_pt * sizeof( double));
	QEPP_FREE( (void *)eps->x);
	eps->x=app1;

	double ** app2 = (double **)QEPP_ALLOC( sizeof(double), eps->n_pt, eps->n_col);
	for( long int i=0; i<eps->n_pt; i++)
		memcpy( app2[i], eps->values[i+1], eps->n_col * sizeof( double));
	QEPP_FREE( (void **)eps->values);
	eps->values=app2;

//Free all unused variables
	FREE(k_pt);
	QEPP_FREE( (void *)weight);
	QEPP_FREE( (void ***)pp);
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
	if( !qepp_is_file( filename) && (v_cell == 0 || !qepp_is_file( "k.dat")))
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
		weight = (double *)QEPP_ALLOC( sizeof( double), n_kpt);//malloc( n_kpt * sizeof( double));
		memcpy(weight, app11->weight, app11->n_kpt * sizeof( double));
		FREE(app11);
	}
	else
	{
		n_kpt=k_pt->n_pt;
		weight = (double *)QEPP_ALLOC( sizeof( double), n_kpt);//malloc( n_kpt * sizeof( double));
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
	pp = (double ***)QEPP_ALLOC( sizeof( double), n_kpt, n_pt, 3);
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
	qepp_skip_comments( read, "#");
	char buffer[256];
	qepp_getline( buffer, 256, read);
	/*do {
		qepp_getline( buffer, 256, read);
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
		qepp_getline( buffer, 256, read);
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
	double * app1 = (double *)QEPP_ALLOC( sizeof( double), eps->n_pt);
	memcpy( app1, eps->x+1, eps->n_pt * sizeof( double));
	//free(eps->x);
	QEPP_FREE( (void *)eps->x);
	eps->x=app1;
/*
	//double ** app2 = malloc( eps->n_pt * sizeof( double *));
	double ** app2 = (double **)QEPP_ALLOC( sizeof(double *), eps->n_pt);
	memcpy( app2, eps->values+1, eps->n_pt * sizeof( double *));
	free(eps->values[0]);
	free(eps->values);
	eps->values=app2;
*/
	double ** app2 = (double **)QEPP_ALLOC( sizeof(double), eps->n_pt, eps->n_col);
	for( long int i=0; i<eps->n_pt; i++)
		memcpy( app2[i], eps->values[i+1], eps->n_col * sizeof( double));
	QEPP_FREE( (void **)eps->values);
	eps->values=app2;

//Free all unused variables
	FREE(k_pt);
	QEPP_FREE( (void *)weight);
	QEPP_FREE( (void ***)pp);

	*out_ptr = eps;
	QEPP_PRINT( "End eps_mod calculation\n");
	SUCCESS();
}

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
	if( !qepp_is_file( filename) && (v_cell == 0 || !qepp_is_file( "k.dat")))
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
		weight = (double *)QEPP_ALLOC( sizeof( double), n_kpt);//malloc( n_kpt * sizeof( double));
		memcpy(weight, app->weight, app->n_kpt * sizeof( double));
		FREE(app);
	}
	else
	{
		n_kpt= k_pt->n_pt;
		weight = (double *)QEPP_ALLOC( sizeof( double), n_kpt);//malloc( n_kpt * sizeof( double));
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
	pp = (double **)QEPP_ALLOC( sizeof( double), n_kpt, n_pt);
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
		qepp_getline( buffer, 256, read);
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
		qepp_getline( buffer, 256, read);
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
	double * app1 = (double *)QEPP_ALLOC( sizeof( double), eps->n_pt);
	memcpy( app1, eps->x+1, eps->n_pt * sizeof( double));
	QEPP_FREE( (void *)eps->x);
	eps->x=app1;

	double ** app2 = (double **)QEPP_ALLOC( sizeof(double), eps->n_pt, eps->n_col);
	for( long int i=0; i<eps->n_pt; i++)
		memcpy( app2[i], eps->values[i+1], eps->n_col * sizeof( double));
	QEPP_FREE( (void **)eps->values);
	eps->values=app2;

//Free all unused variables
	FREE(k_pt);
	QEPP_FREE( (void *)weight);
	QEPP_FREE( (void **)pp);

	*out_ptr = eps;
	QEPP_PRINT( "End jdos calculation\n");
	SUCCESS();
}











