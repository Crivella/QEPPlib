#include <qepp/qepp_struct.h>

extern unsigned long int TOT_MEM;
extern unsigned int ionode;
extern int verbosity;

cd * initialize_cd(long int x, long int y, long int z)
{
	cd * to_init = malloc( sizeof( cd));
	to_init->typeID = ID_CD;
	to_init->size = sizeof( cd);

	to_init->x = x;
	to_init->y = y;
	to_init->z = z;

	to_init->dens = 	(double ***)AllocateLinearMem3( sizeof( double), x, y, z);
	to_init->coord = 	(double ****)AllocateLinearMem4( sizeof( double), x, y, z, 3);	
	QEPP_PRINT( "Allocating %.2lf MB of mem for cd struct...\n", sizeof(double)*x*y*z*4./(1024.*1024.));

	to_init->print =	&print_cd;
	to_init->duplicate =	&duplicate_cd;
	to_init->free = 	&free_cd;

	return to_init;
}

void * print_cd( cd * to_print, FILE * write)
{
	if( to_print == NULL)
		return NULL;
	if( write == NULL)
		return NULL;

	long int x1,y1,z1;
	x1 = to_print->x;
	y1 = to_print->y;
	z1 = to_print->z;
	double app2, *ptr;

	QEPP_OUT( write, "#%15s%16s%16s%16s%16s%16s\n", "x", "y", "z", "c_dens", "z_index", "xy_proj");
	for( int i1=0; i1<x1; i1++) for( int i2=0; i2<y1; i2++)
	{
		app2 = 0;
		for( int i3=0; i3<z1; i3++)
		{
			
			app2 += to_print->dens[i1][i2][i3];
			ptr = to_print->coord[i1][i2][i3];
			if( i3 == z1-1)
				QEPP_OUT( write, "%16g%16g%16g%16g%16d%16g\n", ptr[0], ptr[1], ptr[2], to_print->dens[i1][i2][i3],i3, app2);
			else
				QEPP_OUT( write, "%16g%16g%16g%16g%16d\n", ptr[0], ptr[1], ptr[2], to_print->dens[i1][i2][i3],i3);
		}
	}

	return NULL;
}

cd * duplicate_cd( cd * to_dupl)
{
	if( to_dupl == NULL)
		return NULL;

	cd * new_s = initialize_cd(to_dupl->x,to_dupl->y,to_dupl->z);
	memcpy( new_s, to_dupl, sizeof( cd));

	return new_s;
}

void * free_cd( cd * to_free)
{
	if( to_free == NULL)
		return NULL;

	FreeLinearMem3( (void ***)to_free->dens);
	FreeLinearMem4( (void ****)to_free->coord);

	free( to_free);

	return NULL;
}


































