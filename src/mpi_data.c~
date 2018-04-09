#include <qepp/qepp_struct.h>

extern unsigned long int TOT_MEM;
extern unsigned int ionode;
extern int verbosity;

#ifdef __MPI

extern mpi_data * mpi;

mpi_data * initialize_mpi_data()
{
	mpi_data * to_init;

	to_init = calloc( 1, sizeof( mpi_data));
	mpi = to_init;
	to_init->typeID = ID_MPI_DATA;
	to_init->size = sizeof( mpi_data);

	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &to_init->world_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &to_init->world_rank);
	MPI_Get_processor_name(to_init->processor_name, &to_init->name_len);
	to_init->ionode = 0;
	//printf("Process %d/%d: %s\n",to_init->world_rank,to_init->world_size,to_init->processor_name);

	to_init->print =	&print_mpi_data;
	to_init->duplicate =	&duplicate_mpi_data;
	to_init->free = 	&free_mpi_data;

	MPI_Barrier( MPI_COMM_WORLD);

	return to_init;
}

void * print_mpi_data( mpi_data * to_print, FILE * write)
{
	if( to_print == NULL)
		return NULL;
	if( write == NULL)
		return NULL;

	QEPP_OUT( write, "Tot:\t%d\nRank:\t%d\nName:\t%s\n",to_print->world_size,to_print->world_rank,to_print->processor_name);

	return NULL;
}

mpi_data * duplicate_mpi_data( mpi_data * to_dupl)
{
	if( to_dupl == NULL)
		return NULL;

	mpi_data * new_s=initialize_mpi_data();

	memcpy(new_s,to_dupl,to_dupl->size);

	return new_s;
}

void * free_mpi_data( mpi_data * to_free)
{
	if(to_free == NULL)
		return NULL;

	free(to_free);
	MPI_Finalize();

	return NULL;
}




#else //__MPI
void * initialize_mpi_data()
{
	return NULL;
}

#endif //__MPI




