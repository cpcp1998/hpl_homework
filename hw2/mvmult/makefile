threads = 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32
problem_size = 1000 2000 4000 8000 16000 32000
out = | tee -a benchmark

all: benchmark
	rm vector* matrix* main gen test

gen: gen.c
	gcc -o gen gen.c

main: main.c
	mpicc -o main main.c

test: test.c
	mpicc -o test test.c

.PHONY: benchmark
benchmark: gen main test
	@echo -n "" > benchmark
	@echo -n 'size\t\t' $(out)
	@for size in $(problem_size) ; do \
	  echo -n "$$size\t\t" $(out) ; \
	  ./gen matrix$$size $$size $$size > /dev/null ; \
	  ./gen vector$$size $$size 1 > /dev/null ; \
	done
	@echo "" $(out)
	@for threads in $(threads) ; do \
	  echo -n "$$threads\t" $(out) ; \
	  mpiexec -np $$threads ./test $(out) ; echo -n '\t' $(out) ; \
	  for size in $(problem_size) ; do \
	    ln -s matrix$$size matrix ; ln -s vector$$size vector; \
	    mpiexec -np $$threads ./main $(out) ; echo -n '\t' $(out) ; \
	    rm matrix vector ; \
	  done ; \
	  echo "" $(out) ; \
	done
	
