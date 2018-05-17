#include <qepp/qepp_function.h>

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
			STRUCT_FREE( new_s);
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

















