#include <qepp/functions.h>

extern unsigned long int TOT_MEM;
#ifdef __GSL

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define DER_NPT 1000
data_set * derivative( data_set * data, int order)
{
	data_set * res = initialize_data_set( DER_NPT);
	double start = data->x[0];
	double end = data->x[ data->n_pt-1];
	double delta = (end - start)/(DER_NPT-1);

	if( data->n_pt < DER_NPT)
	{
		gsl_spline * obj = gsl_spline_alloc( gsl_interp_polynomial, data->n_pt);
		gsl_spline_init( obj, data->x, data->y, data->n_pt);

		gsl_interp_accel * accel = gsl_interp_accel_alloc();
		for( int i=0; i<DER_NPT; i++)
			res->x[i] = start + i*delta;

		for( int i=0; i<DER_NPT; i++)
		{
			res->y[i] = gsl_spline_eval( obj, res->x[i], accel);
			//printf("%lf %lf\n",res->x[i],res->y[i]);
		}
		gsl_spline_free( obj);
		gsl_interp_accel_free( accel);
	}
	else if( data->n_pt > DER_NPT)
	{
		for( int i=0; i<data->n_pt; i++)
		{
			int app = (data->x[i] - start) / delta;
			res->y[ app] = data->y[i];
		}
	}
	else
	{
		for( int i=0; i<data->n_pt; i++)
		{
			res->x[ i] = data->x[i];
			res->y[ i] = data->y[i];
		}
	}

	if( order > 0)
		derivative_calc( res, order);

	return res;
}

void derivative_calc( data_set * data, int order)
{
	if( order > 0)
	{
		double delta = data->x[1] - data->x[0];
		data_set * app = DUPLICATE( data);
		for( int i=0; i<data->n_pt; i++)
		{
			if( i == 0)
				data->y[i] = ( app->y[i+1] - app->y[i])/delta;
			else if( i == data->n_pt-1)
				data->y[i] = ( app->y[i] - app->y[i-1])/delta;
			else
				data->y[i] = ( app->y[i] - app->y[i-1])/(2*delta) +
					( app->y[i+1] - app->y[i])/(2*delta);
		}
		FREE( app);

		return derivative_calc( data, order-1);
	}
	else
		return;
}

double average( double * data, int n_pt)
{
	double res = 0;
	for( int i=0; i<n_pt; i++)
		res+=data[i];
	res/=n_pt;

	return res;
}

double dev_std( double * data, int n_pt)
{
	double res = 0;
	double aver = average( data, n_pt);

	for( int i=0; i<n_pt; i++)
		res += pow( data[i] - aver, 2);
	res = pow( res, .5);
	res/=(n_pt-1);

	return res;
}

long int factorial( int a)
{
	if( a == 1)
		return 1;
	else if( a <= 0)
		return 0;
	return a*factorial( a-1);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int initialize_params( char * func_name, fit_params * par, data_set * data)
{
	double * x = data->x;
	double * y = data->y;
	int n_pt = data->n_pt;

	if( strcmp( func_name, "birch_murnaghan") == 0)
	{
#define V	0
#define K0	1
#define dK0	2
		par->params[V]=		0;
		par->min[V]=		0;
		par->max[V]=		10000;
		par->delta[V]=		1;

		par->params[K0]=	500;
		par->min[K0]=		0;
		par->max[K0]=		10000;
		par->delta[K0]=		100;

		par->params[dK0]=	5;
		par->min[dK0]=		1;
		par->max[dK0]=		15;
		par->delta[dK0]=	1;
		double min=1.E30;
		for( int i=0; i<n_pt; i++)
		{
			x[i] = pow( x[i], 3);
//x[i]*=3.389;
			double app = y[i];
			if( app < min)
			{
				min = app;
				par->params[V] = x[i];
			}
		}
#undef V
#undef K0
#undef dK0
	}
	else if( strcmp( func_name, "polynomial") == 0)
	{
		int n = par->nparams;
		//double deriv[n];
		double x_aver = average( data->x, data->n_pt);
//printf("xaver: %lf\n",x_aver);
		for( int i=0; i<n; i++)
		{
			data_set * app = derivative( data, n-i);
			int num = (x_aver - app->x[0])/(app->x[1] - app->x[0]);
			//deriv[i] = app->y[num];//average( app->y, app->n_pt);
//PRINT(app); printf("aver: %lf\n",deriv[i]);

			double value = app->y[num];
			for( int j=0; j<i; j++)
				value-=par->params[j]*factorial(n-j) / factorial(i)  * pow( x_aver, i-j);
			value /= factorial( n-i);
			par->params[i] = value;
			par->delta[i] = dev_std( app->y, app->n_pt)* value * pow(10, i);
			FREE( app);
			
//printf("(%d) -> V:%g D:%g\n",i,par->params[i],par->delta[i]);
			par->min[i]=-1.E8;
			par->max[i]=+1.E8;
		}
	}
	else if( strcmp( func_name, "lennard_jones") == 0)
	{
		par->params[0] = -1.E2;
		par->min[0] = -5.E3;
		par->max[0] = 1.E2;
		par->delta[0] = 1.E1;

		par->params[1] = 0;
		par->min[1] = 0;
		par->max[1] = 100;
		par->delta[1] = 1;

		double min=1.E30;
		for( int i=0; i<n_pt; i++)
		{
			double app = y[i];
			if( app < min)
			{
				min = app;
				par->params[0] = y[i];
			}
		}
	}

	return 0;
}


double chisq( func_res func, fit_params * par, data_set * data)
{
	double * x = data->x;
	double * y = data->y;
	int n_pt = data->n_pt;

	double chisq=0;
	double new_y[n_pt];
	double offset = 0;

	for( int i=0; i<n_pt; i++)
	{
		double app = func( &x[i], par);
		new_y[i] = app;
		offset += (y[i] - app);
	}
	offset /= (double)n_pt;
	for( int i=0; i<n_pt; i++)
		chisq += pow( new_y[i] + offset -y[i], 2);
	chisq /= (double)n_pt;

	par->offset = offset;

	return chisq;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
fit_params * fit( char * func_name, fit_params * par, data_set * data, double MIN_CHISQ)
{
	func_res func = (double (*)(double *, fit_params *))dlsym(0, func_name);
	//srand( (unsigned) time(NULL));

	double a_chisq;
	double n_chisq= 1.E+30;

	fit_params * n_par = DUPLICATE( par);
	fit_params * o_par = DUPLICATE( par);

	long int n=0;
	int check=0,count=0, control=0;
	double rand_n[par->nparams];
	a_chisq = chisq( func, par, data);
//printf("prova: %lf\n",o_par->delta[0]);
printf("Start chisq: %.2e\n",a_chisq);
	while( a_chisq > MIN_CHISQ && n < MAX_ITER)
	{
		for( int i=0; i<par->nparams;)
		{
			if( !check)  //rand_n[i] = (rand() % 1001)/1000.;  /*
			{
				int imax=7;
				rand_n[i]=0;
				for( int i1=0; i1<imax; i1++)	
					rand_n[i] += (rand()%10)*pow(10,i1);
				rand_n[i]/=pow(10,imax);
			}// */
			double app;
			if( control <= 7)
				app = par->params[i] + (0.5 - rand_n[i])*n_par->delta[i];
			else
				app = par->params[i] + (0.5 - rand_n[i])*o_par->delta[i]*control/2.;
			if( par->min[i] <= app && app <= par->max[i])
				n_par->params[i++] = app; 
			else
				check=0;
		}

		n_chisq = chisq( func, n_par, data);
		if( n_chisq < a_chisq)
		{
//printf("%.2e -> %.2e atn=%6.2f%% <- %li\n",a_chisq,n_chisq,(double)n/MAX_ITER*100.,n);
			a_chisq = n_chisq;
			FREE( par);
			par = DUPLICATE( n_par);
			check=1;
			control = 0;
		}
		else
		{
			count++;
			check=0;
		}
		if( count > 5000)
		{
			count = 0;
			control++;
			for( int i=0; i<par->nparams; i++)
				n_par->delta[i]/=10.;
		}
		n++;
	}
	a_chisq = chisq( func, par, data);
	if( n>= MAX_ITER)
		fprintf( stderr, "WARNING: convergence not achieved, exceeded maximum number of steps %.1e\nchisq %.2e > thr %.2e\n",
			MAX_ITER, a_chisq,MIN_CHISQ);
	else if( control > 5)
		fprintf( stderr, "WARNING: convergence not achieved, stuck in local minima\nchisq %.2e > thr %.2e \n",
			a_chisq,MIN_CHISQ);
	else
		fprintf( stderr, "Convergence achieved: chisq %.2e < thr %.2e \n", a_chisq,MIN_CHISQ);
/*
for( int i=0; i<n_pt; i++)
printf("%7.3f%9.2f -%9.2f =%.2e\n",x[i], func( x+i, par)+par->offset, y[i], func( x+i, par)+par->offset-y[i]);// */
	FREE( n_par);
	FREE( o_par);

	return par;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
double linear( double * val, fit_params * par)
{
	double y = 0;
	double x = *val;
	double a = par->params[0];
	double b = par->params[1];

	y=	a*x  + b;


	return y;
}

double quadratic( double * val, fit_params * par)
{
	double y = 0;
	double x = *val;
	double a = par->params[0];
	double b = par->params[1];
	double c = par->params[2];

	y=	a*pow( x, 2)  + b*x + c;


	return y;
}

double cubic( double * val, fit_params * par)
{
	double y = 0;
	double x = *val;
	double a = par->params[0];
	double b = par->params[1];
	double c = par->params[2];
	double d = par->params[3];

	y=	a*pow( x, 3)  + b*pow( x, 2) + c*x + d;


	return y;
}
double polynomial( double * val, fit_params * par)
{
	double y = 0;
	double x = *val;
	for( int i=0; i<par->nparams; i++)
		y+=par->params[i] * pow( x, par->nparams-i);

	return y;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// E[V] = E0 + K0/dK0 * V * [ ((V / V0) ^ dK0) / (dk0 - 1) + 1 ] - K0 * V0 / (dK0 - 1)
double birch_murnaghan(double * val, fit_params * par)
{
	double y   = 0;
	double V   = *val;
	double V0  = par->params[0];
	double K0  = par->params[1]/RY_TO_KBAR;
	double dK0 = par->params[2];
	double E0  = 0;

	y=	E0 -
		K0 * V0 / (dK0 - 1) +
		(pow( V0/V, dK0)/( dK0 - 1) + 1) * K0/ dK0 * V;

	return y;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
double lennard_jones( double * val, fit_params * par)
{
	double y	= 0;
	double r	= *val;
	double fi_e	= par->params[0];
	double r_e	= par->params[1];

	y=	fi_e * ( pow( r_e/r, 12) - 2 * pow( r_e/r, 6));

	return y;
}

#endif //__GSL



























