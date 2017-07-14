#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
	int p;
	int rank;
	MPI_Comm cart_comm;
	MPI_Comm row_comm;
	MPI_Comm col_comm;
	int periodic[2];
	int size[2];
	int coords[2];
	FILE *file;
	int rows, cols;
	int rowbegin, rowend, colbegin, colend, rowwid, colwid;
	float *a, *b, *c, *ret;
	int i, j;
	double timei,timeo;
	
	MPI_Init(&argc, &argv);
	timei = MPI_Wtime();
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	size[0] = size[1] = 0;
	MPI_Dims_create(p, 2, size);
	periodic[0] = periodic[1] = 0;
	MPI_Cart_create(MPI_COMM_WORLD, 2, size, periodic, 1, &cart_comm);
	MPI_Comm_rank(cart_comm, &rank);
	MPI_Cart_coords(cart_comm, rank, 2, coords);
//	printf("rank: %d\tx: %d\ty: %d\n", rank, coords[0], coords[1]);
//	fflush(stdout);
	MPI_Comm_split(cart_comm, coords[0], coords[1], &row_comm);
	MPI_Comm_split(cart_comm, coords[1], coords[0], &col_comm);
	file = fopen("matrix","r");
	if(file == NULL) {
		printf("Can't open file\n");
		exit(1);
	}
	fread(&rows, sizeof(int), 1, file);
	fread(&cols, sizeof(int), 1, file);
	rowbegin = rows * coords[0] / size[0];
	rowend = rows * (coords[0] + 1) / size[0];
	colbegin = cols * coords[1] / size[1];
	colend = cols * (coords[1] + 1) / size[1];
	rowwid = rowend - rowbegin;
	colwid = colend - colbegin;
//	printf("rank%d: x from %d to %d, y from %d to %d.\n",rank,rowbegin,rowend,colbegin,colend);
	a = (float *) malloc(rowwid * colwid * sizeof(float));
	b = (float *) malloc(colwid * sizeof(float));
	c = (float *) malloc(rowwid * sizeof(float));
	fseek(file, (colbegin + cols * rowbegin) * sizeof(float), SEEK_CUR);
	for(i = 0; i < rowwid; i++){
		fread(a + i * colwid, sizeof(float), colwid, file);
		fseek(file, (cols - colwid) * sizeof(float), SEEK_CUR);
	}
	fclose(file);
	if(coords[0] == 0){
		file = fopen("vector","r");
		if(file == NULL){
			printf("Can't open file\n");
			exit(1);
		}
		fseek(file, 2*sizeof(int) + colbegin * sizeof(float), SEEK_CUR);
		fread(b, sizeof(float), colwid, file);
		fclose(file);
	}
	MPI_Bcast(b, colwid, MPI_FLOAT, 0, col_comm);
	for(i = 0; i < rowwid; i++){
		float temp = 0;
		for(j = 0; j < colwid; j++) temp += a[i*colwid+j]*b[j];
		c[i] = temp;
	}
	ret = (float *) malloc(rows * sizeof(float));
	MPI_Allreduce(c, ret, rowwid, MPI_FLOAT, MPI_SUM, row_comm);

	int *rowwids, *rowdisp;
	rowwids = (int *) malloc(size[0] * sizeof(int));
	rowdisp = (int *) malloc(size[0] * sizeof(int));
	for(i = 0; i < size[0]; i++){
		rowdisp[i] = rows * i / size[0];
		rowwids[i] = rows * (i+1) / size[0] - rowdisp[i];
	}
	MPI_Allgatherv(ret, rowwid, MPI_FLOAT,
	            ret, rowwids, rowdisp, MPI_FLOAT, col_comm);
	if(coords[0] == 0 && coords[1] == 0){
//		for(i = 0; i < rows; i++)
//			printf("%6.2f\n",ret[i]);
		timeo = MPI_Wtime();
		printf("%f",timeo - timei);
	}
	MPI_Finalize();
	return 0;
}
