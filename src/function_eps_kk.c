#include <qepp/qepp_function.h>

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
















