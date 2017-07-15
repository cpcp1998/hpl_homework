length = 1 100 10000 1000000 100000000

.PHONY: benchmark

benchmark: comm_time.c
	mpicc -o comm_time comm_time.c
	echo "Length   Time(s)   Bandwidth(Mbps)" > benchmark
	for len in $(length) ; do \
	  mpiexec -np 2 ./comm_time $$len >> benchmark ; done
	rm comm_time
