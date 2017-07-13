threads = 1 2 3 4
problem_size = 100 200 400 800 1600 3200
CC = gcc -O3

all: profile
	rm gen MatrixMultiply_None.c MatrixMultiply?.c mm* matrix_*
gen: gen.c
	$(CC) -o gen gen.c

MatrixMultiply_None.c: MatrixMultiply.c
	sed '/omp/d' MatrixMultiply.c > MatrixMultiply_None.c

MatrixMultiply1.c: MatrixMultiply.c
	for num in $(threads) ; do sed "s/threads_count/$$num/" MatrixMultiply.c > MatrixMultiply$$num.c ; done

mm: MatrixMultiply_None.c
	$(CC) -o mm MatrixMultiply_None.c

mm1: MatrixMultiply1.c
	for num in $(threads) ; do $(CC) -fopenmp -o mm$$num MatrixMultiply$$num.c ; done

.PHONY: profile
profile: gen mm mm1
	echo 'size\tNo omp\t1\t2\t3\t4' > profile
	for size in $(problem_size) ; do \
	  echo "size: $$size" ; \
	  echo -n "$$size\t" >> profile ; \
	  ./gen matrix_a $$size $$size ; ./gen matrix_b $$size $$size ; \
	  echo "No openmp" ; \
	  printf "`{ /usr/bin/time -f '%e\t' ./mm ; } 2>&1`" >> profile ; \
	   for num in $(threads) ; do  \
	    echo "threads: $$num" ; \
	    printf "`{ /usr/bin/time -f '%e\t' ./mm$$num ; } 2>&1`" >> profile ; \
	  done ; \
	  echo "" >> profile ; \
	done
	
