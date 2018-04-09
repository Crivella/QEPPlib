#include <qepp/my_lapack.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Calculate module of the difference between 2 vectors
double delta_k(double a[3], double b[3])
{
	double app[3] = {0};
	if( b != NULL)
		for( int i=0; i<3; i++)
			app[i] = b[i];
	double diff=0;
	for(int i=0; i<3;i++)
		diff+=pow(a[i]-app[i],2);
	diff=sqrt(diff);
	return diff;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Compara 2 vettori. Restituisce 1 se uguali, 0 se diversi
int vect_comp(double *a, double *b)
{
	for(int i=0;i<3;i++)
		if(a[i]!=b[i])
				return 0;
	return 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Scalar Vector * Vector product (3x3) (Result can be the same vector)
void scalar_vector( double * a, double * b, double * res)
{
	*res = 0;
	double app1[3],app2[3];
	for(int i=0;i<3;i++)
	{
		app1[i]=a[i];
		app2[i]=b[i];
	}
	for(int i=0;i<3;i++)
		*res += app1[i] * app2[i];
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Vect Vector * Vector product (3x3) (Result can be the same vector)
void vector_vector( double * a, double * b, double * res)
{
	double app1[3],app2[3];
	for(int i=0;i<3;i++)
	{
		app1[i]=a[i];
		app2[i]=b[i];
	}
	for(int i=0;i<3;i++)
		res[i] = app1[(i+1)%3] * app2[(i+2)%3] - app1[(i+2)%3] * app2[(i+1)%3];
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Matrix * Vector product (3x3) (Result can be the same vector)
void matrix_vector(double ** matrix,double * vector, double *result)
{
	double app[3];
	for(int i=0;i<3;i++)
		app[i]=vector[i];
	for(int i=0;i<3;i++)
	{
		result[i]=0;
		for(int j=0;j<3;j++)
			result[i]+=matrix[i][j]*app[j];
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Matrix * Matrix product (3x3) (Result can be the same as the 2nd matrix)
void matrix_matrix(double ** matrix1,double ** matrix2, double ** result)
{
	double app[3][3];
	for(int i=0;i<3;i++)
	{
		for(int j=0;j<3;j++)
		{
			app[i][j]=matrix2[i][j];
			result[i][j]=0;
		}
	}
	for(int i=0;i<3;i++)
		for(int j=0;j<3;j++)
			for(int k=0;k<3;k++)
				result[i][j]+=matrix1[i][k]*app[k][j];
	
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Compara 2 matrici. Restituisce 1 se uguali, 0 se diverse
int matrix_comp(double ** a,double ** b)
{
	for(int i=0;i<3;i++)
		for(int j=0;j<3;j++)
			if(fabs(a[i][j]-b[i][j])>0.00001)
				return 0;
	return 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Restituisce dopo quante applicazioni della simmetria il vettore torna in se stesso
int symm_order(double ** matrix)
{
	double ** app = (double **)AllocateLinearMem2( sizeof( double), 3, 3);
	int a=0;
	for(int i=0;i<3;i++)
		for(int j=0;j<3;j++)
			app[i][j]=matrix[i][j];
	do
	{
		a++;
		matrix_matrix(matrix,app,app);
	}while(!matrix_comp(matrix,app) && a<30);

	FreeLinearMem2( (void **)app);
	return a;
}

