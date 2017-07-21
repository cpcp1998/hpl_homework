/* Target: intel processor graphics with shared physical memory.
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <CL/cl.h>

void check_err(cl_int err, const char* description) {
	if (err < 0) {
		fprintf(stderr, "Failed: %s.\n",description);
		exit(1);
	}
	else {
		printf("Succeeded: %s.\n",description);
	}
}

double timediff(LARGE_INTEGER time0, LARGE_INTEGER time1) {
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	return (time1.QuadPart - time0.QuadPart) / (double)freq.QuadPart;
}

int main() {
	cl_platform_id platform;
	cl_device_id device;
	cl_context context;
	cl_command_queue queue;
	cl_program program;
	cl_kernel kernel;
	char device_name[100];
	char *kernel_code;
	cl_mem matrix_mem, invec_mem, outvec_mem;
	size_t global_size, local_size;
	cl_int err = 0;

	FILE *fp;
	char source_name[] = "./mvmult.cl";
	size_t source_size;

	char matrix_name[] = "./matrix";
	char vector_name[] = "./vector";
	int m, n; //actual size of the matrix
	int _m, _n; //allocated size for the matrix
	int _n_4; // n/4
	float *matrix;
	float *invec;
	float *outvec;

	int i, j;
	LARGE_INTEGER time0, time1, time2, time3;

	//read input file
	fp = fopen(matrix_name, "rb");
	fread(&m, sizeof(int), 1, fp);
	fread(&n, sizeof(int), 1, fp);
	//allocated at a 4096 byte boundary and a total size that is a multiple of 64 bytes 
	//as intel required for zero memory copy
	_m = m; _n = 16 * (1 + (n - 1) / 16);
	matrix = (float *)_aligned_malloc(_m * _n * sizeof(float), 4096);
	invec = (float *)_aligned_malloc(_n * sizeof(float), 4096);
	for (i = 0; i < m; i++) {
		fread(matrix + i*_n, sizeof(float), n, fp);
		for (j = n; j < _n; j++) matrix[i*_n + j] = 0;
	}
	fclose(fp);
	fp = fopen(vector_name, "rb");
	fseek(fp, 2 * sizeof(int), SEEK_CUR);
	fread(invec, sizeof(float), n, fp);
	for (i = n; i < _n; i++) invec[i] = 0;
	fclose(fp);

/*	//print input
	printf("Input Matrix %dx%d\n", m, n);
	for (i = 0; i < m; i++) {
		for (j = 0; j < n; j++) printf("%6.2f ", matrix[i*_n + j]);
		putchar('\n');
	}
	printf("Input Vector %d\n", n);
	for (i = 0; i < n; i++) printf("%6.2f ", invec[i]);
	putchar('\n');*/

	QueryPerformanceCounter(&time0);

	//reading kernel code
	fp = fopen(source_name, "r");
	if (!fp) {
		fprintf(stderr, "Failed to load kernel.\n");
		exit(1);
	}
	fseek(fp, 0, SEEK_END);
	source_size = ftell(fp);
	kernel_code = (char *)malloc(source_size + 1);
	rewind(fp);
	fread(kernel_code, 1, source_size, fp);
	fclose(fp);

	//routine of creating command_queue & kernel, assuming platform 0 with GPU
	err |= clGetPlatformIDs(1, &platform, NULL);
	err |= clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
	err |= clGetDeviceInfo(device, CL_DEVICE_NAME, sizeof(device_name), device_name, NULL);
	printf("Device detected: %s\n", device_name);
	context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
	queue = clCreateCommandQueue(context, device, NULL, &err);
	program = clCreateProgramWithSource(context, 1, (const char **)&kernel_code,
		(const size_t *)&source_size, &err);
	err = clBuildProgram(program, 1, &device, "", NULL, NULL);
	check_err(err, "compile program");
	kernel = clCreateKernel(program,"dot_product", &err);
	check_err(err, "create kernel");

	

	//create mem_obj
	matrix_mem = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, _m*_n * sizeof(float), matrix, &err);
	invec_mem = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, _n * sizeof(float), invec, &err);
	outvec_mem = clCreateBuffer(context, CL_MEM_WRITE_ONLY, _m * sizeof(float), NULL, &err);
	
	//set arguments
	_n_4 = _n / 4;
	err |= clSetKernelArg(kernel, 0, sizeof(cl_int), &_n_4);
	err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &matrix_mem);
	err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &invec_mem);
	err |= clSetKernelArg(kernel, 3, sizeof(cl_mem), &outvec_mem);
	
	//calculate on kernel
	printf("Calculating...\n");
	global_size = _m; 
	local_size = 256;
	QueryPerformanceCounter(&time1);
	clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global_size, NULL, 0, NULL, NULL);
	clFinish(queue);
	QueryPerformanceCounter(&time2);
	outvec = (float *)clEnqueueMapBuffer(queue, outvec_mem, CL_TRUE, CL_MAP_READ,0,m,0,NULL,NULL,&err);

	//print output
	//printf("Output Vector %d\n", n);
	//for (i = 0; i < m; i++) printf("%6.2f ", outvec[i]);
	//putchar('\n');

	//cleanup
	clEnqueueUnmapMemObject(queue, outvec_mem, outvec, 0, NULL, NULL);
	clReleaseMemObject(matrix_mem);
	clReleaseMemObject(invec_mem);
	clReleaseMemObject(outvec_mem);
	_aligned_free(matrix);
	_aligned_free(invec);
	clReleaseKernel(kernel);
	clReleaseProgram(program);
	clReleaseCommandQueue(queue);
	clReleaseContext(context);

	QueryPerformanceCounter(&time3);

	printf("Problem size: %dx%d\n", m, n);
	printf("OpenCL time: %6.2fms\n", timediff(time0,time3));
	printf("Calculating time: %6.6fms\n", timediff(time1,time2));
	printf("Speed: %6.2fGFLOPS\n", m*n * 2.0 / timediff(time1,time2) / 1000000000);

	return 0;
}