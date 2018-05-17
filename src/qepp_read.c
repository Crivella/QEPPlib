#include <qepp/qepp_read.h>
#ifdef __LIBXML2
#include <libxml/parser.h>
#include <qepp/wrapper_libxml.h>
//static evc  * read_evc_xml( xmlDocPtr document, xmlNodePtr root, xmlNodePtr node);
//static evc  * read_evc_dat( FILE *, int);
static errh * read_egv_xml( const char * filename, data_file * res);
static errh * read_gkv_xml( const char * filename, wfc * res);
static errh * read_gkv_dat( const char * filename, wfc * res);
static errh * read_data_file_old( xmlNodePtr root, char * path, data_file * res);
static errh * read_data_file_new( xmlNodePtr root, char * path, data_file * res);
#endif // __LIBXML2


#define TEST_ARGS() \
	if( out_ptr == NULL) \
		FAIL( NULL_OUT, " "); \
	*out_ptr = NULL; \
	if( filename == NULL) \
		FAIL( NULL_IN, " "); \
	if( ! qepp_is_file( filename)) \
		FAIL( OPEN_IN_FAIL, "%s is not a file.", filename); \
	FILE * read = fopen( filename, "r"); \
	if( read == NULL) \
		FAIL( OPEN_IN_FAIL, "%s", filename);
	

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//READ
errh * read_nscf_md( const char * filename, nscf_md ** out_ptr)
{
	TEST_ARGS();

	QEPP_PRINT( "Reading nscf metadata from \"%s\"\n", filename);

	long int lines = QEPP_GET_FILE_LINES( read);
	bool spin_orbit=false;
	long int n_kpt;
	int n_bnd;
	double e_fermi;
	char formula[128];
	double vc;
	double n_el;	

	if( qepp_find_string( "spin", read, 0) > 0)
		spin_orbit = true;
	if( qepp_get_value( "number of k points", read, "=", 0, R_LNT, &n_kpt))
		FAIL( FAIL, "Cannot read n_kpt");
	if( qepp_get_value( "number of Kohn-Sham states", read, "=", 0, R_INT, &n_bnd))
		FAIL( FAIL, "Cannot read n_bnd");
	if( qepp_get_value( "Fermi energy", read, 0 , /*md->lines_pos[md->lines-200]*/0, R_FLT, &e_fermi))
	{
		int check = 1;
		char * buffer = qepp_change_file( filename, "scf_1.out");
		FILE * app_f = NULL;
		if( qepp_is_file( buffer))
			app_f = fopen( buffer, "r");
		free( buffer);
		if( app_f != NULL)
		{
			if( !qepp_get_value( "Fermi energy", app_f, 0, 0, R_FLT, &e_fermi))
				check = 0;
			fclose( app_f);
		}
		if( check)
		{
			e_fermi = NAN;
			WARN( "Cannot read e_fermi");
		}
	}
	if( qepp_get_value( ".save", read, "/", 0, R_STR, formula))
		WARN( "Cannot read material name");
	if( qepp_get_value( "unit-cell volume", read, "=", 0, R_FLT, &vc))
	{
		vc = 0;
		WARN( "Cannot read v_cell");
	}
	if( qepp_get_value( "number of electrons", read, "=", 0, R_FLT, &n_el))
	{
		n_el = NAN;
		WARN( "Cannot read n_el");
	}
	fclose(read);

	nscf_md * md = initialize_nscf_md();
	md->lines = lines;
	md->spin_orbit = spin_orbit;
	md->n_kpt = n_kpt;
	md->n_bnd = n_bnd;
	md->e_fermi = e_fermi;
	strcpy( md->formula, formula);
	md->vc = vc;
	md->n_el = n_el;

	*out_ptr = md;
	//QEPP_PRINT( "Found %li kpt, %d bands\n", n_kpt, n_bnd);
	//QEPP_PRINT( "Found %li kpt, %d bands\n", md->n_kpt, md->n_bnd);
	SUCCESS();
}

errh * read_nscf_data( const char * filename, nscf_data ** out_ptr)
{
	TEST_ARGS();

	int bands=0;
	char buffer[256];
	QEPP_PRINT("Reading nscf data from \"%s\"\n", filename);

	if(strstr(filename,"bands") != NULL)
		bands=1;

	nscf_md * md;
	READ( filename, &md);
	if( md == NULL)
		FAIL( FAIL, "Failed to get nscf_md");
	nscf_data * data = initialize_nscf_data( md->n_kpt, md->n_bnd);
	
	data->md = md;
	
	long int count=0;
	//double trash;
	long int pos = qepp_find_string("End of band structure calculation",read,0);
	if(pos == -1)
	{
		pos = qepp_find_string("End of self-consistent calculation",read,0);
		if(pos == -1)
			FAIL( FAIL, "Can't identify start of band data in file");
	}
	fseek(read,pos,SEEK_SET);
	qepp_getline(buffer,256,read);
	while(!feof(read) && count < data->n_kpt)
	{
		for (int i=0; i<3; i++)
			qepp_fscanf_double(read,&data->kpt[count][i]);
		//if(!bands)
			//qepp_fscanf_double(read,&trash);
		for (int i=0; i< data->n_bnd; i++)
		{
			qepp_fscanf_double(read,&data->energies[count][i]);
			if( i==0 && data->energies[count][i] > 100)
				i--;
		}
		if(!bands)
			for (int i=0; i< data->n_bnd; i++)
				qepp_fscanf_double(read,&data->occup[count][i]);
		count++;
	}
	pos = qepp_find_string("number of k points",read,0);
	if(pos == -1)
		FAIL( FAIL, " ");
	fseek(read,pos,SEEK_SET);
	qepp_getline(buffer,256,read);
	count=0;
	int count_2=0;
	char c;
	double weight;
	if ( df != NULL)
		for( long int i=0; i<data->n_kpt; i++)
		{
			weight = df->weight[i];
			data->tot_weight += weight;
			data->weight[i] = weight;
		}
		

#ifdef __LIBXML2
	if( qepp_is_file( "data-file.xml"))
	{
		QEPP_PRINT("Reading weights from data-file.xml\n");
		//data_file * df;
		READ( "data-file.xml", &df);
		for( long int i=0; i<data->n_kpt; i++)
		{
			weight = df->weight[i];
			data->tot_weight += weight;
			data->weight[i] = weight;
		}
	}
	else
#endif

	while(count < data->n_kpt)
	{
		while(count_2 < 1)
		{
			c = fgetc(read);
			if(c == '=')
				count_2++;
		}
		count_2=0;
		for( int i=0; i<3; i++)
			qepp_fscanf_double( read, &data->kpt[count][i]);
		qepp_fscanf_double(read,&data->weight[count]);
		data->tot_weight+=data->weight[count++];
	}
	fclose(read);

	*out_ptr = data;
	QEPP_PRINT("Read %li kpoints with tot_weight %lf\n",data->n_kpt,data->tot_weight);

	SUCCESS();
}

errh * read_band_data( const char * filename, band_data ** out_ptr)
{
	TEST_ARGS();

	band_data * data;
	QEPP_PRINT("Reading band data from \"%s\"\n", filename);

	double app = 0;
	int n_bnd = 0;
	long int n_kpt = 0;
	if( qepp_fscanf_double( read, &app) == EOF)
		FAIL( FAIL, "Empty or unreadable file");
	n_bnd = floor( app);
	if( qepp_fscanf_double( read, &app) == EOF)
		FAIL( FAIL, "Empty or unreadable file");
	n_kpt = floor( app);
	

	data = initialize_band_data( n_bnd, n_kpt);

	QEPP_PRINT("nks=\t%li\nnbnd=\t%d\n",n_kpt,n_bnd);

	for( long int i=0; i<n_kpt; i++)
	{
		for( int j=0; j<3; j++)
			qepp_fscanf_double( read, &data->kpt[i][j]);
		for( int j=0; j<n_bnd; j++)
			qepp_fscanf_double( read, &data->bands[i][j]);
	}

	*out_ptr = data;
	SUCCESS();
}

errh * read_band_pp( const char * filename, band_pp ** out_ptr)
{
	TEST_ARGS();

	QEPP_PRINT("Reading band pp from \"%s\"\n", filename);

	double app;
	qepp_fscanf_double( read, &app);
	int n_bnd = (int)floor(app);
	qepp_fscanf_double( read, &app);
	long int n_kpt = (long int)floor(app);

	band_pp * data = initialize_band_pp( n_kpt, n_bnd);

	int n_bnd_occ=0;
	for( long int i=0; i<n_kpt; i++)
	{
		for( int j=0; j<3; j++)
			qepp_fscanf_double( read, &data->kpt[i][j]);
		qepp_fscanf_double( read, &app);
		n_bnd_occ = (int)floor(app);
		data->n_bnd_occ[i] = n_bnd_occ;
		int lim = n_bnd - n_bnd_occ;
		for( int j=0; j<3; j++)
		{
			qepp_fscanf_double( read, &app); //Trash
			for( int k=0; k<lim; k++)
				for( int n=0; n<n_bnd_occ; n++)
					qepp_fscanf_double( read, &data->pp[i][n][k][j]);
		}
	}
	QEPP_PRINT("Read %li kpt with %d bands\n", n_kpt,n_bnd);

	*out_ptr = data;
	SUCCESS();
}

errh * read_spin_data( const char * filename, spin_data ** out_ptr)
{
	TEST_ARGS();

	spin_data * data;
	FILE * md = fopen( "spinore.out", "r");
	if( md == NULL)
		FAIL( FAIL, "Cannot open file spinore.out");
	QEPP_PRINT("Reading band data from \"%s\"\n", filename);

	int n_bnd = 0;
	long int n_kpt = 0;
	if( qepp_get_value( "nks", md, "s", 0, R_LNT, &n_kpt))
		FAIL( FAIL , "Cannot read nks");
	if( qepp_get_value( "nbndsi", md, "=", 0, R_INT, &n_bnd))
		FAIL( FAIL, "Cannot read nbndsi");
	QEPP_PRINT("nks=\t%li\nnbnd=\t%d\n",n_kpt,n_bnd);

	data = initialize_spin_data( n_bnd, n_kpt);

	for( long int i=0; i<n_kpt; i++)
	{
		//double dump;
		for( int j=0; j<n_bnd; j++)
		{
			long int app1=0;
			int app2=0;
			/*double app3=0;
			qepp_fscanf_double( read, &app3);
			app1 = floor(app3+0.5);
			qepp_fscanf_double( read, &app3);
			app2 = floor(app3+0.5);*/
			if( fscanf( read, "%li", &app1)) {}
			if( fscanf( read, "%d", &app2)) {}
			app1--; app2--;
			if(app1 != i || app2 != j) {
				WARN( "Warning fileenum not matching cycle %li <> %li   %d <> %d",app1,i,app2,j); getchar();}
			for( int k=0; k<2; k++)
				qepp_fscanf_double( read, &data->cd[i][j][k]);
			for( int k=0; k<6; k++)
				qepp_fscanf_double( read, &data->m[i][j][k/2][k%2]);
			//qepp_fscanf_double( read, &data->m[i][j][3][0]);
			for( int k=0; k<2; k++)
				qepp_fscanf_double( read, &data->m_tot[i][j][k]);
			for( int k=0; k<2; k++)
				qepp_fscanf_double( read, &data->elicity[i][j][k]);
			for( int k=0; k<2; k++)
				qepp_fscanf_double( read, &data->chirality[i][j][k]);
			//for( int k=0; k<2; k++)
			//	qepp_fscanf_double( read, &dump/*&data->chir2[i][j][k]*/);
		}
	}

	*out_ptr = data;
	SUCCESS();
}

errh * read_opt_data( const char * filename, opt_data ** out_ptr, char * comments)
{
	TEST_ARGS();

	opt_data * data;
	QEPP_PRINT("Reading opt data from \"%s\"\n", filename);

	long int lines = QEPP_GET_FILE_LINES(read,comments);
	long int count=0;
	int col = QEPP_GET_FILE_COL(read,comments)-1;
	char c;
	QEPP_PRINT("Reading %li lines with %d columns\n",lines,col+1);

	data = initialize_opt_data( lines, col);
	while(!feof(read) && count < lines)
	{
		c=fgetc(read);
		if( comments != NULL)
		{
			while( strchr(comments,c) != NULL)
			{
				while( c != '\n')
					c=fgetc(read);
				c=fgetc(read);
			}
		}
		if(!feof(read))
			fseek( read, -1 , SEEK_CUR);

		qepp_fscanf_double(read,&data->x[count]);
		for( int i=0; i<col; i++)
			qepp_fscanf_double(read,&data->values[count][i]);
		count++;
		if( count % 500000 == 0)
			QEPP_PRINT("Read %li lines...\n",count);
	}

	fclose(read);
	QEPP_PRINT("Read %li lines with %i columns\n",count, col+1);

	*out_ptr = data;
	SUCCESS();
}

errh * read_m_elem( const char * filename, m_elem ** out_ptr, char * comments)
{
	TEST_ARGS();

	m_elem * data;
	char fname_app[128];
	char * fname;
	if( !qepp_is_file( "nscf_1.out"))
	{
		if( !qepp_is_file( "bands_1.out"))
			fname = qepp_get_file( "nscf_1.out", ".out");
		else
		{
			strcpy( fname_app, "bands_1.out");
			fname = fname_app;
		}
	}
	else
	{
		strcpy( fname_app, "nscf_1.out");
		fname = fname_app;
	}

	nscf_data * app;
	READ( fname, &app);
	if( app == NULL)
		FAIL( FAIL, "Cannot open nscf data");
	long int n_kpt = app->n_kpt;
	int n_bnd = app->md->n_bnd;
	QEPP_PRINT("Reading matrixelements from \"%s\"\n", filename);

	long int lines = QEPP_GET_FILE_LINES(read,comments);
	long int count=0;
	char c;

	data = initialize_m_elem( n_kpt, n_bnd);
	QEPP_PRINT("Memory allocated...\n");
	long int k_app = 0;
	int vb = 0, cb = 0;
	while(!feof(read) && count < lines)
	{
		c=fgetc(read);
		if( comments != NULL)
		{
			if( strchr(comments,c) != NULL)
			{
				while( c != '\n')
					c=fgetc(read);
				fgetc(read);
			}
		}
		if(!feof(read))
			fseek( read, -1 , SEEK_CUR);

		if( fscanf( read, "%li %d %d", &k_app, &vb, &cb)) {}
		k_app--; vb--; cb--;
		if (fscanf( read, "%lf %lf %lf %lf %lf\n", &data->pp[k_app][vb][cb][0], &data->pp[k_app][vb][cb][1],
			&data->pp[k_app][vb][cb][2],
			&data->gap[k_app][vb][cb], &data->df[k_app][vb][cb])) {}

		count++;
		if( count % 1000000 == 0)
			QEPP_PRINT("Read %li lines...\n",count);
	}

	fclose(read);
	QEPP_PRINT("Read %li lines with %li kpt and %d bands\n",count, n_kpt, n_bnd);

	*out_ptr = data;
	SUCCESS();
}

errh * read_fit_params( const char * filename, fit_params ** out_ptr) { SUCCESS();}
errh * read_data_set( const char * filename, data_set ** out_ptr) { SUCCESS();}

errh * read_pdos_data( const char * filename, pdos_data ** out_ptr, char * comments)
{
	TEST_ARGS();

	pdos_data * data = NULL;
	char buffer[256];
	int n_states=0;

	char * nscf_f = qepp_get_file( "nscf_1.out", ".out");
	QEPP_PRINT("Acquiring nscf output file data from: %s...\n", nscf_f);
	nscf_data * nscf;
	READ( nscf_f, &nscf);
	QEPP_PRINT("...done\n");

	long int pos = qepp_find_string("state #",read,0);
	fseek( read, pos, SEEK_SET);
	qepp_getline(buffer,256,read);
	while( strstr( buffer, "state #") != NULL)
	{
		n_states++;
		qepp_getline(buffer,256,read);
	}
	QEPP_PRINT("%li kpt, %d bands, %d states found\n",nscf->n_kpt,nscf->n_bnd,n_states);

	data = initialize_pdos_data( nscf->n_kpt, nscf->n_bnd, n_states);
	data->nscf = STRUCT_DUPL( nscf);

	QEPP_PRINT("Reading state data...\n");
	fseek( read, pos, SEEK_SET);
	int state_app;
	int app=1;
	pdos_state * p_app;
	char c;
	int cc=0;
	for( int i=0; i<n_states; i++)
	{
		p_app = data->states[i];
		qepp_getline( buffer, 256, read);
		cc = 0;
//QEPP_PRINT( "%s\n", buffer+cc);
		while( (c = buffer[cc++]) != '#') {}
		app *= sscanf( buffer+cc, "%d", &state_app);
//QEPP_PRINT( "%s   %d\n", buffer+cc, state_app);
		while( (c = buffer[cc++]) != 'm') {}
		app *= sscanf( buffer+cc, "%d", &p_app->atom_n);
//QEPP_PRINT( "%s   %d\n", buffer+cc, p_app->atom_n);
		while( (c = buffer[cc++]) != '(') {}
		app *= sscanf( buffer+cc, "%s)", p_app->name);
//QEPP_PRINT( "%s   %s\n", buffer+cc, p_app->name);
		while( (c = buffer[cc++]) != 'c') {}
		app *= sscanf( buffer+cc, "%d", &p_app->wfc_n);
//QEPP_PRINT( "%s   %d\n", buffer+cc, p_app->wfc_n);
		while( (c = buffer[cc++]) != '=') {}
		app *= sscanf( buffer+cc, "%lf", &p_app->j);
//QEPP_PRINT( "%s   %.1lf\n", buffer+cc, p_app->j);
		while( (c = buffer[cc++]) != '=') {}
		app *= sscanf( buffer+cc, "%d", &p_app->l);
//QEPP_PRINT( "%s   %d\n", buffer+cc, p_app->l);
		while( (c = buffer[cc++]) != '=') {}
		app *= sscanf( buffer+cc, "%lf", &p_app->m_j);
//QEPP_PRINT( "%s   %.1lf\n", buffer+cc, p_app->m_j);
		if( app == 0)
		{
			STRUCT_FREE( data);
			FAIL( FAIL, "Failed to read state %d...\n",i+1);
		}
	}
	QEPP_PRINT("...done\n");

	QEPP_PRINT("Reading pdos data...\n");
	long int k_app=0;
	double v_app[3];
	int b_app, b_app_r;
	double en_app;
	int n_c;
	int c_app;
	app=1;
	while( k_app < nscf->n_kpt)
	{
		while( fgetc(read) != 'k' && !feof(read)) {}
		for( int i=0; i<3; i++)
			qepp_fscanf_double( read, &v_app[i]);
		if( delta_k( nscf->kpt[k_app],v_app) > 0.001)
		{
			STRUCT_FREE( data);
			FAIL( FAIL, "Mismatch between pdos and nscf file for k_pt coordinates");
		}
		b_app=0;
		while( b_app < nscf->n_bnd)
		{
			while( fgetc(read) != '(' && !feof(read)) {}
			app *= fscanf( read, "%d", &b_app_r); b_app_r--;
			qepp_fscanf_double( read, &en_app);
			qepp_getline( buffer, 256, read);
			if( fabs( en_app - nscf->energies[k_app][b_app]) > 2.E-3)
			{
				STRUCT_FREE( data);
				FAIL( FAIL, "Mismatch between pdos and nscf file for k_pt #%li enrgies at band #%d...\n", k_app, b_app);
			}
			n_c = 0;
			long int pos_app = ftell(read);
			while( (c = fgetc(read)) != '|')
				if( c == '[')
					n_c++;
			fseek(read,pos_app,SEEK_SET);
			c_app=0;
			while( c_app < n_c)
			{
				while( ( c = fgetc(read)) != '=' && c != '+') {}
				double r_app;
				qepp_fscanf_double( read, &r_app);
				double n_app;
				qepp_fscanf_double( read, &n_app);
				int n = n_app-1;
				data->pdos[k_app][b_app][n] = r_app;
				c_app++;
			}
			
			b_app++;
		}
		k_app++;
	}

	*out_ptr = data;
	SUCCESS();
}

errh * read_pdos_state( const char * filename, pdos_state ** out_ptr)
{
	SUCCESS();
}

#ifdef __LIBXML2
errh * read_data_file( const char * filename, data_file ** out_ptr)
{
	*out_ptr = NULL;
	data_file * res = NULL;
	if( out_ptr == NULL)
		FAIL( NULL_OUT, " ");
	if( filename == NULL)
		FAIL( NULL_IN, " ");
	char fname[1024];
	char * path;
	if( outdir[0] == '\0' || prefix[0] == '\0')
		path = get_tmp_path( );
	else
	{
		path = malloc( 1024 * sizeof( char));
		sprintf( path, "%s/%s.save/", outdir, prefix);
	}
	//QEPP_PRINT("path = %s\n", path);
	//if( path == NULL)
	//	FAIL( FAIL, "Can't find tmp directory");
	//sprintf( fname, "%s/%s", path, filename);
	strcpy( fname, filename);
//printf("fname: %s\npath: %s\n\n", fname,path);
	/*FILE * read = fopen(fname,"r");
	if( read == NULL)
		FAIL( OPEN_IN_FAIL, "%s", fname);
	fclose( read);*/

	if( path == NULL)
	{
		path = malloc( 12);
		strcpy( path, "./");
	}
	sprintf( fname, "%s%s", path, filename);

	int check=0;
	res = initialize_data_file();
	if( !qepp_is_file( fname))
	{
		sprintf( fname, "%s%s", path, "data-file-schema.xml");
		if ( !qepp_is_file( fname))
			check = 1;
		res->version = 6*1E6 + 2*1E3 + 0*1E0;
	}
	else
		res->version = 6*1E6 + 1*1E3 + 0*1E0;

QEPP_PRINT( "VERSION: %li\n\n", res->version);

	xmlDocPtr	document;
	xmlNodePtr	root;

	document = xmlReadFile( fname, NULL, 0);
	root = xmlDocGetRootElement( document);

	

	//Check espresso version
	if( res->version < 6*1E6 + 2*1E3 + 0*1E0)
	{
		if( parse_errh( read_data_file_old( root, path, res)) == FAIL)
			check = 1;
	}
	else //if( xmlStrcmp( root->name, (const xmlChar *)"Root"))
	{
		if( parse_errh( read_data_file_new( root, path, res)) == FAIL)
			check = 1;
	}

	

	xmlFreeDoc( document);
	free( path);

	if( check)
	{
		STRUCT_FREE( res);
		FAIL( FAIL, "Failed to read data-file");
	}

	*out_ptr = res;
	SUCCESS();
}


static errh * read_data_file_new( xmlNodePtr root, char * path, data_file * res)
{
	res->n_kpt=10;
	res->lp=10.2;
	res->lp_t = BOHR_TO_M;
	res->a[0][0] = -1; res->a[0][1] = 0; res->a[0][2] = 1; 
	res->a[1][0] =  0; res->a[1][1] = 1; res->a[1][2] = 1; 
	res->a[2][0] = -1; res->a[2][1] = 1; res->a[2][2] = 0;
	res->a_t=1;
	res->b[0][0] = -1; res->b[0][1] =-1; res->b[0][2] = 1; 
	res->b[1][0] =  1; res->b[1][1] = 1; res->b[1][2] = 1; 
	res->b[2][0] = -1; res->b[2][1] = 1; res->b[2][2] =-1;
	res->b_t = 2*PI/(res->lp*res->lp_t);

	res->c_dim[0]=1; res->c_dim[1]=1; res->c_dim[2]=1; 
	strcpy( res->a_n, "alat");
	res->kpt = (double **)QEPP_ALLOC( sizeof( double), res->n_kpt, 3);
	res->weight = (double *)QEPP_ALLOC( sizeof( double), res->n_kpt);
	//res->egval_link = (char **)QEPP_ALLOC( sizeof( char), res->n_kpt, 256);
	res->egvec_link = (char **)QEPP_ALLOC( sizeof( char), res->n_kpt, 256);
	res->wfc_link = (char **)QEPP_ALLOC( sizeof( char), res->n_kpt, 256);
	res->wfc_link2 = (char **)QEPP_ALLOC( sizeof( char), res->n_kpt, 256);

	res->kpt[0][0]=.125; res->kpt[0][1]=.125; res->kpt[0][2]=.125;
	res->kpt[1][0]=.125; res->kpt[1][1]=.125; res->kpt[1][2]=.375;
	res->kpt[2][0]=.125; res->kpt[2][1]=.125; res->kpt[2][2]=.625;
	res->kpt[3][0]=.125; res->kpt[3][1]=.125; res->kpt[3][2]=.875;
	res->kpt[4][0]=.125; res->kpt[4][1]=.375; res->kpt[4][2]=.375;
	res->kpt[5][0]=.125; res->kpt[5][1]=.375; res->kpt[5][2]=.625;
	res->kpt[6][0]=.125; res->kpt[6][1]=.375; res->kpt[6][2]=.825;
	res->kpt[7][0]=.125; res->kpt[7][1]=.625; res->kpt[7][2]=.625;
	res->kpt[8][0]=.375; res->kpt[8][1]=.375; res->kpt[8][2]=.375;
	res->kpt[9][0]=.375; res->kpt[9][1]=.375; res->kpt[9][2]=.625;

	res->weight[0]=1;
	res->weight[1]=3;
	res->weight[2]=3;
	res->weight[3]=3;
	res->weight[4]=3;
	res->weight[5]=6;
	res->weight[6]=6;
	res->weight[7]=3;
	res->weight[8]=1;
	res->weight[9]=3;

	for( long int i=0; i<res->n_kpt; i++)
	{
		sprintf( res->wfc_link[i], "wfc%li.dat", i+1);
		sprintf( res->wfc_link2[i], "wfc%li.dat", i+1);
		sprintf( res->egvec_link[i], "wfc%li.dat", i+1);
	}
	//FAIL( FAIL, "Non-recognized version of espresso (must be <= 6.1)");

	SUCCESS();
}


static errh * read_data_file_old( xmlNodePtr root, char * path, data_file * res)
{
	xmlNodePtr	node, app;
	xmlChar		* key;

	char buffer[128];
	////////////////////////////////////////////////////////////////////////////////////////////////////
	//Read CELL
	app = qepp_libxml_find_node( "CELL", root);

	node = qepp_libxml_find_node( "BRAVAIS_LATTICE", app);
	assert( !qepp_libxml_get_node_value( &res->bravais, node, R_STR, 1));

	node = qepp_libxml_find_node( "LATTICE_PARAMETER", app);
	key = xmlGetProp( node, (const xmlChar *)"UNITS");
	if( strcasestr( (char *)key, "bohr") != NULL)
		res->lp_t = BOHR_TO_M;
	xmlFree( key);
	assert( !qepp_libxml_get_node_value( &res->lp, node, R_LNF, 1));

	node = qepp_libxml_find_node( "CELL_DIMENSIONS", app);
	assert( !qepp_libxml_get_node_value( &res->c_dim, node, R_LNF, 6));

	node = qepp_libxml_find_node( "DIRECT_LATTICE_VECTORS/UNITS_FOR_DIRECT_LATTICE_VECTORS", app);
	assert( !qepp_libxml_get_node_attr( &res->a_n, "UNITS", node, R_STR, 1));
	if( !strcmp( res->a_n, "Bohr"))
		res->a_t = BOHR_TO_M;

	for(int i=0; i<3; i++)
	{
		sprintf( buffer, "DIRECT_LATTICE_VECTORS/a%d", i+1);
		node = qepp_libxml_find_node( buffer, app);
		assert( !qepp_libxml_get_node_value( &res->a[i][0], node, R_LNF, 3));
	}

	node = qepp_libxml_find_node( "RECIPROCAL_LATTICE_VECTORS/UNITS_FOR_RECIPROCAL_LATTICE_VECTORS", app);
	assert( !qepp_libxml_get_node_attr( &res->b_n, "UNITS", node, R_STR, 1));
	if( !strcmp( res->b_n, "2 pi / a"))
		res->b_t = 2*PI/(res->lp*res->lp_t);

	for(int i=0; i<3; i++)
	{
		sprintf( buffer, "RECIPROCAL_LATTICE_VECTORS/b%d", i+1);
		node = qepp_libxml_find_node( buffer, app);
		assert( !qepp_libxml_get_node_value( &res->b[i][0], node, R_LNF, 3));
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//Read BRILLOUIN_ZONE
	app = qepp_libxml_find_node( "BRILLOUIN_ZONE", root);

	node = qepp_libxml_find_node( "NUMBER_OF_K-POINTS", app);
	assert( !qepp_libxml_get_node_value( &res->n_kpt, node, R_LNT, 1));

	res->kpt = (double **)QEPP_ALLOC( sizeof( double), res->n_kpt, 3);
	res->weight = (double *)QEPP_ALLOC( sizeof( double), res->n_kpt);
	//res->egval_link = (char **)QEPP_ALLOC( sizeof( char), res->n_kpt, 256);
	res->egvec_link = (char **)QEPP_ALLOC( sizeof( char), res->n_kpt, 256);
	res->wfc_link = (char **)QEPP_ALLOC( sizeof( char), res->n_kpt, 256);


	node = qepp_libxml_find_node( "UNITS_FOR_K-POINTS", app);
	key = xmlGetProp( node, (const xmlChar *)"UNITS");
	if( !xmlStrcmp( key, (const xmlChar *)"2 pi / a"))
		res->kpt_t = 2*PI/(res->lp*res->lp_t);
	xmlFree( key);

	for( long int i=0; i<res->n_kpt; i++)
	{
		sprintf( buffer, "K-POINT.%li", i+1);
		node = qepp_libxml_find_node( buffer, app);
		assert( !qepp_libxml_get_node_attr( &res->kpt[i][0], "XYZ", node, R_LNF, 3));
		assert( !qepp_libxml_get_node_attr( &res->weight[i], "WEIGHT", node, R_LNF, 1));
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//Read BAND_STRUCTURE_INFO
	app = qepp_libxml_find_node( "BAND_STRUCTURE_INFO", root);

	node = qepp_libxml_find_node( "NUMBER_OF_BANDS", app);
	assert( !qepp_libxml_get_node_value( &res->n_bnd, node, R_INT, 1));

	res->egv = (double **)QEPP_ALLOC( sizeof( double), res->n_kpt, res->n_bnd);
	res->occ = (double **)QEPP_ALLOC( sizeof( double), res->n_kpt, res->n_bnd);


	node = qepp_libxml_find_node( "NUMBER_OF_ELECTRONS", app);
	assert( !qepp_libxml_get_node_value( &res->n_el, node, R_LNF, 1));

	node = qepp_libxml_find_node( "NUMBER_OF_SPIN_COMPONENTS", app);
	assert( !qepp_libxml_get_node_value( &res->n_spin, node, R_INT, 1));

	if( res->n_spin == 4)
		res->wfc_link2 = (char **)QEPP_ALLOC( sizeof( char), res->n_kpt, 256);

	if( res->n_spin == 2)
	{
		res->wfc_link2 = (char **)QEPP_ALLOC( sizeof( char), res->n_kpt, 256);
		//res->egval_link2= (char **)QEPP_ALLOC( sizeof( char), res->n_kpt, 256);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//Read EIGENVALUES
	app = qepp_libxml_find_node( "EIGENVALUES", root);

	for( long int i=0; i<res->n_kpt; i++)
	{
		sprintf( buffer, "K-POINT.%li/K-POINT_COORDS", i+1);
		node = qepp_libxml_find_node( buffer, app);
		double a[3];
		assert( !qepp_libxml_get_node_value( &a[0], node, R_LNF, 3));
		for( int n=0; n<3; n++)
			if( a[n] != res->kpt[i][n])
				WARN( "kpt coordinates in eigenvalue list differ from BZ list");

		if( res->n_spin != 2)
		{
			sprintf( buffer, "K-POINT.%li/DATAFILE", i+1);
			node = qepp_libxml_find_node( buffer, app);
			assert( !qepp_libxml_get_node_attr( &buffer, "iotk_link", node, R_STR, 1));
			char name[256];
			//sprintf( res->egval_link[i], "%s%s", path, buffer);
			sprintf( name, "%s%s", path, buffer);
			read_egv_xml( name, res);
		}
		else
		{	
			sprintf( buffer, "K-POINT.%li/DATAFILE.1", i+1);
			node = qepp_libxml_find_node( buffer, app);
			assert( !qepp_libxml_get_node_attr( &buffer, "iotk_link", node, R_STR, 1));
			//sprintf( res->egval_link[i], "%s%s", path, buffer);
			char name[256];
			sprintf( name, "%s%s", path, buffer);
			read_egv_xml( name, res);

			sprintf( buffer, "K-POINT.%li/DATAFILE.2", i+1);
			node = qepp_libxml_find_node( buffer, app);
			assert( !qepp_libxml_get_node_attr( &buffer, "iotk_link", node, R_STR, 1));
			//sprintf( res->egval_link2[i], "%s%s", path, buffer);
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	//Read EIGENVECTORS
	app = qepp_libxml_find_node( "EIGENVECTORS", root);

	for( long int i=0; i<res->n_kpt; i++)
	{
		sprintf( buffer, "K-POINT.%li/GK-VECTORS", i+1);
		node = qepp_libxml_find_node( buffer, app);
		assert( !qepp_libxml_get_node_attr( &buffer, "iotk_link", node, R_STR, 1));
		sprintf( res->egvec_link[i], "%s%s", path, buffer);
	
		if( res->n_spin > 1)
		{
			sprintf( buffer, "K-POINT.%li/WFC.1", i+1);
			node = qepp_libxml_find_node( buffer, app);
			assert( !qepp_libxml_get_node_attr( &buffer, "iotk_link", node, R_STR, 1));
			sprintf( res->wfc_link[i], "%s%s", path, buffer);


			sprintf( buffer, "K-POINT.%li/WFC.2", i+1);
			node = qepp_libxml_find_node( buffer, app);
			assert( !qepp_libxml_get_node_attr( &buffer, "iotk_link", node, R_STR, 1));
			sprintf( res->wfc_link2[i], "%s%s", path, buffer);
		}
		else
		{
			sprintf( buffer, "K-POINT.%li/WFC", i+1);
			node = qepp_libxml_find_node( buffer, app);
			assert( !qepp_libxml_get_node_attr( &buffer, "iotk_link", node, R_STR, 1));
			sprintf( res->wfc_link[i], "%s%s", path, buffer);
		}
	}

	SUCCESS();
}
#endif //__LIBXML2

errh * read_wfc( const char * filename, wfc ** out_ptr)
{
	if( out_ptr == NULL)
		FAIL( NULL_OUT, " ");
	if( filename == NULL)
		FAIL( NULL_IN, " ");
	FILE * read = fopen(filename,"r");
	if( read == NULL)
		FAIL( OPEN_IN_FAIL, "%s", filename);
	fclose( read);

	if( strstr( filename, ".xml") != NULL)		return read_wfc_xml( filename, out_ptr);
	else if( strstr( filename, ".dat") != NULL)	return read_wfc_dat( filename, out_ptr);
	else 						FAIL( FAIL, "Non recognized extension for wfc file");
}

/*
errh * read_gkv( const char * filename, gkv ** out_ptr)
{
	if( out_ptr == NULL)
		FAIL( NULL_OUT, " ");
	if( filename == NULL)
		FAIL( NULL_IN, " ");
	FILE * read = fopen(filename,"r");
	if( read == NULL)
		FAIL( OPEN_IN_FAIL, "%s", filename);
	fclose( read);

	if( strstr( filename, ".xml") != NULL)		return read_gkv_xml( filename, out_ptr);
	else if( strstr( filename, ".dat") != NULL)	return read_gkv_dat( filename, out_ptr);
	else 						FAIL( FAIL, "Non recognized extension for gkvectors file");
}
*/

/*
errh * read_egv( const char * filename, egv ** out_ptr)
{
	if( out_ptr == NULL)
		FAIL( NULL_OUT, " ");
	if( filename == NULL)
		FAIL( NULL_IN, " ");
	FILE * read = fopen(filename,"r");
	if( read == NULL)
		FAIL( OPEN_IN_FAIL, "%s", filename);
	fclose( read);

	if( strstr( filename, ".xml") != NULL)		return read_egv_xml( filename, out_ptr);
	//else if( strstr( filename, ".dat") != NULL)	return read_egv_dat( filename, out_ptr);
	else 						FAIL( FAIL, "Non recognized extension for eigenvalues file");
}
*/


#ifdef __LIBXML2
errh * read_wfc_xml( const char * filename, wfc ** out_ptr)
{
	wfc * res = NULL;
	TEST_ARGS();
	fclose( read);

	res = initialize_wfc( );

	xmlDocPtr	document;
	xmlNodePtr	root, node;
	document = xmlReadFile( filename, NULL, 0);
	root = xmlDocGetRootElement( document);

	char buffer[128];

	node = qepp_libxml_find_node( "INFO", root);
	assert( !qepp_libxml_get_node_attr( &res->ngw,   "ngw", node, R_LNT, 1));
	assert( !qepp_libxml_get_node_attr( &res->igwx,  "igwx", node, R_LNT, 1));
	assert( !qepp_libxml_get_node_attr( &buffer,     "gamma_only", node, R_STR, 1));
	if( buffer[0] == 'F')
		res->gamma_only = false;
	else
		res->gamma_only = true;
	assert( !qepp_libxml_get_node_attr( &res->nbnd,  "nbnd", node, R_INT, 1));
	assert( !qepp_libxml_get_node_attr( &res->ik,    "ik", node, R_LNT, 1));
	assert( !qepp_libxml_get_node_attr( &res->nk,    "nk", node, R_LNT, 1));
	assert( !qepp_libxml_get_node_attr( &res->ispin, "ispin", node, R_INT, 1));
	assert( !qepp_libxml_get_node_attr( &res->nspin, "nspin", node, R_INT, 1));
	assert( !qepp_libxml_get_node_attr( &res->scale_factor, "scale_factor", node, R_LNF, 1));


	res->index = QEPP_ALLOC( sizeof( long int), res->nbnd);
	res->grid = QEPP_ALLOC( sizeof( long int), res->nbnd, 3);
	char name[256];
	strcpy( name, filename);
	int app = strlen( name);
//QEPP_PRINT("len: %d\n", app);
	while( name[app] != '/' && app>0)
	{
		name[app] = '\0';
		app--;
	}
	char name1[256];
	sprintf( name1, "%s%s", name, "gkvectors.xml");
//QEPP_PRINT( "filename:%s    name: %s   name1: %s\n", filename, name, name1);
	read_gkv_xml( name1, res);

	res->val = QEPP_ALLOC( sizeof( double complex), res->nbnd, res->igwx);
	for(int i=0; i<res->nbnd; i++)
	{
		sprintf( buffer, "evc.%d", i+1);
		node = qepp_libxml_find_node( buffer, root);
		assert( !qepp_libxml_get_node_value( res->val[i], node, R_LNF, 2*res->igwx));
	}
	
	xmlFreeDoc( document);

	*out_ptr = res;
	SUCCESS();
}

static errh * read_gkv_xml( const char * filename, wfc * res)
{
	//gkv * res = NULL;
	//if( out_ptr == NULL)
	//	FAIL( NULL_OUT, " ");
	//*out_ptr = NULL;
	if( filename == NULL)
		FAIL( NULL_IN, " ");
	if( ! qepp_is_file( filename))
		FAIL( OPEN_IN_FAIL, "%s is not a file.", filename);
	FILE * read = fopen( filename, "r");
	if( read == NULL)
		FAIL( OPEN_IN_FAIL, "%s", filename);
	fclose( read);

	xmlDoc		* document;
	xmlNode		* root, * node;
	document = xmlReadFile( filename, NULL, 0);
	root = xmlDocGetRootElement( document);

	long int ngkv = 0;
	char buffer[128];

	node = qepp_libxml_find_node( "NUMBER_OF_GK-VECTORS", root);
	assert( !qepp_libxml_get_node_value( &ngkv, node, R_LNT, 1));
	if( ngkv <= 0)
		FAIL( FAIL, "Failed to read ngkv");
	//res = initialize_gkv( ngkv);

	node = qepp_libxml_find_node( "MAX_NUMBER_OF_GK-VECTORS", root);
	assert( !qepp_libxml_get_node_value( &res->max_igwx, node, R_LNT, 1));

	node = qepp_libxml_find_node( "GAMMA_ONLY", root);
	assert( !qepp_libxml_get_node_value( &buffer, node, R_STR, 1));
	if( buffer[0] == 'T')
		res->gamma_only = true;
	else
		res->gamma_only = false;

	node = qepp_libxml_find_node( "K-POINT_COORDS", root);
	assert( !qepp_libxml_get_node_value( res->kpt, node, R_LNF, 3));

	node = qepp_libxml_find_node( "INDEX", root);
	assert( !qepp_libxml_get_node_value( qepp_mem_get_base( res->index), node, R_LNT, ngkv));

	node = qepp_libxml_find_node( "GRID", root);
	assert( !qepp_libxml_get_node_value( qepp_mem_get_base( res->grid), node, R_INT, ngkv*3));

	xmlFreeDoc( document);

	//*out_ptr = res;
	SUCCESS();
}

static errh * read_egv_xml( const char * filename, data_file * res)
{
	//egv * res = NULL;
	//TEST_ARGS();
	//fclose( read);

	xmlDoc		* document;
	xmlNode		* root, * node;
	document = xmlReadFile( filename, NULL, 0);
	root = xmlDocGetRootElement( document);

	int n_bnd = 0;
	char buffer[128];

	node = qepp_libxml_find_node( "INFO", root);
	assert( !qepp_libxml_get_node_attr( &n_bnd, "nbnd", node, R_INT, 1));
	//res = initialize_egv( n_bnd);

	//if( res == NULL)
	//	FAIL( FAIL, "Failed to read eigenvalue file\n");

	node = qepp_libxml_find_node( "UNITS_FOR_ENERGIES", root);
	assert( !qepp_libxml_get_node_attr( &buffer, "UNITS", node, R_STR, 1));
	if ( !strcmp( buffer, "Hartree"))	res->e_t = HARTREE_TO_RY * RY_TO_EV;
	else if( !strcmp( buffer, "Rydberg"))	res->e_t = RY_TO_EV;
	else WARN( "Failed to read energy units");

	node = qepp_libxml_find_node( "EIGENVALUES", root);
	assert( !qepp_libxml_get_node_value( qepp_mem_get_base( res->egv), node, R_LNF, n_bnd));

	node = qepp_libxml_find_node( "OCCUPATIONS", root);
	assert( !qepp_libxml_get_node_value( qepp_mem_get_base( res->occ), node, R_LNF, n_bnd));

	xmlFreeDoc( document);

	//*out_ptr = res;
	SUCCESS();
}
#else //__LIBXML
errh * read_data_file( const char * filename, data_file ** data) {FAIL( FAIL, "Only available with libxml2 compilation\n");}
errh * read_wfc_xml( const char * filename, wfc ** a) {FAIL( FAIL, "Only available with libxml2 compilation\n");}
errh * read_gkv_xml( const char * filename, gkv ** a) {FAIL( FAIL, "Only available with libxml2 compilation\n");}
errh * read_egv_xml( const char * filename, egv ** a) {FAIL( FAIL, "Only available with libxml2 compilation\n");}
#endif //__LIBXML


errh * read_wfc_dat( const char * filename, wfc ** out_ptr)
{
	double app;
	long int n_bnd;
	wfc * res = NULL;
	if( out_ptr == NULL)
		FAIL( NULL_OUT, " ");
	if( filename == NULL)
		FAIL( NULL_IN, " ");
	FILE * read = fopen( filename, "rb");
	if( read == NULL)
		FAIL( OPEN_IN_FAIL, "%s", filename);

	if( df == NULL)
		FAIL( FAIL, "data-file has not been parsed previously");

	if( df->version < 6*1E6 + 2*1E3 + 0*1E0)
	{
		long int pos = ftell( read);
		if( qepp_get_dat_attr( &app, read, pos, "INFO", "nbnd"))
			FAIL( FAIL, "Can't read nbnd");	
		n_bnd = app;
		res = initialize_wfc( n_bnd);
		if( qepp_get_dat_attr( &app, read, pos, "INFO", "ngw"))
			FAIL( FAIL, "Can't read ngw");
		res->ngw = app;
		if( qepp_get_dat_attr( &app, read, pos, "INFO", "igwx"))
			FAIL( FAIL, "Can't read igwx");
		res->igwx = app;
		if( qepp_get_dat_attr( &app, read, pos, "INFO", "ik"))
			FAIL( FAIL, "Can't read ik");
		res->ik = app;
		if( qepp_get_dat_attr( &app, read, pos, "INFO", "nk"))
			FAIL( FAIL, "Can't read nk");
		res->nk = app;
		if( qepp_get_dat_attr( &app, read, pos, "INFO", "ispin"))
			FAIL( FAIL, "Can't read ispin");
		res->ispin = app;
		if( qepp_get_dat_attr( &app, read, pos, "INFO", "nspin"))
			FAIL( FAIL, "Can't read nspin");
		res->nspin = app;
		if( qepp_get_dat_attr( &app, read, pos, "INFO", "scale_factor"))
			FAIL( FAIL, "Can't read scale");
		res->scale_factor = app;
		
		res->index = QEPP_ALLOC( sizeof( long int), res->igwx);
		res->grid = QEPP_ALLOC( sizeof( int), res->igwx, 3);
		char name[128];
		strcpy( name, filename);
		int app = strlen( name);
//QEPP_PRINT("len: %d\n", app);
		while( name[app] != '/' && app>0)
		{
			name[app] = '\0';
			app--;
		}
		char name1[128];
		sprintf( name1, "%s%s", name, "gkvectors.dat");
//QEPP_PRINT( "filename:%s    name: %s   name1: %s\n", filename, name, name1);
		read_gkv_dat( name1, res);
		
		char needle[64];
		res->val = QEPP_ALLOC( sizeof( double complex), n_bnd, res->igwx);
		for( int i=0; i<n_bnd; i++)
		{
			sprintf( needle, "evc.%d", i+1);
			if( qepp_get_dat_value( res->val[i], read, pos, needle, res->igwx, sizeof( double complex), dump_size))
			{
				STRUCT_FREE( res);
				FAIL( FAIL, "Failed to read wavefunction");
			}
		}
	}
	else
	{
		int nkpt, nspin, nbnd, igwx, max_index, ispin;
		double scale_factor, x, y, z;
		int dump_i, dd; double dump_d;
		dd=fread( &dump_i, 4, 1, read);	//size_vect(
		dd=fread( &nkpt, 4, 1, read);
		dd=fread( &x, 8, 1, read);	//k_x [1/bohr]
		dd=fread( &y, 8, 1, read);	//k_y [1/bohr]
		dd=fread( &z, 8, 1, read);	//k_z [1/bohr]
		dd=fread( &ispin, 4, 1, read);	//spin_component
		dd=fread( &dump_i, 4, 1, read);	//GAMMA_ONLY
		dd=fread( &scale_factor, 8, 1, read);	//scale_factor
		dd=fread( &dump_i, 4, 1, read);	//)size_vect
		dd=fread( &dump_i, 4, 1, read);	//size_vect(
		dd=fread( &max_index, 4, 1, read);	//max_index
		dd=fread( &igwx, 4, 1, read);	//igwx
		dd=fread( &nspin, 4, 1, read);	//nspin
		dd=fread( &nbnd, 4, 1, read);	//nbnd
		dd=fread( &dump_i, 4, 1, read);	//)size_vect
		dd=fread( &dump_i, 4, 1, read);	//size_vect(
		dd=fread( &dump_d, 8, 1, read);	//b1_x [1/bohr]
		dd=fread( &dump_d, 8, 1, read);	//b1_y [1/bohr]
		dd=fread( &dump_d, 8, 1, read);	//b1_z [1/bohr]
		dd=fread( &dump_d, 8, 1, read);	//b2_x [1/bohr]
		dd=fread( &dump_d, 8, 1, read);	//b2_y [1/bohr]
		dd=fread( &dump_d, 8, 1, read);	//b2_z [1/bohr]
		dd=fread( &dump_d, 8, 1, read);	//b3_x [1/bohr]
		dd=fread( &dump_d, 8, 1, read);	//b3_y [1/bohr]
		dd=fread( &dump_d, 8, 1, read);	//b3_z [1/bohr]
		dd=fread( &dump_i, 4, 1, read);	//)size_vect

		res = initialize_wfc( nbnd);
		res->ngw = -1;
		res->igwx = igwx;
		res->ik = nkpt;
		res->nk = -1;
		res->ispin = ispin;
		res->nspin = nspin;
		res->scale_factor = scale_factor;
		res->kpt[0] = x; res->kpt[1] = y; res->kpt[2] = z; 
		res->grid = QEPP_ALLOC( sizeof( int), igwx, 3);


		dd=fread( &dump_i, 4, 1, read);	//size_vect(
		dd=fread( qepp_mem_get_base( res->grid), sizeof( int), igwx*3, read);	//g-vect grid
		dd=fread( &dump_i, 4, 1, read);	//)size_vect
		
		res->val = QEPP_ALLOC( sizeof( double complex), nbnd, nspin*igwx);
		for( int i=0; i<nbnd; i++)
		{
			dd=fread( &dump_i, 4, 1, read);	//size_vect(
			if( fread( res->val[i], sizeof( double complex), nspin*igwx, read) != nspin*igwx) //coefficients
			{
				STRUCT_FREE( res);
				FAIL( FAIL, "Failed to read wavefunction");
			}
//QEPP_PRINT( "%g\n", creal(res->val[i][0]));
			dd=fread( &dump_i, 4, 1, read);	//)size_vect
		}
		dd+=dd;
	}
	


	fclose( read);

	*out_ptr = res;
	SUCCESS();
}

static errh * read_gkv_dat( const char * filename, wfc * res)
{
	long int pos;
	long int size;
	double app;
	int kind;
	int app_i;
	long int ngkv;

	//gkv * res = NULL;
	if( filename == NULL)
		FAIL( NULL_IN, " ");
	FILE * read = fopen( filename, "rb");
	if( read == NULL)
		FAIL( OPEN_IN_FAIL, "%s", filename);

	if( df == NULL)
		FAIL( FAIL, "data-file has not been parsed previously");

	if( df->version < 6*1E6 + 2*1E3 + 0*1E0)
	{
		//read ngkw
		pos = ftell( read);
		if( qepp_get_dat_attr( &app, read, pos, "NUMBER_OF_GK-VECTORS", "kind"))
			FAIL( FAIL, " ");
		kind = app;
		switch( kind)
		{
		case 4:
			if( qepp_get_dat_value( &app_i, read, pos, "NUMBER_OF_GK-VECTORS", 1, kind, dump_size)) 
				FAIL( FAIL, "Can't read ngkv");
			ngkv = app_i;
			break;
		case 8:
			if( qepp_get_dat_value( &ngkv, read, pos, "NUMBER_OF_GK-VECTORS", 1, kind, dump_size)) 
				FAIL( FAIL, "Can't read ngkv");
			break;
		default:
			FAIL( FAIL, "kind for ngkw not implemented");
		}
		if( ngkv<0)
			FAIL( FAIL, "Negative ngkv");
		//res = initialize_gkv( ngkv);

		//read max_ngkv
		pos = ftell( read);
		if( qepp_get_dat_attr( &app, read, pos, "MAX_NUMBER_OF_GK-VECTORS", "kind"))
			FAIL( FAIL, " ");
		kind = app;
		switch( kind)
		{
		case 4:
			if( qepp_get_dat_value( &app_i, read, pos, "MAX_NUMBER_OF_GK-VECTORS", 1, kind, dump_size)) 
				FAIL( FAIL, "Can't read ngkv");
			res->max_igwx = app_i;
			break;
		case 8:
			if( qepp_get_dat_value( &res->max_igwx, read, pos, "MAX_NUMBER_OF_GK-VECTORS", 1, kind, dump_size)) 
				FAIL( FAIL, "Can't read ngkv");
			break;
		default:
			FAIL( FAIL, "kind for ngkw not implemented");
		}

		//read kpt coord
		if( qepp_get_dat_value( res->kpt, read, pos, "K-POINT_COORDS", 3, sizeof( double), dump_size)) 
			FAIL( FAIL, "Can't read kpt coordinates");

		//read index
		pos = ftell( read);
		if( qepp_get_dat_attr( &app, read, pos, "INDEX", "size"))
			FAIL( FAIL, " ");
		size = app;
		if( qepp_get_dat_value( qepp_mem_get_base( res->index), read, pos, "INDEX", size, sizeof( int), dump_size)) 
			FAIL( FAIL, "Can't read index");

		//read grid
		pos = ftell( read);
		if( qepp_get_dat_attr( &app, read, pos, "GRID", "size"))
			FAIL( FAIL, " ");
		size = app;
		if( qepp_get_dat_value( qepp_mem_get_base( res->grid), read, pos, "GRID", size, sizeof( int), dump_size)) 
			FAIL( FAIL, "Can't read grid");
	}
	else
	{
		int nkpt, nspin, nbnd, igwx, max_index;
		double x,y,z;
		int dump_i, dd; double dump_d;
		dd=fread( &dump_i, 4, 1, read);	//size_vect(
		dd=fread( &nkpt, 4, 1, read);
		dd=fread( &x, 8, 1, read);		//k_x [1/bohr]
		dd=fread( &y, 8, 1, read);		//k_y [1/bohr]
		dd=fread( &z, 8, 1, read);		//k_z [1/bohr]
		dd=fread( &dump_i, 4, 1, read);	//???
		dd=fread( &dump_i, 4, 1, read);	//GAMMA_ONLY
		dd=fread( &dump_d, 8, 1, read);	//scale_factor
		dd=fread( &dump_i, 4, 1, read);	//)size_vect
		dd=fread( &dump_i, 4, 1, read);	//size_vect(
		dd=fread( &max_index, 4, 1, read);	//max_index
		dd=fread( &igwx, 4, 1, read);	//igwx
		dd=fread( &nspin, 4, 1, read);	//nspin
		dd=fread( &nbnd, 4, 1, read);	//nbnd
		dd=fread( &dump_i, 4, 1, read);	//)size_vect
		dd=fread( &dump_i, 4, 1, read);	//size_vect(
		dd=fread( &dump_d, 8, 1, read);	//b1_x [1/bohr]
		dd=fread( &dump_d, 8, 1, read);	//b1_y [1/bohr]
		dd=fread( &dump_d, 8, 1, read);	//b1_z [1/bohr]
		dd=fread( &dump_d, 8, 1, read);	//b2_x [1/bohr]
		dd=fread( &dump_d, 8, 1, read);	//b2_y [1/bohr]
		dd=fread( &dump_d, 8, 1, read);	//b2_z [1/bohr]
		dd=fread( &dump_d, 8, 1, read);	//b3_x [1/bohr]
		dd=fread( &dump_d, 8, 1, read);	//b3_y [1/bohr]
		dd=fread( &dump_d, 8, 1, read);	//b3_z [1/bohr]
		dd=fread( &dump_i, 4, 1, read);	//)size_vect

		if( igwx<0)
			FAIL( FAIL, "Negative ngkv");
		//res = initialize_gkv( igwx);
		res->max_igwx = max_index;

		res->kpt[0]=x; res->kpt[1]=y; res->kpt[2]=z;		

		dd=fread( &dump_i, 4, 1, read);	//size_vect(
		dd=fread( qepp_mem_get_base( res->grid), sizeof( int), 3*igwx, read);	//g-vect grid
		dd=fread( &dump_i, 4, 1, read);	//)size_vect
		dd+=dd;
	}

	fclose( read);

	//*out_ptr = res;
	SUCCESS();
}


















