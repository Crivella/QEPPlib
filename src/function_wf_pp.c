#include <qepp/qepp_function.h>

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
							mp_sum( &cc2, MPI_COMM_WORLD, 1);
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
	if( qepp_is_file( (char *)filename))
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
		qepp_fscanf_double( out, &app);
		start_k = app-1;
		qepp_fscanf_double( out, &app);
		start_b1 = app;
	}
	else
		out = fopen( filename, "w");
	if( out == NULL)
		FAIL( FAIL, "Cannout open file matrixelements for write");

	double complex * app = (double complex *)QEPP_ALLOC( sizeof( double complex), 3);

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

	QEPP_FREE( (void *)app);

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

	res = (double complex *)QEPP_ALLOC( sizeof( double complex), n_bnd);
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
		klist = QEPP_ALLOC( sizeof( long int), n_kpt);
		for( long int i=0; i<n_kpt; i++)
			klist[i] = i;
	}		
	res = (double complex *)QEPP_ALLOC( sizeof( double complex), n_bnd);
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

	//QEPP_FREE( (void *)app_v);
	QEPP_FREE( (void *)klist);

	*out_ptr = res;
	SUCCESS();
}
























