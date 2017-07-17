/*
*   Matrix multiplication
*/
#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <stdio.h>
#include <time.h>


/*
*   Function 'rerror' is called when the program detects an
*   error and wishes to print an appropriate message and exit.
*/

void rerror(char *s)
{
	printf("%s\n", s);
	exit(-1);
}

/*
*   Function 'allocate_vector", passed the number of rows,
*   allocates a vector of floats.
*/

void allocate_vector(float **subs, int rows) {
	int    i;
	float *lptr, *rptr;
	float *storage;

	*subs = (float *)malloc(rows * sizeof(float));
	return;
}

/*
*   Given the name of a file containing a vector of floats, function
*   'read_vector' opens the file and reads its contents.
*/

void read_vector(
	char    *s,          /* File name */
	float **subs,       /* vector */
	int     *m)          /* Number of rows in matrix */
{
	char     error_msg[80];
	FILE    *fptr;          /* Input file pointer */
	int      dummy;

	fptr = fopen(s, "rb");
	if (fptr == NULL) {
		sprintf(error_msg, "Can't open file '%s'", s);
		rerror(error_msg);
	}
	fread(m, sizeof(int), 1, fptr);
	fread(&dummy, sizeof(int), 1, fptr);
	allocate_vector(subs, *m);
	fread(*subs, sizeof(float), *m, fptr);
	fclose(fptr);
	return;
}


/*
*   Function 'allocate_matrix", passed the number of rows and columns,
*   allocates a two-dimensional matrix of floats.
*/

void allocate_matrix(float ***subs, int rows, int cols) {
	int    i;
	float *lptr, *rptr;
	float *storage;

	storage = (float *)malloc(rows * cols * sizeof(float));
	*subs = (float **)malloc(rows * sizeof(float *));
	for (i = 0; i < rows; i++)
		(*subs)[i] = &storage[i*cols];
	return;
}

/*
*   Given the name of a file containing a matrix of floats, function
*   'read_matrix' opens the file and reads its contents.
*/

void read_matrix(
	char    *s,          /* File name */
	float ***subs,       /* 2D submatrix indices */
	int     *m,          /* Number of rows in matrix */
	int     *n)          /* Number of columns in matrix */
{
	char     error_msg[80];
	FILE    *fptr;          /* Input file pointer */

	fptr = fopen(s, "rb");
	if (fptr == NULL) {
		sprintf(error_msg, "Can't open file '%s'", s);
		rerror(error_msg);
	}
	fread(m, sizeof(int), 1, fptr);
	fread(n, sizeof(int), 1, fptr);
	allocate_matrix(subs, *m, *n);
	fread((*subs)[0], sizeof(float), *m * *n, fptr);
	fclose(fptr);
	return;
}

void print_matrix(float **a, int rows, int cols)
{
	int i, j;

	for (i = 0; i < rows; i++) {
		for (j = 0; j < cols; j++)
			printf("%6.2f ", a[i][j]);
		putchar('\n');
	}
	putchar('\n');
	return;
}

void print_vector(float *a, int rows)
{
	int i;

	for (i = 0; i < rows; i++)
		printf("%6.2f ", a[i]);
	putchar('\n');
	putchar('\n');
	return;
}

void vector_multiply(float **a, float *b, float *c, int m, int n)
{
	int i;
	for (i = 0; i < m; i++)
	{
		int j;
		float temp = 0;
		for (j = 0; j < n; j++)
			temp += a[i][j] * b[j];
		c[i] = temp;
	}
	return;
}

int main(int *argc, char *argv[])
{
	int m, n;         /* Dimensions of matrix */
	int n2;           /* Dimensions of vector */
	float **a;        /* The matrix being multiplied */
	float *b;         /* The vector being multiplied */
	float *c;         /* Product vector */
	clock_t time0, time1;

	read_matrix("matrix", &a, &m, &n);
	//   print_matrix (a, m, n);
	read_vector("vector", &b, &n2);
	//   print_vector (b, n2);
	if (n != n2) rerror("Incompatible matrix dimensions");
	allocate_vector(&c, m);
	time0 = clock();
	vector_multiply(a, b, c, m, n);
	time1 = clock();
	printf("Problem size: %dx%d\n", m, n);
	printf("Calculating time: %6.2fms\n", (time1 - time0) / (double)CLOCKS_PER_SEC * 1000);
	printf("Speed: %6.2fGFLOPS\n", m*n * 2.0 / (time1 - time0) * CLOCKS_PER_SEC / 1000000000);
	//print_vector(c, m);
	return 0;
}
