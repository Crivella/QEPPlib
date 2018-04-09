#include <qepp/qepp_err.h>

extern int verbosity;
extern FILE * errf;
#ifdef __MPI
extern mpi_data * mpi;
#endif
extern int ionode;

ecode parse_errh( errh * e)
{
	ecode app = e->code;
	if( errf != NULL)
	{
		switch( e->code)
		{
		case SUCCESS:
			if( verbosity >= 4) fprintf( errf, "%s(): SUCCESS\n", e->cname);
			break;
		case WARNING:
			if( verbosity >= 3) fprintf( errf, "%s(): WARNING:", e->cname);
			break;
		/*case MSG:
			if( verbosity >= 4) fprintf( errf, "%s(): ", e->cname);
			break;*/
		case FAIL:
			fprintf( errf, "%s(): FAIL:", e->cname);
			break;
		case NULL_IN:
			if( verbosity >= 2) fprintf( errf, "%s(): passing null pointer for input", e->cname);
			break;
		case NULL_OUT:
			if( verbosity >= 2) fprintf( errf, "%s(): passing null pointer for output", e->cname);
			break;
		case OPEN_IN_FAIL:
			if( verbosity >= 2) fprintf( errf, "%s(): Failed to open input file: ", e->cname);
			break;
		default:
			fprintf( errf, "%s: Case not implemented...yet...", e->cname);
			break;
		}
		if( e->ext && verbosity >= 4)
			fprintf( errf, "  %s", e->extra);
		fprintf( errf, "\n");
	}
	free( e);
	return app;
}

errh * set_errh( ecode c, const char n [], const char a [])
{
	errh * res = malloc( sizeof( errh));

	res->code = c;
	//res->cname = malloc( ( strlen(n)+1) * sizeof( char));
	strcpy( res->cname, n);
	if( a != NULL)
	{
		res->ext = true;
		strcpy( res->extra, a);
	}
	else
		res->ext = false;

	return res;
}













