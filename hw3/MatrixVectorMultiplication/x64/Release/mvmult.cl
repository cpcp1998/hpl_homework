__kernel void
dot_product (const int size,
	__constant float4* restrict mat,
	__constant float4* restrict invec,
	__global float* restrict outvec)
{
	int i = get_global_id(0);
	int j;
	float res = 0;

	for(j=0;j<size;j++){
		res+=dot(mat[i*size+j],invec[j]);
	}
	outvec[i]=res;
}
