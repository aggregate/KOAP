
#include <CL/opencl.h>
#include <stdio.h>

#include "VecAdd.h"
#define SIMDS 4 
#define MAXBLOCK 8 
#define GLOBAL_WORK_SIZE SIMDS * MAXBLOCK
#define LOCAL_WORK_SIZE MAXBLOCK
/*
The above defines are defined for both this host code and for the OpenCL
code. Defines following this comment are local to this file.

 */
int checkError(cl_int errCode, char *fun)
{
	if (errCode != CL_SUCCESS)
	{
		printf("Error in %s %d\n", fun, errCode);
		return 0;
	}
	return 1;
}


#define NPROC GLOBAL_WORK_SIZE



#include <time.h>



int main(int argc, char **argv)
{
	float a[NPROC];
	float b[NPROC];
	float out[NPROC];

	int i;
	for (i = 0; i < NPROC; ++i)
	{a[i] = i; b[i] = i; out[i] = 0;}	

	
	for (i = 0; i < NPROC; ++i)
	{printf("%2.2f + %2.2f = %2.2f\n", a[i], b[i], out[i]);}

	
	cl_platform_id platid = NULL;
	cl_device_id cldev = NULL;
	cl_context cpuContext;
	cl_command_queue comQ;
	cl_program clprog;
	cl_int errCode = 0;
	cl_uint np;


	cl_kernel kernel_VecAdd;
	cl_kernel kernel_VecMul;

	char *source = kernelstr;
	char cBuffer[128];
	size_t ksize = sizeof(kernelstr);


	void *VecAdd_arg0p;
	cl_mem cl_VecAdd_arg0;

	void *VecAdd_arg1p;
	cl_mem cl_VecAdd_arg1;

	void *VecAdd_arg2p;
	cl_mem cl_VecAdd_arg2;

	void *VecMul_arg0p;
	cl_mem cl_VecMul_arg0;

	void *VecMul_arg1p;
	cl_mem cl_VecMul_arg1;

	void *VecMul_arg2p;
	cl_mem cl_VecMul_arg2;

	/* OpenCL Environment Setup */

	printf("Size of kernel code: %d bytes\n", (int)ksize);

		errCode =
	clGetPlatformIDs(0, NULL, &np);
	if (!checkError(errCode, "clGetPlatformIDs:")){return 0;}

	printf("Number of platforms: %d\n", np);

	errCode =
	clGetPlatformIDs(1, &platid, NULL);
	if (!checkError(errCode, "clGetPlatformIDs:")){return 0;}

		errCode =
	clGetPlatformInfo(platid, CL_PLATFORM_NAME, sizeof(cBuffer),
		cBuffer, NULL);
	if (!checkError(errCode, "clGetPlatformInfo:")){return 0;}

	printf("Platform = %s\n", cBuffer);

		errCode =
	clGetDeviceIDs(platid, CL_DEVICE_TYPE_GPU, 1, &cldev, NULL);
	if (!checkError(errCode, "clGetDeviceIDs:")){return 0;}

	cpuContext =
	clCreateContext(0, 1, &cldev, NULL, NULL, &errCode);
	if (!checkError(errCode, "clCreateContext:")){return 0;}

	comQ =
	clCreateCommandQueue(cpuContext, cldev, 0, &errCode);
	if (!checkError(errCode, "clCreateCommandQueue:")){return 0;}


	clprog =
	clCreateProgramWithSource(cpuContext, 1, (const char **)&source,
		&ksize, &errCode);
	if (!checkError(errCode, "clCreateProgramWithSource:")){return 0;}
	
	int buildError = 
	clBuildProgram(clprog, 0, NULL, "", NULL, NULL);
	cl_build_status stat;
	char opt[1000];
	char log[4000];

		errCode =
	clGetProgramBuildInfo (clprog, cldev, CL_PROGRAM_BUILD_STATUS,
		sizeof(cl_build_status), &stat, NULL);
	if (!checkError(errCode, "clGetProgramInfo:")){return 0;}

		errCode =
	clGetProgramBuildInfo (clprog, cldev, CL_PROGRAM_BUILD_OPTIONS,
		sizeof(opt), opt, NULL);
	if (!checkError(errCode, "clGetProgramInfo:")){return 0;}
   
		errCode =
	clGetProgramBuildInfo (clprog, cldev, CL_PROGRAM_BUILD_LOG,
		sizeof(log), log, NULL);
	if (!checkError(errCode, "clGetProgramBuildInfo:")){return 0;}

	printf("Program Build Info:\n");
	printf("Build Status\n%d\n", stat);
	printf("Build Flags\n%s\n", opt);
	printf("Build Log\n%s\n", log);
	if (!checkError(buildError, "clBuildProgram:")){return 0;}

	int lws = LOCAL_WORK_SIZE;
	int gws = GLOBAL_WORK_SIZE;


		cl_VecAdd_arg0=
	clCreateBuffer(cpuContext, CL_MEM_READ_ONLY,
		( sizeof(float) * NPROC ), NULL, &errCode);
	if (!checkError(errCode, "clCreateBuffer(VecAdd_0):")){return 0;}

	cl_VecAdd_arg1=
	clCreateBuffer(cpuContext, CL_MEM_READ_ONLY,
		( sizeof(float) * NPROC ), NULL, &errCode);
	if (!checkError(errCode, "clCreateBuffer(VecAdd_1):")){return 0;}

	cl_VecAdd_arg2=
	clCreateBuffer(cpuContext, CL_MEM_WRITE_ONLY,
		( sizeof(float) * NPROC ), NULL, &errCode);
	if (!checkError(errCode, "clCreateBuffer(VecAdd_2):")){return 0;}

	cl_VecMul_arg0=
	clCreateBuffer(cpuContext, CL_MEM_READ_ONLY,
		( sizeof(float) * NPROC ), NULL, &errCode);
	if (!checkError(errCode, "clCreateBuffer(VecMul_0):")){return 0;}

	cl_VecMul_arg1=
	clCreateBuffer(cpuContext, CL_MEM_READ_ONLY,
		( sizeof(float) * NPROC ), NULL, &errCode);
	if (!checkError(errCode, "clCreateBuffer(VecMul_1):")){return 0;}

	cl_VecMul_arg2=
	clCreateBuffer(cpuContext, CL_MEM_WRITE_ONLY,
		( sizeof(float) * NPROC ), NULL, &errCode);
	if (!checkError(errCode, "clCreateBuffer(VecMul_2):")){return 0;}

	kernel_VecAdd =
	clCreateKernel(clprog, "VecAdd", &errCode);
	if (!checkError(errCode, "clCreateKernel:")){return 0;}

	kernel_VecMul =
	clCreateKernel(clprog, "VecMul", &errCode);
	if (!checkError(errCode, "clCreateKernel:")){return 0;}

		errCode =
	clSetKernelArg(kernel_VecAdd, 0, sizeof(cl_mem), (void*)&cl_VecAdd_arg0);
	if (!checkError(errCode, "clSetKernelArg(0):")){return 0;}

		errCode =
	clSetKernelArg(kernel_VecAdd, 1, sizeof(cl_mem), (void*)&cl_VecAdd_arg1);
	if (!checkError(errCode, "clSetKernelArg(1):")){return 0;}

		errCode =
	clSetKernelArg(kernel_VecAdd, 2, sizeof(cl_mem), (void*)&cl_VecAdd_arg2);
	if (!checkError(errCode, "clSetKernelArg(2):")){return 0;}

		errCode =
	clSetKernelArg(kernel_VecMul, 0, sizeof(cl_mem), (void*)&cl_VecMul_arg0);
	if (!checkError(errCode, "clSetKernelArg(0):")){return 0;}

		errCode =
	clSetKernelArg(kernel_VecMul, 1, sizeof(cl_mem), (void*)&cl_VecMul_arg1);
	if (!checkError(errCode, "clSetKernelArg(1):")){return 0;}

		errCode =
	clSetKernelArg(kernel_VecMul, 2, sizeof(cl_mem), (void*)&cl_VecMul_arg2);
	if (!checkError(errCode, "clSetKernelArg(2):")){return 0;}



	struct timespec getCallStart;
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &getCallStart);

		VecAdd_arg2p = out;
	VecAdd_arg1p = b;
	VecAdd_arg0p = a;
	errCode =
	clEnqueueWriteBuffer(comQ, cl_VecAdd_arg0,CL_TRUE, 0,
	(sizeof(float) * NPROC), VecAdd_arg0p, 0, NULL, NULL);
	if (!checkError(errCode, "clEnqueueWriteBuffer(VecAdd_0):")){return 0;}

	errCode =
	clEnqueueWriteBuffer(comQ, cl_VecAdd_arg1,CL_TRUE, 0,
	(sizeof(float) * NPROC), VecAdd_arg1p, 0, NULL, NULL);
	if (!checkError(errCode, "clEnqueueWriteBuffer(VecAdd_1):")){return 0;}

	errCode =
	clEnqueueNDRangeKernel(comQ, kernel_VecAdd, 1, NULL, (const size_t *) &gws,
		(const size_t *) &lws, 0, NULL, NULL);
	if (!checkError(errCode, "clEnqueueNDRangeKernel(VecAdd):")){return 0;}

	errCode =
	clEnqueueReadBuffer(comQ, cl_VecAdd_arg2,CL_TRUE, 0,
	(sizeof(float) * NPROC), VecAdd_arg2p, 0, NULL, NULL);
	if (!checkError(errCode, "clEnqueueReadBuffer(VecAdd_2):")){return 0;}


		VecMul_arg2p = out;
	VecMul_arg1p = b;
	VecMul_arg0p = a;
	errCode =
	clEnqueueWriteBuffer(comQ, cl_VecMul_arg0,CL_TRUE, 0,
	(sizeof(float) * NPROC), VecMul_arg0p, 0, NULL, NULL);
	if (!checkError(errCode, "clEnqueueWriteBuffer(VecMul_0):")){return 0;}

	errCode =
	clEnqueueWriteBuffer(comQ, cl_VecMul_arg1,CL_TRUE, 0,
	(sizeof(float) * NPROC), VecMul_arg1p, 0, NULL, NULL);
	if (!checkError(errCode, "clEnqueueWriteBuffer(VecMul_1):")){return 0;}

	errCode =
	clEnqueueNDRangeKernel(comQ, kernel_VecMul, 1, NULL, (const size_t *) &gws,
		(const size_t *) &lws, 0, NULL, NULL);
	if (!checkError(errCode, "clEnqueueNDRangeKernel(VecMul):")){return 0;}

	errCode =
	clEnqueueReadBuffer(comQ, cl_VecMul_arg2,CL_TRUE, 0,
	(sizeof(float) * NPROC), VecMul_arg2p, 0, NULL, NULL);
	if (!checkError(errCode, "clEnqueueReadBuffer(VecMul_2):")){return 0;}



	struct timespec getCallFinish;
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &getCallFinish);

	double callTime = ((getCallFinish.tv_sec - getCallStart.tv_sec)+
			(getCallFinish.tv_nsec - getCallStart.tv_nsec) / 1000000000.0);

	for (i = 0; i < NPROC; ++i)
	{printf("%2.2f * %2.2f = %2.2f\n", a[i], b[i], out[i]);}

	printf("Call Time: %2.9lf\n", callTime);
	
	if(kernel_VecAdd)clReleaseKernel(kernel_VecAdd);
	if(kernel_VecMul)clReleaseKernel(kernel_VecMul);
	if(clprog)clReleaseProgram(clprog);
	if(comQ)clReleaseCommandQueue(comQ);
	if(cpuContext)clReleaseContext(cpuContext);
	if(cl_VecAdd_arg0)clReleaseMemObject(cl_VecAdd_arg0);
	if(cl_VecAdd_arg1)clReleaseMemObject(cl_VecAdd_arg1);
	if(cl_VecAdd_arg2)clReleaseMemObject(cl_VecAdd_arg2);
	if(cl_VecMul_arg0)clReleaseMemObject(cl_VecMul_arg0);
	if(cl_VecMul_arg1)clReleaseMemObject(cl_VecMul_arg1);
	if(cl_VecMul_arg2)clReleaseMemObject(cl_VecMul_arg2);


	return 0;
}
