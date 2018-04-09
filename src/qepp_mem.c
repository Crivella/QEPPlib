#include <qepp/qepp_mem.h>

extern unsigned long int TOT_MEM;

long int get_size( void * a)
{
	long int res=1;
	int dim = BASE_DIM(a);
	long int * ptr = *((long int **)BASE_ADDR(a));

	for( int i=0; i<dim; i++)
		res *= *(ptr+2+i);
	return res;
}

void * AllocateLinearMem1( unsigned long int size, long int s1)
{
	long int memapp=0, prod=0, dim=1;
	void * new;

	prod = s1;
	memapp = prod *size;
	assert ( ! (memapp == 0));

	char * app = malloc( memapp + sizeof( long int *));
	long int ** app1 = (long int **)app;
	app1[0] = malloc( (2+dim) * sizeof( long int));
	app1[0][0] = dim;
	app1[0][1] = size;
	app1[0][2] = s1;

	new = app + sizeof( long int *);

	//TOT_MEM += s1*size + sizeof( long int *) * (1) + sizeof( long int) * (2+dim);
	
	return new;
}

void ** AllocateLinearMem2( unsigned long int size, long int s1, long int s2)
{
	long int memapp=0, prod=0, dim=2;
	void ** new;

	prod = s1*s2;
	memapp = prod * size;
	assert ( ! (memapp == 0));

	char * app = malloc( memapp + sizeof( long int *));
	long int ** app1 = (long int **)app;
	app1[0] = malloc( (2+dim) * sizeof( long int));
	app1[0][0] = dim;
	app1[0][1] = size;
	app1[0][2] = s1;
	app1[0][3] = s2;

	memapp = s1 * sizeof( void *);
	new = malloc( memapp);
	for( int i1=0; i1<s1; i1++)
	{
		int idx = i1*s2;
		new[i1] = app + idx * size + sizeof( long int *);
	}

	//TOT_MEM += s1*s2*size + sizeof( long int *) * (1+s1) + sizeof( long int) * (2+dim);
	
	return new;
}

void *** AllocateLinearMem3( unsigned long int size, long int s1, long int s2, long int s3)
{
	long int memapp=0, prod=0, dim=3;
	void *** new;

	prod = s1*s2*s3;
	memapp = prod * size;
	assert ( ! (memapp == 0));

	char * app = malloc( memapp + sizeof( long int *));
	long int ** app1 = (long int **)app;
	app1[0] = malloc( (2+dim) * sizeof( long int));
	app1[0][0] = dim;
	app1[0][1] = size;
	app1[0][2] = s1;
	app1[0][3] = s2;
	app1[0][4] = s3;

	memapp = s1 * sizeof( void **);
	new = malloc( memapp);
	for( int i1=0; i1<s1; i1++)
	{
		memapp = s2 * sizeof( void *);
		new[i1] = malloc( memapp);
		for( int i2=0; i2<s2; i2++)
		{
			int idx = i1*s2*s3 + i2*s3;
			new[i1][i2] = app + idx * size + sizeof( long int *);
		}
	}

	//TOT_MEM += s1*s2*s3*size + sizeof( long int *) * (1+s1+s2) + sizeof( long int) * (2+dim);
	
	return new;
}

void **** AllocateLinearMem4( unsigned long int size, long int s1, long int s2, long int s3, long int s4)
{
	long int memapp=0, prod=0, dim=4;
	void **** new;

	prod = s1*s2*s3*s4;
	memapp = prod * size;
	assert ( ! (memapp == 0));

	char * app = malloc( memapp + sizeof( long int *));
	long int ** app1 = (long int **)app;
	app1[0] = malloc( (2+dim) * sizeof( long int));
	app1[0][0] = dim;
	app1[0][1] = size;
	app1[0][2] = s1;
	app1[0][3] = s2;
	app1[0][4] = s3;
	app1[0][5] = s4;

	memapp = s1 * sizeof( void ***);
	new = malloc( memapp);
	for( int i1=0; i1<s1; i1++)
	{
		memapp = s2 * sizeof( void **);
		new[i1] = malloc( memapp);
		for( int i2=0; i2<s2; i2++)
		{
			memapp = s3 * sizeof( void *);
			new[i1][i2] = malloc( memapp);
			for( int i3=0; i3<s3; i3++)
			{
				int idx = i1*s2*s3*s4 + i2*s3*s4 + i3 *s4;
				new[i1][i2][i3] = app + idx * size + sizeof( long int *);
			}
		}
	}

	//TOT_MEM += s1*s2*s3*s4*size + sizeof( long int *) * (1+s1+s2+s3) + sizeof( long int) * (2+dim);
	
	return new;
}

void * DuplicateLinearMem1( void * a)
{
	void * res=NULL;
	void * app = BASE_ADDR(a);
	long int * ptr = *((long int **)app);
	long int s1 = ptr[2];
	long int tot = s1;
	long int size = ptr[1];

	char * new = malloc( size*tot + sizeof( long int *));
	int dim = 1;
	long int ** app1 = (long int **)new;
	memcpy( new, app, size*tot + sizeof( long int *));
	app1[0] = malloc( (2+dim) * sizeof( long int));
	memcpy( app1[0], ptr, (2+dim) * sizeof( long int));

	res = new + sizeof( long int *);

	//TOT_MEM += s1*size + sizeof( long int *) * (1) + sizeof( long int) * (2+dim);

	return res;
}

void ** DuplicateLinearMem2( void ** a)
{
	void ** res=NULL;
	void * app = BASE_ADDR(a[0]);
	long int * ptr = *((long int **)app);
	long int s1 = ptr[2];
	long int s2 = ptr[3];
	long int tot = s1*s2;
	long int size = ptr[1];

	char * new = malloc( size*tot + sizeof( long int *));
	int dim = 2;
	long int ** app1 = (long int **)new;
	memcpy( new, app, size*tot + sizeof( long int *));
	app1[0] = malloc( (2+dim) * sizeof( long int));
	memcpy( app1[0], ptr, (2+dim) * sizeof( long int));

	res = malloc( s1 * sizeof( void *));
	for( int i1=0; i1<s1; i1++)
	{
		int idx = i1*s2;
		res[i1] = new + idx * size + sizeof( long int *);
	}

	//TOT_MEM += s1*s2*size + sizeof( long int *) * (1+s1) + sizeof( long int) * (2+dim);

	return res;
}

void *** DuplicateLinearMem3( void *** a)
{
	void *** res=NULL;
	void * app = BASE_ADDR(a[0][0]);
	long int * ptr = *((long int **)app);
	long int s1 = ptr[2];
	long int s2 = ptr[3];
	long int s3 = ptr[4];
	long int tot = s1*s2*s3;
	long int size = ptr[1];

	char * new = malloc( size*tot + sizeof( long int *));
	int dim = 3;
	long int ** app1 = (long int **)new;
	memcpy( new, app, size*tot + sizeof( long int *));
	app1[0] = malloc( (2+dim) * sizeof( long int));
	memcpy( app1[0], ptr, (2+dim) * sizeof( long int));

	res = malloc( s1 * sizeof( void **));
	for( int i1=0; i1<s1; i1++)
	{
		res[i1] = malloc(  s2 * sizeof( void *));
		for( int i2=0; i2<s2; i2++)
		{
			int idx = i1*s2*s3 + i2*s3;
			res[i1][i2] = new + idx * size + sizeof( long int *);
		}
	}

	//TOT_MEM += s1*s2*s3*size + sizeof( long int *) * (1+s1+s2) + sizeof( long int) * (2+dim);

	return res;
}

void **** DuplicateLinearMem4( void **** a)
{
	void **** res=NULL;
	void * app = BASE_ADDR(a[0][0][0]);
	long int * ptr = *((long int **)app);
	long int s1 = ptr[2];
	long int s2 = ptr[3];
	long int s3 = ptr[4];
	long int s4 = ptr[5];
	long int tot = s1*s2*s3*s4;
	long int size = ptr[1];

	char * new = malloc( size*tot + sizeof( long int *));
	int dim = 4;
	long int ** app1 = (long int **)new;
	memcpy( new, app, size*tot + sizeof( long int *));
	app1[0] = malloc( (2+dim) * sizeof( long int));
	memcpy( app1[0], ptr, (2+dim) * sizeof( long int));

	res = malloc( s1 * sizeof( void ***));
	for( int i1=0; i1<s1; i1++)
	{
		res[i1] = malloc( s2 * sizeof( void **));
		for( int i2=0; i2<s2; i2++)
		{
			res[i1][i2] = malloc( s3 * sizeof( void *));
			for( int i3=0; i3<s3; i3++)
			{
				int idx = i1*s2*s3*s4 + i2*s3*s4 + i3 *s4;
				res[i1][i2][i3] = new + idx * size + sizeof( long int *);
			}
		}
	}

	//TOT_MEM += s1*s2*s3*s4*size + sizeof( long int *) * (1+s1+s2+s3) + sizeof( long int) * (2+dim);

	return res;
}


void FreeLinearMem1( void * a)
{
	if( a == NULL)
		return;
	void * app = BASE_ADDR(a);
	long int * ptr = *((long int **)app);
	long int s1 = ptr[2];
	long int size = ptr[1];
	int dim = ptr[0];

	free( ptr);	
	free( app);

	TOT_MEM -= s1*size + sizeof( long int *) * (1) + sizeof( long int) * (2+dim);

	return;
}

void FreeLinearMem2( void ** a)
{
	if( a == NULL)
		return;
	void * app = BASE_ADDR(a[0]);
	long int * ptr = *((long int **)app);
	long int s1 = ptr[2];
	long int s2 = ptr[3];
	long int size = ptr[1];
	int dim = ptr[0];

	free( a);
	free( ptr);
	free( app);

	TOT_MEM -= s1*s2*size + sizeof( long int *) * (1+s1) + sizeof( long int) * (2+dim);

	return;
}

void FreeLinearMem3( void *** a)
{
	if( a == NULL)
		return;
	void * app = BASE_ADDR(a[0][0]);
	long int * ptr = *((long int **)app);
	long int s1 = ptr[2];
	long int s2 = ptr[3];
	long int s3 = ptr[4];
	long int size = ptr[1];
	int dim = ptr[0];

	free( ptr);
	for( int i=0; i<s1; i++)
		free( a[i]);
	free( a);
	free( app);

	TOT_MEM -= s1*s2*s3*size + sizeof( long int *) * (1+s1+s2) + sizeof( long int) * (2+dim);

	return;
}

void FreeLinearMem4( void **** a)
{
	if( a == NULL)
		return;
	void * app = BASE_ADDR(a[0][0][0]);
	long int * ptr = *((long int **)app);
	long int s1 = ptr[2];
	long int s2 = ptr[3];
	long int s3 = ptr[4];
	long int s4 = ptr[5];
	long int size = ptr[1];
	int dim = ptr[0];

	free( ptr);
	for( int i=0; i<s1; i++)
	{
		for( int j=0; j<s2; j++)
			free( a[i][j]);
		free( a[i]);
	}
	free( a);
	free( app);

	TOT_MEM -= s1*s2*s3*s4*size + sizeof( long int *) * (1+s1+s2+s3) + sizeof( long int) * (2+dim);

	return;
}





