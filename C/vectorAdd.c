/*
Max Local Workgroup Size on various gpus:
    Radeon HD 4870
        GPU_MAXBLOCK=256

    Radeon HD 4650
        GPU_MAXBLOCK=128
*/

#define GPU_SIMDS			4
#define GPU_MAXBLOCK		4

#define LOCAL_WORK_SIZE		GPU_MAXBLOCK
#define GLOBAL_WORK_SIZE	(GPU_SIMDS * GPU_MAXBLOCK)
#define NPROC				GLOBAL_WORK_SIZE


/* User includes */

/* Default includes */

#include <CL/opencl.h>
#include <stdio.h>
#include <sys/time.h>

/* The OpenCL kernel string and associated defines */
#include "vectorAdd.h"

int checkError(cl_int errCode, char *fun)
{
    if (errCode != CL_SUCCESS)
    {
        printf("Error in %s %d\n", fun, errCode);
        return 0;
    }
    return 1;
}

int main(int argc, char** argv)
{
	float a[NPROC];
	void *ap = &a;
	cl_mem cl_a;

 	float b[NPROC];
	void *bp = &b;
	cl_mem cl_b;

 	float out[NPROC];
	void *outp = &out;
	cl_mem cl_out;

	cl_platform_id platid = NULL;
    cl_device_id cldev = NULL;
    cl_context cpuContext;
    cl_command_queue comQ;
    cl_program clprog;
    cl_kernel kernel;
	cl_int errCode = 0;
	cl_uint np;


	char *source = kernelstr;
	char cBuffer[128];
	size_t ksize = sizeof(kernelstr);


	int i;
 	for (i = 0; i < NPROC; ++i)
 	{a[i] = i; b[i] = i; out[i] = 0;}
 	for (i = 0; i < NPROC; ++i)
 	{printf("%2.2f + %2.2f = %2.2f\n", a[i], b[i], out[i]);}

	/* OpenCL Environment Setup */

	printf("Size of kernel: %d\n", (int)ksize);

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


	cl_a =
	clCreateBuffer(cpuContext, CL_MEM_READ_ONLY,
		(sizeof(float) * NPROC), NULL, &errCode);
	if (!checkError(errCode, "clCreateBuffer(0):")){return 0;}

	cl_b =
	clCreateBuffer(cpuContext, CL_MEM_READ_ONLY,
		(sizeof(float) * NPROC), NULL, &errCode);
	if (!checkError(errCode, "clCreateBuffer(1):")){return 0;}

	cl_out =
	clCreateBuffer(cpuContext, CL_MEM_WRITE_ONLY,
		 (sizeof(float) * NPROC), NULL, &errCode);
	if (!checkError(errCode, "clCreateBuffer(2):")){return 0;}


	clprog =
	clCreateProgramWithSource(cpuContext, 1, (const char **)&source,
		&ksize, &errCode);
	if (!checkError(errCode, "clCreateProgramWithSource:")){return 0;}

    	errCode =
	clBuildProgram(clprog, 0, NULL, "-cl-fast-relaxed-math", NULL, NULL);
    if (!checkError(errCode, "clBuildProgram:"))
    {
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
		return 0;
    }

    kernel =
	clCreateKernel(clprog, "VecAdd", &errCode);
    if (!checkError(errCode, "clCreateKernel:")){return 0;}


		errCode =
	clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&cl_a);
	if (!checkError(errCode, "clSetKernelArg(0):")){return 0;}

		errCode =
	clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*)&cl_b);
	if (!checkError(errCode, "clSetKernelArg(1):")){return 0;}

		errCode =
	clSetKernelArg(kernel, 2, sizeof(cl_mem), (void*)&cl_out);
	if (!checkError(errCode, "clSetKernelArg(2):")){return 0;}

	i = 0;
	while (i < 2)
	{
		errCode =
		clEnqueueWriteBuffer(comQ, cl_a, CL_TRUE, 0,
		(sizeof(float) * NPROC), ap, 0, NULL, NULL);
		if (!checkError(errCode, "clEnqueueWriteBuffer(0):")){return 0;}

		errCode =
		clEnqueueWriteBuffer(comQ, cl_b, CL_TRUE, 0,
		(sizeof(float) * NPROC), bp, 0, NULL, NULL);
		if (!checkError(errCode, "clEnqueueWriteBuffer(1):")){return 0;}

/*
	struct timeval _getkernelstart;
    gettimeofday(&_getkernelstart, 0);


	int lws = LOCAL_WORK_SIZE;
	int gws = GLOBAL_WORK_SIZE;
*/
		errCode =
		clEnqueueNDRangeKernel(comQ, kernel, 1, NULL, (const size_t *) &gws,
			(const size_t *) &lws, 0, NULL, NULL);
		if (!checkError(errCode, "clEnqueueNDRangeKernel:")){return 0;}


		errCode =
		clEnqueueReadBuffer(comQ, cl_out, CL_TRUE, 0,
			(sizeof(float) * NPROC), outp, 0, NULL, NULL);
		if (!checkError(errCode, "clEnqueueReadBuffer(2):")){return 0;}

/*
	struct timeval _getkernelstop;
    gettimeofday(&_getkernelstop, 0);

	double kernelRunTime = ((_getkernelstop.tv_sec - _getkernelstart.tv_sec) +
			(_getkernelstop.tv_usec - _getkernelstart.tv_usec) / 1000000.0);
*/	


		for (i = 0; i < NPROC; ++i)
 		{printf("%2.2f + %2.2f = %2.2f\n", a[i], b[i], out[i]);}
		for (i = 0; i < NPROC; ++i)
 		{a[i] = out[i];}
		++i;
	} // end of kernel loop


	if(kernel)clReleaseKernel(kernel);
    if(clprog)clReleaseProgram(clprog);
    if(comQ)clReleaseCommandQueue(comQ);
    if(cpuContext)clReleaseContext(cpuContext);
	if(cl_a)clReleaseMemObject(cl_a);
	if(cl_b)clReleaseMemObject(cl_b);
	if(cl_out)clReleaseMemObject(cl_out);

	return 0;
}

