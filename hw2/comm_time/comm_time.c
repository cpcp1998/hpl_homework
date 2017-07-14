#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define N 10

int main(int argc, char ** argv)
{
	int rank, count;
	char *data;
	double begin, end;
	double accu;
	int i;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	count = 0;
	if(argc == 2) count = atoi(argv[1]);
	if(count <= 0) exit(1);

	if(rank < 2) {
		data = (char *) malloc(count);
		if(data == NULL) exit(2);
	}
	accu = 0;
	MPI_Barrier(MPI_COMM_WORLD);

	for(i = 0; i < count; i++) data[i]=0; //force array data to be cached

	for(i = 0; i < N; i++)
	{	
		MPI_Barrier(MPI_COMM_WORLD);
		if(rank == 0) begin = MPI_Wtime();
		MPI_Barrier(MPI_COMM_WORLD);
		if (rank == 0) {
			MPI_Send(data, count, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
		}
		else if (rank == 1){
			MPI_Recv(data, count, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
		MPI_Barrier(MPI_COMM_WORLD);
		if(rank == 0) { 
			end = MPI_Wtime();
			accu += end - begin;
		}
	}
	if(rank == 0)
		printf("%9d%10.6f%15.6f\n", count, accu / N, count / accu * N / 1000000 * 8);
	MPI_Finalize();
	return 0;
}
