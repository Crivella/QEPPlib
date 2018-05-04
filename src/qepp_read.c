#include <qepp/qepp_read.h>

extern unsigned int ionode;
extern int verbosity;

extern unsigned long int TOT_MEM;

extern char outdir[1024];
extern char prefix[128];
extern char workdir[1024];
extern char datafile_path[1024];
extern data_file * df;

#define TEST_ARGS \
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
errh * read_nscf_md( char * filename, nscf_md ** out_ptr)
{
	TEST_ARGS;

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
		char * buffer = change_file( filename, "scf_1.out");
		FILE * app_f = NULL;
		if( qepp_is_file( buffer))
			app_f = open_qe_out( buffer);
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

errh * read_nscf_data( char * filename, nscf_data ** out_ptr)
{
	TEST_ARGS;

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
		
/*
#ifdef __LIBXML2
	if( qepp_is_file( "data-file.xml"))
	{
		QEPP_PRINT("Reading weights from data-file.xml\n");
		data_file * df;
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
*/
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

errh * read_band_data( char * filename, band_data ** out_ptr)
{
	TEST_ARGS;

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

errh * read_band_pp( char * filename, band_pp ** out_ptr)
{
	TEST_ARGS;

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

errh * read_spin_data( char * filename, spin_data ** out_ptr)
{
	TEST_ARGS;

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

errh * read_opt_data( char * filename, opt_data ** out_ptr, char * comments)
{
	TEST_ARGS;

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

errh * read_m_elem(char * filename, m_elem ** out_ptr, char * comments)
{
	TEST_ARGS;

	m_elem * data;
	char fname_app[128];
	char * fname;
	if( !qepp_is_file( "nscf_1.out"))
	{
		if( !qepp_is_file( "bands_1.out"))
			fname = get_file( "nscf_1.out", ".out");
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

errh * read_fit_params(char * filename, fit_params ** out_ptr) { SUCCESS();}
errh * read_data_set(char * filename, data_set ** out_ptr) { SUCCESS();}

errh * read_pdos_data(char * filename, pdos_data ** out_ptr, char * comments)
{
	TEST_ARGS;

	pdos_data * data = NULL;
	char buffer[256];
	int n_states=0;

	char * nscf_f = get_file( "nscf_1.out", ".out");
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
	printf("%li kpt, %d bands, %d states found\n",nscf->n_kpt,nscf->n_bnd,n_states);

	data = initialize_pdos_data( nscf->n_kpt, nscf->n_bnd, n_states);
	data->nscf = DUPLICATE( nscf);

	QEPP_PRINT("Reading state data...\n");
	fseek( read, pos, SEEK_SET);
	int state_app;
	int app=1;
	pdos_state * p_app;
	for( int i=0; i<n_states; i++)
	{
		p_app = data->states[i];
		while( fgetc(read) != '#') {}
		app *= fscanf( read, "%d", &state_app);
		while( fgetc(read) != 'm') {}
		app *= fscanf( read, "%d", &p_app->atom_n);
		while( fgetc(read) != '(') {}
		app *= fscanf( read, "%s)", p_app->name);
		while( fgetc(read) != 'c') {}
		app *= fscanf( read, "%d", &p_app->wfc_n);
		while( fgetc(read) != '=') {}
		app *= fscanf( read, "%lf", &p_app->j);
		while( fgetc(read) != '=') {}
		app *= fscanf( read, "%d", &p_app->l);
		while( fgetc(read) != '=') {}
		app *= fscanf( read, "%lf", &p_app->m_j);
		if( app == 0)
		{
			FREE( data);
			FAIL( FAIL, "Failed to read state %d...\n",i+1);
		}
	}
	QEPP_PRINT("...done\n");

	QEPP_PRINT("Reading pdos data...\n");
	long int k_app=0;
	double v_app[3];
	int b_app, b_app_r;
	double en_app;
	char c;
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
			FREE( data);
			FAIL( FAIL, "Mismatch between pdos and nscf file for k_pt coordinates");
		}
		b_app=0;
		while( b_app < nscf->n_bnd)
		{
			while( fgetc(read) != '(' && !feof(read)) {}
			app *= fscanf( read, "%d", &b_app_r); b_app_r--;
			qepp_fscanf_double( read, &en_app);
			qepp_getline(buffer,256,read);
			if( fabs( en_app - nscf->energies[k_app][b_app]) > 0.005)
			{
				FREE( data);
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

errh * read_pdos_state(char * filename, pdos_state ** out_ptr)
{
	SUCCESS();
}

#ifdef __LIBXML2
errh * read_data_file( const char * filename, data_file ** out_ptr)
{
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
	//if( path == NULL)
	//	FAIL( FAIL, "Can't find tmp directory");
	//sprintf( fname, "%s/%s", path, filename);
	strcpy( fname, filename);
//printf("fname: %s\npath: %s\n\n", fname,path);
	/*FILE * read = fopen(fname,"r");
	if( read == NULL)
		FAIL( OPEN_IN_FAIL, "%s", fname);
	fclose( read);*/


	res = initialize_data_file();

	xmlDoc		* document;
	xmlNode		* root, * node, * app, * app1;
	xmlChar		* key;

	document = xmlReadFile( fname, NULL, 0);
	root = xmlDocGetRootElement( document);

	char * endptr = NULL;
	//printf( "%s\n", root->name); fflush(stdout);
	if( !xmlStrcmp( root->name, (const xmlChar *)"Root"))
	{
		for( node = root->children; node; node=node->next)
		{
			if( !xmlStrcmp( node->name, (const xmlChar *)"CELL"))
			{
				app = node;
				for( node = node->children; node; node=node->next)
				{
					if( !xmlStrcmp( node->name, (const xmlChar *)"BRAVAIS_LATTICE"))
					{
						key = xmlNodeListGetString( document, node->xmlChildrenNode, 1);
						strcpy( res->bravais, (char *)key);
						xmlFree( key);
					}

					if( !xmlStrcmp( node->name, (const xmlChar *)"LATTICE_PARAMETER"))
					{
						key = xmlGetProp( node, (const xmlChar *)"UNITS");
						if( strcasestr( (char *)key, "bohr") != NULL)
							res->lp_t = BOHR_TO_M;
						xmlFree( key);

						key = xmlNodeListGetString( document, node->xmlChildrenNode, 1);
						qepp_sscanf_double( (char *)key, &res->lp, NULL);
						xmlFree( key);
					}
					if( !xmlStrcmp( node->name, (const xmlChar *)"CELL_DIMENSIONS"))
					{
						key = xmlNodeListGetString( document, node->xmlChildrenNode, 1);
						endptr = (char *)key;
						for( int n=0; n<6; n++)
							qepp_sscanf_double( endptr, &res->c_dim[n], &endptr);
						xmlFree( key);
					}
					if( !xmlStrcmp( node->name, (const xmlChar *)"DIRECT_LATTICE_VECTORS"))
					{
						app1 = node;
						node = node->children;
						if( node == NULL)
						{
							node = app1;
							WARN( "Failed to acquire direct lattice info");
							break;
						}
						while( strstr( (char *)node->name, "UNITS") == NULL)
							node = node->next;

						key = xmlGetProp( node, (const xmlChar *)"UNITS");
						strcpy( res->a_n, (char *)key);
						if( !xmlStrcmp( key, (const xmlChar *)"Bohr"))
							res->a_t = BOHR_TO_M;
						xmlFree( key);

						for(int i=0; i<3&&node; i++, node=node->next)
						{
							while( strstr( (char *)node->name, "a") == NULL)
								node=node->next;
							key = xmlNodeListGetString( document, node->xmlChildrenNode, 1);
							endptr = (char *)key;
							for( int j=0; j<3; j++)
								qepp_sscanf_double( endptr, &res->a[i][j], &endptr);
							xmlFree( key);
						}

						node = app1;
					}
					if( !xmlStrcmp( node->name, (const xmlChar *)"RECIPROCAL_LATTICE_VECTORS"))
					{
						app1 = node;
						node = node->children;
						if( node == NULL)
						{
							node = app1;
							WARN( "Failed to acquire reciprocal lattice info");
							break;
						}
						while( strstr( (char *)node->name, "UNITS") == NULL)
							node = node->next;

						key = xmlGetProp( node, (const xmlChar *)"UNITS");
						strcpy( res->b_n, (char *)key);
						if( !xmlStrcmp( key, (const xmlChar *)"2 pi / a"))
							res->b_t = 2*PI/(res->lp*res->lp_t);

						xmlFree( key);


						for(int i=0; i<3&&node; i++, node=node->next)
						{
							while( strstr( (char *)node->name, "b") == NULL)
								node=node->next;	
							key = xmlNodeListGetString( document, node->xmlChildrenNode, 1);
							endptr = (char *)key;
							for( int j=0; j<3; j++)
								qepp_sscanf_double( endptr, &res->b[i][j], &endptr);
							xmlFree( key);
						}

						node = app1;
					}
				}
				node = app;
			}
			if( !xmlStrcmp( node->name, (const xmlChar *)"BRILLOUIN_ZONE"))
			{
				app = node;
				char buffer[64]="K-POINT.1";
				int app_n=1;
				for( node=node->children; node; node=node->next)
				{
					if( !xmlStrcmp( node->name, (const xmlChar *)"NUMBER_OF_K-POINTS"))
					{
						key = xmlNodeListGetString( document, node->xmlChildrenNode, 1);
						res->n_kpt = strtol( (char *)key, 0, 10);
						res->kpt = (double **)AllocateLinearMem2( sizeof( double), res->n_kpt, 3);
						res->weight = (double *)AllocateLinearMem1( sizeof( double), res->n_kpt);
						res->egval_link = (char **)AllocateLinearMem2( sizeof( char), res->n_kpt, 256);
						res->egvec_link = (char **)AllocateLinearMem2( sizeof( char), res->n_kpt, 256);
						res->wfc_link = (char **)AllocateLinearMem2( sizeof( char), res->n_kpt, 256);
						xmlFree( key);
					}
					if( !xmlStrcmp( node->name, (const xmlChar *)"UNITS_FOR_K-POINTS"))
					{
						key = xmlGetProp( node, (const xmlChar *)"UNITS");
						if( !xmlStrcmp( key, (const xmlChar *)"2 pi / a"))
							res->kpt_t = 2*PI/(res->lp*res->lp_t);
						xmlFree( key);
					}
					if( !xmlStrcmp( node->name, (const xmlChar *)buffer))
					{
						key = xmlGetProp( node, (const xmlChar *)"XYZ");
						endptr = (char *)key;
						for( int n=0; n<3; n++)
							qepp_sscanf_double( endptr, &res->kpt[app_n-1][n], &endptr);
						xmlFree( key);

						key = xmlGetProp( node, (const xmlChar *)"WEIGHT");
						qepp_sscanf_double( (char *)key, &res->weight[app_n-1], 0);
						xmlFree( key);

						app_n++;
						sprintf( buffer, "K-POINT.%d", app_n);
					}
				}
				node = app;
			}
			if( !xmlStrcmp( node->name, (const xmlChar *)"EIGENVALUES"))
			{
				app = node;
				char buffer[64]="K-POINT.1";
				int app_n=1;
				for( node=node->children; node; node=node->next)
				{
					if( !xmlStrcmp( node->name, (const xmlChar *)buffer))
					{
						app1 = node;
						for( node=node->children; node; node=node->next)
						{
							if( !xmlStrcmp( node->name, (const xmlChar *)"K-POINT_COORDS"))
							{
								key = xmlNodeListGetString( document, node->xmlChildrenNode, 1);
								endptr = (char *)key;
								double a;
								for( int n=0; n<3; n++)
								{
									qepp_sscanf_double( endptr, &a, &endptr);
									if( a != res->kpt[app_n-1][n])
										WARN( "kpt coordinates in eigenvalue list differ from BZ list");
								}
								xmlFree( key);
							}
							if( !xmlStrcmp( node->name, (const xmlChar *)"DATAFILE"))
							{
								key = xmlGetProp( node, (const xmlChar *)"iotk_link");
//QEPP_PRINT("%s\n", key);
								sprintf( res->egval_link[app_n-1], "%s/%s", path, (char *)key);
//QEPP_PRINT("%s\n", res->egval_link[app_n-1]);
//printf("%s\n", res->egval_link[app_n-1]); 
								//strcpy( res->egval_link[app_n-1], (char *)key);
								xmlFree( key);
							}
						}
						node = app1;

						app_n++;
						sprintf( buffer, "K-POINT.%d", app_n);
					}
				}
				node = app;
			}
			if( !xmlStrcmp( node->name, (const xmlChar *)"EIGENVECTORS"))
			{
				app = node;
				char buffer[64]="K-POINT.1";
				int app_n=1;
				for( node=node->children; node; node=node->next)
				{
					if( !xmlStrcmp( node->name, (const xmlChar *)buffer))
					{
						app1 = node;
						for( node=node->children; node; node=node->next)
						{
							if( !xmlStrcmp( node->name, (const xmlChar *)"GK-VECTORS"))
							{
								key = xmlGetProp( node, (const xmlChar *)"iotk_link");
								sprintf( res->egvec_link[app_n-1], "%s/%s", path, (char *)key);
								//strcpy( res->egvec_link[app_n-1], (char *)key);
								xmlFree( key);
							}
							if( res->n_spin == 4)
							{
								if( !xmlStrcmp( node->name, (const xmlChar *)"WFC.1"))
								{
									key = xmlGetProp( node, (const xmlChar *)"iotk_link");
									sprintf( res->wfc_link[app_n-1], "%s/%s", path, (char *)key);
									//strcpy( res->wfc_link[app_n-1], (char *)key);
									xmlFree( key);
								}
								if( !xmlStrcmp( node->name, (const xmlChar *)"WFC.2"))
								{
									key = xmlGetProp( node, (const xmlChar *)"iotk_link");
									sprintf( res->wfc_link2[app_n-1], "%s/%s", path, (char *)key);
									//strcpy( res->wfc_link2[app_n-1], (char *)key);
									xmlFree( key);
								}
							}
							else
							{
								if( !xmlStrcmp( node->name, (const xmlChar *)"WFC"))
								{
									key = xmlGetProp( node, (const xmlChar *)"iotk_link");
									sprintf( res->wfc_link[app_n-1], "%s/%s", path, (char *)key);
									//strcpy( res->wfc_link[app_n-1], (char *)key);
									xmlFree( key);
								}
							}
						}
						node = app1;

						app_n++;
						sprintf( buffer, "K-POINT.%d", app_n);
					}
				}
				node = app;
			}
			if( !xmlStrcmp( node->name, (const xmlChar *)"BAND_STRUCTURE_INFO"))
			{
				app = node;
				for( node=node->children; node; node=node->next)
				{
					if( !xmlStrcmp( node->name, (const xmlChar *)"NUMBER_OF_BANDS"))
					{
						key = xmlNodeListGetString( document, node->xmlChildrenNode, 1);
						res->n_bnd = atoi( (char *)key);
						xmlFree( key);
					}
					if( !xmlStrcmp( node->name, (const xmlChar *)"NUMBER_OF_ELECTRONS"))
					{
						key = xmlNodeListGetString( document, node->xmlChildrenNode, 1);
						res->n_el = atof( (char *)key);
						xmlFree( key);
					}
					if( !xmlStrcmp( node->name, (const xmlChar *)"NUMBER_OF_SPIN_COMPONENTS"))
					{
						key = xmlNodeListGetString( document, node->xmlChildrenNode, 1);
						res->n_spin = atoi( (char *)key);
						xmlFree( key);

						if( res->n_spin == 4)
							res->wfc_link2 = (char **)AllocateLinearMem2( sizeof( char), res->n_kpt, 256);
					}
				}
				node = app;
			}
			if( !xmlStrcmp( node->name, (const xmlChar *)"INFO"))
			{
			}
		}
	}
	else
	{
		FAIL( FAIL, "Non-recognized version of espresso (must be <= 6.1)");
	}
	xmlFreeDoc( document);
	free( path);

	*out_ptr = res;
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


#ifdef __LIBXML2
errh * read_wfc_xml( const char * filename, wfc ** out_ptr)
{
	wfc * res = NULL;
	if( out_ptr == NULL)
		FAIL( NULL_OUT, " ");
	if( filename == NULL)
		FAIL( NULL_IN, " ");
	FILE * read = fopen(filename,"r");
	if( read == NULL)
		FAIL( OPEN_IN_FAIL, "%s", filename);
	fclose( read);

	xmlDoc		* document;
	xmlNode		* root, * node;
	xmlChar		*key;

	int n_evc=0;

	document = xmlReadFile( filename, NULL, 0);
	root = xmlDocGetRootElement( document);
	if( !xmlStrcmp( root->name, (const xmlChar *)"WFC"))
	{
		for( node = root->children; node; node=node->next)
			if( qepp_strcmp_wc( "evc.*", (char *)node->name))
				n_evc++;
		res = initialize_wfc( n_evc);
 
		for( node = root->children; node; node=node->next)
		{
			if( !xmlStrcmp( node->name, (const xmlChar *)"INFO"))
			{
				key = xmlGetProp( node, (const xmlChar *)"ngw");
				res->ngw = strtol( (char *)key, 0, 10);
				xmlFree( key);

				key = xmlGetProp( node, (const xmlChar *)"igwx");
				res->igwx = strtol( (char *)key, 0, 10);
				xmlFree( key);

				key = xmlGetProp( node, (const xmlChar *)"gamma_only");
				if( key[0] == 'F')
					res->gamma_only = false;
				else
					res->gamma_only = true;
				xmlFree( key);

				key = xmlGetProp( node, (const xmlChar *)"nbnd");
				res->nbnd = strtod( (char *)key, 0);
				xmlFree( key);

				key = xmlGetProp( node, (const xmlChar *)"ik");
				res->ik = strtol( (char *)key, 0, 10);
				xmlFree( key);

				key = xmlGetProp( node, (const xmlChar *)"nk");
				res->nk = strtol( (char *)key, 0, 10);
				xmlFree( key);

				key = xmlGetProp( node, (const xmlChar *)"ispin");
				res->ispin = atoi( (char *)key);
				xmlFree( key);

				key = xmlGetProp( node, (const xmlChar *)"nspin");
				res->nspin = atoi( (char *)key);
				xmlFree( key);

				key = xmlGetProp( node, (const xmlChar *)"scale_factor");
				res->scale_factor = strtod( (char *)key, 0);
				xmlFree( key);
			}
		}

		int i=0;
		for( node = root->children; node; node=node->next)
		{
			if( qepp_strcmp_wc( "evc.*", (char *)node->name))
				res->evc_vect[i++] = read_evc_xml( document, root, node);
		}
	}
	xmlFreeDoc( document);

	*out_ptr = res;
	SUCCESS();
}

evc * read_evc_xml( xmlDoc * document, xmlNode * root, xmlNode * node)
{
	evc * res = NULL;
	if( document == NULL || root == NULL)
		return NULL;

	xmlChar * app;
	app = xmlGetProp( node, (const xmlChar *)"size");
	long int size = strtol( (char *)app, 0, 10);
	xmlFree( app);

	res = initialize_evc( size);

	xmlChar * key = xmlNodeListGetString( document, node->xmlChildrenNode, 1);

	double r=0, c=0;
	char * endptr = (char *)key;
	for( long int i=0; i<size; i++)
	{
		qepp_sscanf_double( endptr, &r, &endptr);
		qepp_sscanf_double( endptr, &c, &endptr);
		res->val[i] = r + c * I;
	}
	xmlFree( key);

	return res;
}

errh * read_gkv_xml( const char * filename, gkv ** out_ptr)
{
	gkv * res = NULL;
	if( out_ptr == NULL)
		FAIL( NULL_OUT, " ");
	if( filename == NULL)
		FAIL( NULL_IN, " ");
	FILE * read = fopen(filename,"r");
	if( read == NULL)
		FAIL( OPEN_IN_FAIL, "%s", filename);
	fclose( read);

	xmlDoc		* document;
	xmlNode		* root, * node;
	xmlChar		* key;

	long int ngkv = 0;

	document = xmlReadFile( filename, NULL, 0);
	root = xmlDocGetRootElement( document);
	if( !xmlStrcmp( root->name, (const xmlChar *)"GK-VECTORS"))
	{
		for( node = root->children; node; node=node->next)
		{
			if( !xmlStrcmp( node->name, (const xmlChar *)"NUMBER_OF_GK-VECTORS"))
			{
				key = xmlNodeListGetString( document, node->xmlChildrenNode, 1);
				ngkv = strtol( (char *)key, 0, 10);
				xmlFree( key);

				if( ngkv <= 0)
					FAIL( FAIL, "Failed to read ngkv");
				res = initialize_gkv( ngkv);
			}
			if( !xmlStrcmp( node->name, (const xmlChar *)"MAX_NUMBER_OF_GK-VECTORS"))
			{
				key = xmlNodeListGetString( document, node->xmlChildrenNode, 1);
				res->max_ngkv = strtol( (char *)key, 0, 10);
				xmlFree( key);
			}
			if( !xmlStrcmp( node->name, (const xmlChar *)"GAMMA_ONLY"))
			{
				key = xmlNodeListGetString( document, node->xmlChildrenNode, 1);
				if( key[0] == 'T')
					res->gamma_only = true;
				else
					res->gamma_only = false;
				xmlFree( key);
			}
			if( !xmlStrcmp( node->name, (const xmlChar *)"K-POINT_COORDS"))
			{
				key = xmlNodeListGetString( document, node->xmlChildrenNode, 1);
				char * endptr = (char *)key;
				for( int i=0; i<3; i++)
					qepp_sscanf_double( endptr, &res->kpt[i], &endptr);
				xmlFree( key);
			}
			if( !xmlStrcmp( node->name, (const xmlChar *)"INDEX"))
			{
				key = xmlNodeListGetString( document, node->xmlChildrenNode, 1);
				char * endptr = (char *)key;
				
				for( long int i=0; i<ngkv; i++)
					res->index[i] = strtol( endptr, &endptr, 10);
				xmlFree( key);
			}
			if( !xmlStrcmp( node->name, (const xmlChar *)"GRID"))
			{
				key = xmlNodeListGetString( document, node->xmlChildrenNode, 1);
				char * endptr = (char *)key;
				
				for( long int i=0; i<ngkv; i++)
				{
					for( int j=0; j<3; j++)
						res->grid[i][j] = strtol( endptr, &endptr, 10);
				}
				xmlFree( key);
			}
		}
	}
	xmlFreeDoc( document);

	*out_ptr = res;
	SUCCESS();
}

errh * read_egv_xml( const char * filename, egv ** out_ptr)
{
	egv * res = NULL;
	if( out_ptr == NULL)
		FAIL( NULL_OUT, " ");
	if( filename == NULL)
		FAIL( NULL_IN, " ");
	FILE * read = fopen(filename,"r");
	if( read == NULL)
		FAIL( OPEN_IN_FAIL, "%s", filename);
	fclose( read);

	xmlDoc		* document;
	xmlNode		* root, * node;
	xmlChar		* key;

	int n_bnd = 0;
	double e_t=1;

	document = xmlReadFile( filename, NULL, 0);
	root = xmlDocGetRootElement( document);
	if( !xmlStrcmp( root->name, (const xmlChar *)"Root"))
	{
		for( node = root->children; node; node=node->next)
		{
			if( !xmlStrcmp( node->name, (const xmlChar *)"INFO"))
			{
				key = xmlGetProp( node, (const xmlChar *)"nbnd");
				n_bnd = strtol( (char *)key, 0, 10);
				
				res = initialize_egv( n_bnd);

				xmlFree( key);
			}
			if( !xmlStrcmp( node->name, (const xmlChar *)"UNITS_FOR_ENERGIES"))
			{
				key = xmlGetProp( node, (const xmlChar *)"UNITS");
				if ( !xmlStrcmp( key, (const xmlChar *)"Hartree"))	e_t = HARTREE_TO_RY * RY_TO_EV;
				else if( !xmlStrcmp( key, (const xmlChar *)"Rydberg"))	e_t = RY_TO_EV;
				else WARN( "Failed to read energy units");				

				xmlFree( key);
			}
			if( !xmlStrcmp( node->name, (const xmlChar *)"EIGENVALUES"))
			{
				key = xmlNodeListGetString( document, node->xmlChildrenNode, 1);
				
				char * endptr = (char *)key;
				for( int i=0; i<n_bnd; i++)
					qepp_sscanf_double( endptr, &res->val[i], &endptr);

				xmlFree( key);
			}
			if( !xmlStrcmp( node->name, (const xmlChar *)"OCCUPATIONS"))
			{
				key = xmlNodeListGetString( document, node->xmlChildrenNode, 1);
				
				char * endptr = (char *)key;
				for( int i=0; i<n_bnd; i++)
					qepp_sscanf_double( endptr, &res->occ[i], &endptr);

				xmlFree( key);
			}
		}
	}
	xmlFreeDoc( document);

	if( res == NULL)
		FAIL( FAIL, "Failed to read eigenvalue file\n");
	res->e_t = e_t;

	*out_ptr = res;
	SUCCESS();
}
#else //__LIBXML
errh * read_data_file( const char * filename, data_file **); {FAIL( FAIL, "Only available with libxml2 compilation\n");}
errh * read_wfc_xml( ); {FAIL( FAIL, "Only available with libxml2 compilation\n");}
evc  * read_evc_xml( ); {FAIL( FAIL, "Only available with libxml2 compilation\n");}
errh * read_gkv_xml( ); {FAIL( FAIL, "Only available with libxml2 compilation\n");}
errh * read_egv_xml( ); {FAIL( FAIL, "Only available with libxml2 compilation\n");}
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

	long int pos = ftell( read);

	if( qepp_get_xml_param( &app, read, pos, "INFO", "nbnd"))		FAIL( FAIL, "Can't read nbnd");	n_bnd = app;
	res = initialize_wfc( n_bnd);
//QEPP_PRINT( "%li\n", n_bnd);
	if( qepp_get_xml_param( &app, read, pos, "INFO", "ngw"))
		FAIL( FAIL, "Can't read ngw");
	res->ngw = app;
	if( qepp_get_xml_param( &app, read, pos, "INFO", "igwx"))
		FAIL( FAIL, "Can't read igwx");
	res->igwx = app;
	if( qepp_get_xml_param( &app, read, pos, "INFO", "ik"))
		FAIL( FAIL, "Can't read ik");
	res->ik = app;
	if( qepp_get_xml_param( &app, read, pos, "INFO", "nk"))
		FAIL( FAIL, "Can't read nk");
	res->nk = app;
	if( qepp_get_xml_param( &app, read, pos, "INFO", "ispin"))
		FAIL( FAIL, "Can't read ispin");
	res->ispin = app;
	if( qepp_get_xml_param( &app, read, pos, "INFO", "nspin"))
		FAIL( FAIL, "Can't read nspin");
	res->nspin = app;
	if( qepp_get_xml_param( &app, read, pos, "INFO", "scale_factor"))
	FAIL( FAIL, "Can't read scale");
	res->scale_factor = app;
		
	for( int i=0; i<n_bnd; i++)
	{
//QEPP_PRINT( "%d\n", i);
		if( (res->evc_vect[i] = read_evc_dat( read, i+1)) == NULL)
			FAIL( FAIL, "Failed to read wavefunction");
	}
	


	fclose( read);

	*out_ptr = res;
	SUCCESS();
}

evc * read_evc_dat( FILE * read, int n)
{
	double app1;
	double complex * app;
	char needle[64];
	long int size;
	//void * dump = malloc( 12);
	evc * res = NULL;
	if( read == NULL)
		return NULL;

	long int pos = ftell( read);
	sprintf( needle, "evc.%d", n);
	if( qepp_get_xml_param( &app1, read, ftell( read), needle, "size")) return NULL;	size = app1;

	res = initialize_evc( size);

	if( qepp_get_xml_value( (void **)&app, read, pos, needle, size, sizeof( double complex), dump_size))
	{
		FREE( res);
		return NULL;
	}
	memcpy( &res->val[0], app, size*sizeof( double complex));
	free( app);

	return res;
}

errh * read_gkv_dat( const char * filename, gkv ** out_ptr)
{
	long int pos;
	long int size;
	double app;
	int kind;
	long int * app_l=NULL;
	int * app_i=NULL;
	long int ngkv;

	gkv * res = NULL;
	if( out_ptr == NULL)
		FAIL( NULL_OUT, " ");
	if( filename == NULL)
		FAIL( NULL_IN, " ");
	FILE * read = fopen( filename, "rb");
	if( read == NULL)
		FAIL( OPEN_IN_FAIL, "%s", filename);

	//read ngkw
	pos = ftell( read);
	if( qepp_get_xml_param( &app, read, pos, "NUMBER_OF_GK-VECTORS", "kind"))
		FAIL( FAIL, " ");
	kind = app;
	switch( kind)
	{
	case 4:
		if( qepp_get_xml_value( (void **)&app_i, read, pos, "NUMBER_OF_GK-VECTORS", 1, kind, dump_size)) 
			FAIL( FAIL, "Can't read ngkv");
		ngkv = *app_i;
		free( app_i);
		break;
	case 8:
		if( qepp_get_xml_value( (void **)&app_i, read, pos, "NUMBER_OF_GK-VECTORS", 1, kind, dump_size)) 
			FAIL( FAIL, "Can't read ngkv");
		ngkv = *app_l;
		free( app_l);
		break;
	default:
		FAIL( FAIL, "kind for ngkw not implemented");
	}
	if( ngkv<0)
		FAIL( FAIL, "Negative ngkv");
	res = initialize_gkv( ngkv);

	//read max_ngkv
	pos = ftell( read);
	if( qepp_get_xml_param( &app, read, pos, "MAX_NUMBER_OF_GK-VECTORS", "kind"))
		FAIL( FAIL, " ");
	kind = app;
	switch( kind)
	{
	case 4:
		if( qepp_get_xml_value( (void **)&app_i, read, pos, "MAX_NUMBER_OF_GK-VECTORS", 1, kind, dump_size)) 
			FAIL( FAIL, "Can't read ngkv");
		res->max_ngkv = *app_i;
		free( app_i);
		break;
	case 8:
		if( qepp_get_xml_value( (void **)&app_i, read, pos, "MAX_NUMBER_OF_GK-VECTORS", 1, kind, dump_size)) 
			FAIL( FAIL, "Can't read ngkv");
		res->max_ngkv = *app_l;
		free( app_l);
		break;
	default:
		FAIL( FAIL, "kind for ngkw not implemented");
	}

	//read kpt coord
	double * kpt = NULL;
	if( qepp_get_xml_value( (void **)&kpt, read, pos, "K-POINT_COORDS", 3, sizeof( double), dump_size)) 
		FAIL( FAIL, "Can't read ngkv");
	memcpy( res->kpt, kpt, 3 * sizeof( double));
	free( kpt);

	//read index
	pos = ftell( read);
	if( qepp_get_xml_param( &app, read, pos, "INDEX", "size"))
		FAIL( FAIL, " ");
	size = app;
	if( qepp_get_xml_value( (void **)&app_i, read, pos, "INDEX", size, sizeof( int), dump_size)) 
		FAIL( FAIL, "Can't read index");
	memcpy( res->index, app_i, size * sizeof( int));
	free( app_i);

	//read grid
	pos = ftell( read);
	if( qepp_get_xml_param( &app, read, pos, "GRID", "size"))
		FAIL( FAIL, " ");
	size = app;
	if( qepp_get_xml_value( (void **)&app_i, read, pos, "GRID", size, sizeof( int), dump_size)) 
		FAIL( FAIL, "Can't read grid");
	memcpy( &res->grid[0][0], app_i, size * sizeof( int));
	free( app_i);

	fclose( read);

	*out_ptr = res;
	SUCCESS();
}


















