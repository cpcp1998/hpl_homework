#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
	int p;
	int rank;
	int size[2];
	
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	size[0] = size[1] = 0;
	MPI_Dims_create(p, 2, size);
	if (rank == 0) printf("%dx%d",size[0],size[1]);
	
	MPI_Finalize();
	return 0;
}
