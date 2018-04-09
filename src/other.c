#include <qepp/other.h>


extern unsigned long int TOT_MEM;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Chech if a1<=check<=a2  or  a2<=check<=a1
int check_interval(double a1, double a2, double check)
{
	if( a2 <= a1 && a2 <= check && check <= a1)
		return 1;
	if ( a1 < a2 && a1 <= check && check <= a2)
		return 1;
	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Power of 10 (integer exponent)
double my_pow10(int n)
{
	if(n==0)
		return (double)1;
	if(n<0)
		return (double)0.1*my_pow10(n+1);
	else
		return (double)10*my_pow10(n-1);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Generate spherical grid arount point center of radius rad, with spacing mesh
double ** generate_spherical_grid(double * center, double rad, double * mesh)
{
	double VOLUME = 4.0/3.0 * PI * pow(rad,3);
	int n_kpt = ceil( VOLUME / (mesh[0]*mesh[1]*mesh[2]));
	double ** results = (double **)AllocateLinearMem2( sizeof( double), n_kpt, 3);
	/*malloc( n_kpt * sizeof(double *));;
	for( int i=0; i < n_kpt; i++)
		results[i]=malloc( 3 * sizeof(double));*/
	
	double scan_coord[3];
	int count=0;
	double up_limit[3];
	for(int i=0; i < 3; i++)
		up_limit[i]=center[i]+rad;

	scan_coord[2]=center[2]-rad;
	while(scan_coord[2]<=up_limit[2])
	{
		scan_coord[1]=center[1]-rad;
		while(scan_coord[1]<=up_limit[1])
		{
			scan_coord[0]=center[0]-rad;
			while(scan_coord[0]<=up_limit[0])
			{
				if(delta_k(scan_coord, center)<=rad)
				{
					for(int i=0; i < 3;i++)
						results[count][i]=scan_coord[i];
					count++;
					if(count>=n_kpt)
						return 0;
				}
				scan_coord[0]+=mesh[0];
			}
			scan_coord[1]+=mesh[1];
		}
		scan_coord[2]+=mesh[2];
	}
	results[count]=0;		

	return results;
}

double ** generate_spherical_surf( double * center, double rad, double teta, double fi)
{
	double ** res = NULL;

	long int n_t = ceil(fabs( TWOPI/teta));
	long int n_f = ceil(fabs( PI/fi))+1;
	long int n_pt = n_t*n_f;
	res = (double **)AllocateLinearMem2( sizeof( double), n_pt+1, 3);

	long int count=0;
	double a1=0, a2=0;
	for( long int i=0; i<n_t; i++)
	{
		a2=0;
		for( long int j=0; j<n_f; j++)
		{
			res[count][0] = center[0] + rad * sin( a2) * cos( a1);
			res[count][1] = center[1] + rad * sin( a2) * sin( a1);
			res[count][2] = center[2] + rad * cos( a2);

			count++;
			if( i != 0)
			{
				if( fabs( 0 - a2) < 1.E-4)
					count--;
				else if( fabs( PI - a2) < 1.E-4)
					count--;
			}
			if( count >n_pt)
			{
				WARN( "N_pt exceeding max value %li...", n_pt);
			}

			a2 += fi;
		}
		a1 += teta;
	}
	res[count] = NULL;



	return res;
}













