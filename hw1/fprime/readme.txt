PROBLEM2: Find Prime
fprime.c 为原始代码
fprime_odd.c 不再重复筛偶数
openmp优化在fprime_odd.c基础上进行

运行make输出运行时间到benchmark

本机benchmark.example运行环境:
	corei7-6500U @ 2.50GHz 双核
	Linux Subsystem for Windows
	gcc 4.8.4

本机运行结果：
	直接优化算法取得效果较好，使用并行带来的优化效果并不显著。
