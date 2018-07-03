#include <math.h>
#include <qepp/qepp_mem.h>
#include <qepp/qepp_constants.h>

int ylmr( int lm, double * K, double ** res)
{
	double cost, sint;
	int lmax;
	double k_mod;
	double fi;
	double N;

	double ** Q;
	double * ylm;
	/*********************************************************************************
	Useful formulas:
	Y_{l,m} = sqrt[(2l+1)/(4 PI)] * sqrt[(l+m)!/(l-m)!] * P_{l,m} * e^{i*m*fi}
	P_{l,m} = Associated Legendre polinomials  ->  Q_{l,m} = sqrt[(l-m)!/(l+m)!]
	(l-m) * Y_{l,m} = (2l-1) * Y_{l-1,m} - (l-1 + m) * Y_{l-2,m}
	sqrt( l^2 - m^2) * Q_{l,m} = (2l-1) * Q_{l-1,m} - sqrt[(l-1)^2 - m^2] * Q_{l-2,m}

	Y_{l,m} real = Y_{l,m} + Y_{l,-m}       = sqrt[(2l+1)/(4 PI)] * sqrt[(l+m)!/(l-m)!] * P_{l,m} * cos(m*fi)
	Y_{l,m} complex = i(Y_{l,m} - Y_{l,-m}) = sqrt[(2l+1)/(4 PI)] * sqrt[(l+m)!/(l-m)!] * P_{l,m} * sin(m*fi)
	
	Y^{real}_{l,m} = sqrt[(2l+1)/(4 PI)] * Q_{l,m} * cos(m*fi)
	Y^{comp}_{l,m} = sqrt[(2l+1)/(4 PI)] * Q_{l,m} * sin(m*fi)
	**********************************************************************************/
	

	for( lmax=0; (lmax+1)*(lmax+1) != lm; lmax++)
		if( lmax > 25)
			return 1;

	if( lmax == 0)
		 return sqrt( 1./ PI);

	Q   = QEPP_ALLOC( sizeof( double), lmax+1, lmax+1);
	ylm = QEPP_ALLOC( sizeof( double), lm);
	
	k_mod = 0;
	for( int i=0; i<3; i++)
		k_mod += pow( K[i], 2);
	k_mod = sqrt( k_mod);


	cost = K[2] / k_mod;
	if( ! isnormal( cost))
		cost = 0.;
	sint = sqrt( 1 - pow( cost, 2));

	fi = atan( K[1] / K[0]);
	if( ! isnormal( fi))
		fi = 0.;
	if( fi < 0)
		fi += PI;
	if( K[1] < 0)
		fi += PI;

	Q[0][0] = 1.;
	Q[1][0] = cost;
	Q[1][1] = -sint/sqrt(2);

	double sq2 = sqrt( 2.);

	for( int l=2; l<=lmax; l++)
	{
		int lim=l-2;
		for( int m=0; m<=lim; m++)
			Q[l][m] = 1./sqrt( l*l - m*m) * ( (2*l-1) * cost * Q[l-1][m] - sqrt( (l-1)*(l-1) - m*m) * Q[l-2][m]);
		Q[l][l-1] = cost * sqrt( 2*l-1) * Q[l-1][l-1];
		Q[l][l]   = sqrt( 2*l-1) / sqrt( 2*l) * sint * Q[l-1][l-1];
	}

	for( int l=0; l<=lmax; l++)
	{
		N = sqrt( (2*l+1) / PI);
		int l2 = l*l;
		ylm[ l2] = N * Q[l][0];
		for( int m=1; m<=lmax; m++)
		{
			ylm[l2 + 2*m-1] = N * sq2 * Q[l][m] * cos( m*fi);
			ylm[l2 + 2*m]   = N * sq2 * Q[l][m] * sin( m*fi);
		}
	}
	

	QEPP_FREE( Q);

	*res = ylm;
	return 0;
}




























