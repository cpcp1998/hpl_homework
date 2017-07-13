/* * This program uses the Sieve of Eratosthenes to determine the
 * number of prime numbers less than or equal to 'n'.
 *
 * Adapted from code appearing in "Parallel Programming in C with
 * MPI and OpenMP," by Michael J. Quinn, McGraw-Hill (2004).
 */

#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[])
{
	int count; /* Prime count */
	int first; /* Index of first multiple */
	int i;
	int index; /* Index of current prime */
	char *marked; /* Marks for 2,...,'n' */
	long long int n; /* Sieving from 2, ..., 'n' */
	long long int N; /* Size of sieve and loop bounds */
	int prime; /* Current prime */
	if (argc != 2) {
		printf ("Command line: %s <m>\n", argv[0]);
		exit (1);
	}
	n = atoi(argv[1]);
	N = n+1;

	marked = (char *) malloc (N); //alocate slots for numbers in range [0,n]
	if (marked == NULL) {
		printf ("Cannot allocate enough memory\n");
		exit (1);
	}

	for (i = 3; i < N; i+=2) marked[i] = 1;
	for (i = 4; i < N; i+=2) marked[i] = 0;
	marked[0] = 0;
	marked[1] = 0; // not primes
	marked[2] = 1;
	index = 3;
	prime = 3;
	do {
		for (i = prime * 3; i < N; i += prime * 2) marked[i] = 0;
		while (!marked[++index]) ;
		prime = index;
	} while (prime * prime <= n);

	count = 0;
	for (i = 0; i < N; i++)
		count += marked[i];
	printf ("\nThere are %d primes less than or equal to %lld\n\n", count, n);
	return 0;
}
