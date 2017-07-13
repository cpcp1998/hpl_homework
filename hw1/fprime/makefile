threads = 1 2 3 4
problem_size = 1000 3000 10000 30000 100000 300000 1000000 3000000 10000000 30000000 100000000 300000000 1000000000
CC = gcc -O3


all: profile
	for num in $(threads) ; do rm fprime$$num* ; done
	rm fprime fprime_odd
fprime1.c: fprime_p.c
	for num in $(threads) ; do sed "s/threads_count/$$num/" fprime_p.c > fprime$$num.c ; done

fprime: fprime.c
	$(CC) -o fprime fprime.c

fprime_odd: fprime_odd.c
	$(CC) -o fprime_odd fprime_odd.c

fprime1: fprime1.c
	for num in $(threads) ; do $(CC) -fopenmp -o fprime$$num fprime$$num.c ; done

.PHONY: profile
profile: fprime fprime_odd fprime1
	echo 'size\t\torig\todd\t1\t2\t3\t4' > profile
	for size in $(problem_size) ; do \
	  echo "size: $$size" ; \
	  if [ $$size -ge 10000000 ] ; then echo -n "$$size\t" >> profile ; else echo -n "$$size\t\t" >> profile ; fi ; \
	  echo "orig" ; \
	  printf "`{ /usr/bin/time -f '%e\t' ./fprime $$size > /dev/null; } 2>&1`" >> profile ; \
	  echo "odd" ; \
	  printf "`{ /usr/bin/time -f '%e\t' ./fprime_odd $$size > /dev/null ; } 2>&1`" >> profile ; \
	   for num in $(threads) ; do  \
	    echo "threads: $$num" ; \
	    printf "`{ /usr/bin/time -f '%e\t' ./fprime$$num $$size > /dev/null ; } 2>&1`" >> profile ; \
	  done ; \
	  echo "" >> profile ; \
	done
	
