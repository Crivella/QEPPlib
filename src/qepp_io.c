#include <qepp/qepp_io.h>

char io_buff[1024];
char outdir[1024];
char prefix[128];
char workdir[1024];
char datafile_path[1024];
FILE * in_f;
FILE * errf;
FILE * outf;
unsigned int ionode;
int verbosity;

void open_io_env( int node, int mode, int verb)
{
	struct pollfd fds[1];
	fds[0].fd = 0;
	fds[0].events = POLLIN;

	initialize_mpi_data();

	ionode = node;
	int max = mpi->world_size;

	if( ionode >= max)
	{
		fprintf( stderr, "Ionode number exceed max number of proc\n");
		exit( 1);
	}

	verbosity = verb;
	char buff1[256];
	char buff2[256];
	switch( mode)
	{
	case -1:
#ifdef __MPI
		sprintf( buff1, "err_%d.log", mpi->world_rank);
		sprintf( buff2, "out_%d.log", mpi->world_rank);
#else
		sprintf( buff1, "err_s.log");
		sprintf( buff1, "out_s.log");
#endif
		errf = fopen( buff1, "w");
		outf = fopen( buff2, "w");
		break;
	/*case 0:
#ifdef __MPI
		if( mpi->world_rank == ionode)
		{
#endif
			errf = stderr;
			outf = stdout;
#ifdef __MPI
		}
#endif
		break;*/
	default:
		errf = stderr;
		outf = stdout;
		break;
	}


	int check=0;
	if( mpi->world_rank == ionode)
	{
		if( poll( fds, 1, 0) == 0)	//stdin is empty
		{
			printf( "stdin is empty\n");
		}
		else //stdin contains data
		{
			/*check=1;
			char buff[4096];
			char * ptr;
			if( fread( buff, 1, sizeof( buff), stdin) ) {}
			qepp_trim_ws( buff);
			ptr = strstr( buff, "outdir=");
			if( qepp_get_str( ptr, outdir))
			{
				fprintf( stderr, "Error reading tmp from file\n");
				exit(1);
			}
			ptr = strstr( buff, "prefix=");
			if( qepp_get_str( ptr, prefix))
			{
				fprintf( stderr, "Error reading prefix from file\n");
				exit(1);
			}
			ptr = strstr( buff, "workdir=");
			if( qepp_get_str( ptr, workdir))
			{
				if( getcwd( workdir, sizeof( workdir))) {};
				fprintf( stderr, "Error reading workdir from file. Using current dir '%s'\n",workdir);
				//exit(1);
			}
			printf( "tmp = %s\n", outdir);
			printf( "prefix = %s\n", prefix);
			printf( "workdir = %s\n", workdir);
			sprintf( datafile_path, "%s/%s.save/data-file.xml", outdir, prefix);
			if( !qepp_is_file( datafile_path))
				sprintf( datafile_path, "%s/%s.save/data-file-schema.xml", outdir, prefix);
			if( !qepp_is_file( datafile_path))
			{
				fprintf( stderr, "Can't find any datafile correspondig to version of espresso > 5.0\n");
				exit(1);
			}
			printf( "df = %s\n", datafile_path);
*/
			//QEPP_ALL_PRINT("sono %d\n", mpi->world_rank);

			//READ( &df, datafile_path);
			//df->print(df,stdout);//PRINT_DATA( df, stdout);	
		}	
	}

	mp_bcast( &check, MPI_COMM_WORLD, 1);
	if( check)
	{
		mp_bcast( outdir, MPI_COMM_WORLD);
		mp_bcast( prefix, MPI_COMM_WORLD);
		mp_bcast( workdir, MPI_COMM_WORLD);
		mp_bcast( datafile_path, MPI_COMM_WORLD);

		//READ( &df, datafile_path);
	}
	//READ( &df, datafile_path);
	//df->print(df,stdout);//PRINT_DATA( df, stdout);	


	//df->print(df,stdout);//PRINT_DATA( df, stdout);	
	//printf( "tmp = %s\n", outdir);
	//printf( "prefix = %s\n", prefix);
	//printf( "workdir = %s\n", workdir);


	return ;
}

void close_io_env()
{
	free_mpi_data( mpi);
	if( errf != stderr && errf != NULL)
		fclose( errf);
	if( outf != stdout && outf != NULL)
		fclose( outf);

	return ;
}

void qepp_print( int proc, void * data, FILE * w)
{
	if( w == NULL)
	{
		if( outf != NULL)
			w = outf;
		else
			return;
	}

#ifdef __MPI
	if( proc == -1 || proc == mpi->world_rank)
	{
#endif
		fprintf( w, "%s", (char *)data);
		fflush( w);
#ifdef __MPI
	}
#endif


	return ;
}

















