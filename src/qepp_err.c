#include <qepp/qepp_err.h>

ecode parse_errh( errh * e)
{
	ecode app = e->code;
	if( errf != NULL)
	{
		switch( e->code)
		{
		case SUCCESS:
			if( verbosity >= 4) QEPP_ALL_OUT( errf, "%s(): SUCCESS\n", e->cname);
			break;
		case WARNING:
			if( verbosity >= 3) QEPP_ALL_OUT( errf, "%s(): WARNING:", e->cname);
			break;
		/*case MSG:
			if( verbosity >= 4) QEPP_ALL_OUT( errf, "%s(): ", e->cname);
			break;*/
		case FAIL:
			QEPP_ALL_OUT( errf, "%s(): FAIL:", e->cname);
			break;
		case NULL_IN:
			if( verbosity >= 2) QEPP_ALL_OUT( errf, "%s(): passing null pointer for input", e->cname);
			break;
		case NULL_OUT:
			if( verbosity >= 2) QEPP_ALL_OUT( errf, "%s(): passing null pointer for output", e->cname);
			break;
		case OPEN_IN_FAIL:
			if( verbosity >= 2) QEPP_ALL_OUT( errf, "%s(): Failed to open input file: ", e->cname);
			break;
		default:
			QEPP_ALL_OUT( errf, "%s: Case not implemented...yet...", e->cname);
			break;
		}
		if( e->ext && verbosity >= 4)
			QEPP_ALL_OUT( errf, "  %s", e->extra);
		QEPP_ALL_OUT( errf, "\n");
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













