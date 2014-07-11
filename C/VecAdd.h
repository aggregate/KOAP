char kernelstr[] = 
"#define SIMDS 4 \n"
"#define MAXBLOCK 8 \n"
"#define GLOBAL_WORK_SIZE SIMDS * MAXBLOCK\n"
"#define LOCAL_WORK_SIZE MAXBLOCK\n"
"/*\n"
"The above defines are defined for both this host code and for the OpenCL\n"
"code. Defines following this comment are local to this file.\n"
"\n"
" */\n"
"\n"
"__kernel void VecAdd(__global float *a, __global float *b, __global float *out)\n"
"{\n"
"	int iproc = get_global_id(0);\n"
"	out[iproc] = a[iproc] + b[iproc];\n"
"}\n"
" \n"
"__kernel void VecMul(__global float *a, __global float *b, __global float *out){\n"
"	int iproc = get_global_id(0);\n"
"	out[iproc] = a[iproc] * b[iproc];\n"
"}\n"
"\n"
;
