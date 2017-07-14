length = 1 100 10000 1000000 100000000

.PHONY: profile

profile: comm_time.c
	mpicc -o comm_time comm_time.c
	echo "Length   Time(s)   Bandwidth(Mbps)" > profile
	for len in $(length) ; do \
	  mpiexec -np 2 ./comm_time $$len >> profile ; done
	rm comm_time
