// do a parallel add
__kernel void VecAdd(__global float *a, __global float *b, __global float *out)
{
	int iproc = get_global_id(0);
	out[iproc] = a[iproc] + b[iproc];
}
