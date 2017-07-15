PROBLEM 1: MPI_Send & MPI_Recv

usage: 
    mpicc comm_time.c ; mpiexec -np 2 ./a.out (data length)
    或者 make 自动生成报告

本机benchmark.example运行环境:
	corei7-6500U @ 2.50GHz 双核
	Linux Subsystem for Windows
	gcc 4.8.4
	openmpi 1.6.5

注：
1)事先读写数组，使被传数据位于缓存中
2)重复操作取时间平均
