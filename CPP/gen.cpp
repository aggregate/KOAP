// gen.cpp
#include <string>
#include <vector>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <map>
#include "gen.h"
using namespace std;

// will hold names of identifiers to be used in the generated code
map <string,string> CONSTS;

/* 
	Initialize constants to default identifiers, then read any 
	redefinitions from a file.
*/
void initCONSTS(bool updateConsts, ifstream &fin)
{
	CONSTS["KOAP_PLATID"] = KOAP_PLATID;
	CONSTS["KOAP_CLDEV"] = KOAP_CLDEV;
	CONSTS["KOAP_CONTEXT"] = KOAP_CONTEXT;
	CONSTS["KOAP_CMDQ"] = KOAP_CMDQ;
	CONSTS["KOAP_PROG"] = KOAP_PROG;
	CONSTS["KOAP_ERR"] = KOAP_ERR;
	CONSTS["KOAP_NUM_PLATS"] = KOAP_NUM_PLATS;
	CONSTS["KOAP_CLSRC"] = KOAP_CLSRC;
	CONSTS["KOAP_KSTR"] = KOAP_KSTR;
	CONSTS["KOAP_KSZ"] = KOAP_KSZ;
	CONSTS["KOAP_PLATNAME"] = KOAP_PLATNAME;
	CONSTS["KOAP_GLBSZ"] = KOAP_GLBSZ;
	CONSTS["KOAP_LCLSZ"] = KOAP_LCLSZ;

	if (updateConsts)
	{
		while (fin)
		{
			string key;
			string val;
			fin >> key;
			fin >> val;
			CONSTS[key] = val;
		}	
	}
}

// generate the kernel string declaration
vector <string> genKstrDecl()
{
	vector <string> KstrDecl;
	KstrDecl.push_back("char "+CONSTS["KOAP_KSTR"]+"[] = \n");
	return KstrDecl;
}

// generate everything needed to declare a global environment
vector <string> genGlobal(string prefix, vector <string> knames)
{
	vector <string> globalCL;

	globalCL.push_back("#include <stdio.h>\n");
	globalCL.push_back("#include <CL/opencl.h>\n");
	globalCL.push_back("#include \""+prefix+".h\"\n");
	globalCL.push_back("// Global OpenCL environment vars\n");
	globalCL.push_back("cl_platform_id "+CONSTS["KOAP_PLATID"]+";\n");
	globalCL.push_back("cl_device_id "+CONSTS["KOAP_CLDEV"]+";\n");
	globalCL.push_back("cl_context "+CONSTS["KOAP_CONTEXT"]+";\n");
	globalCL.push_back("cl_command_queue "+CONSTS["KOAP_CMDQ"]+";\n");
	globalCL.push_back("cl_program "+CONSTS["KOAP_PROG"]+";\n");
	globalCL.push_back("cl_int "+CONSTS["KOAP_ERR"]+";\n");
	globalCL.push_back("cl_uint "+CONSTS["KOAP_NUM_PLATS"]+";\n");
	globalCL.push_back("char "+CONSTS["KOAP_PLATNAME"]+"[256];\n");

	// generate a declaration for each kernel
	for (int i = 0; i < knames.size(); i += 1)
	{globalCL.push_back("cl_kernel kernel_"+knames[i]+";\n");}

	globalCL.push_back("char *"+CONSTS["KOAP_CLSRC"]+" = "
			+CONSTS["KOAP_KSTR"]+";\n");
	globalCL.push_back("size_t "+CONSTS["KOAP_KSZ"]+" = sizeof("
			+CONSTS["KOAP_KSTR"]+");\n");
	globalCL.push_back("size_t "+CONSTS["KOAP_GLBSZ"]+";\n");
	globalCL.push_back("size_t "+CONSTS["KOAP_LCLSZ"]+";\n");


	globalCL.push_back("int checkError(cl_int "+CONSTS["KOAP_ERR"]
			+", char *fun)\n");
	globalCL.push_back("{\n");
	globalCL.push_back("\tif ("+CONSTS["KOAP_ERR"]+" != CL_SUCCESS)\n");
	globalCL.push_back("\t{\n");
	globalCL.push_back("\t\tprintf(\"Error in %s %d\\n\", fun, "
			+CONSTS["KOAP_ERR"]+");\n");
	globalCL.push_back("\t\treturn 0;\n");
	globalCL.push_back("\t}\n");
	globalCL.push_back("\treturn 1;\n");
	globalCL.push_back("}\n");
	globalCL.push_back("\n");

	return globalCL;
}

// generate the clean up code
vector <string> genClCleanup(vector <string> knames)
{
	vector <string> rt;
	// free each kernel
	for (int i = 0; i < knames.size(); ++i)
	{
		rt.push_back("\tif(kernel_"+knames[i]+") "
			+"clReleaseKernel(kernel_"+knames[i]+");\n");
	}
	rt.push_back("\tif("+CONSTS["KOAP_PROG"]+") clReleaseProgram("
			+CONSTS["KOAP_PROG"]+");\n");
	rt.push_back("\tif("+CONSTS["KOAP_CMDQ"]+") clReleaseCommandQueue("
			+CONSTS["KOAP_CMDQ"]+");\n");
	rt.push_back("\tif("+CONSTS["KOAP_CONTEXT"]+") clReleaseContext("
			+CONSTS["KOAP_CONTEXT"]+");\n");
	return rt;
}

// generate the code to release an OpenCL memory buffer
vector <string> genClFree(string buf)
{
	vector <string> rt;
	rt.push_back("\tif(cl_"+buf+") clReleaseMemObject(cl_"+buf+");\n");
	return rt;
}

/* 
	Generate the code to read the contents of a memory buffer back
	to the host.
*/
vector <string> genClRead(string buf)
{
	string bufName = buf;
	if (buf[0] == '*' || buf[0] == '&')
	{bufName = buf.substr(1, buf.length());}
	vector <string> rt;
	rt.push_back("\t"+CONSTS["KOAP_ERR"]+" =\n");
	rt.push_back("\tclEnqueueReadBuffer("+CONSTS["KOAP_CMDQ"]+", cl_"
			+bufName+", CL_TRUE, 0,\n");
	rt.push_back("\t\tcl_"+bufName+"_sz,"+buf+", 0, NULL, NULL);\n");
	rt.push_back("\tif(!checkError("+CONSTS["KOAP_ERR"]+",\"$clread(..."
			+buf+"...):\")){exit(0);}\n");
	rt.push_back("\n");
	return rt;
}

// Generate code to set kernel arguments and launch a kernel
vector <string> genClCall(vector <string> args)
{
	vector <string> rt;
	for (int i = 1; i < args.size() - 2; ++i)
	{
		// convert arg number to a string
		char str[4];
		snprintf(str, sizeof(str), "%d", i-1);
		rt.push_back("\t\t"+CONSTS["KOAP_ERR"]+" =\n");
		rt.push_back("\tclSetKernelArg(kernel_"
			+args[0]+","+str+", sizeof(cl_mem),(void*)&cl_"+args[i]+");\n");
		rt.push_back("\tif(!checkError("+CONSTS["KOAP_ERR"]
			+",\"clSetKernelArg("+args[0]+","+args[i]+"):\")){exit(0);}\n");
		rt.push_back("\n");
	}
	rt.push_back("\t"+CONSTS["KOAP_GLBSZ"]+" = "+args[args.size()-2]+";\n");
	rt.push_back("\t"+CONSTS["KOAP_LCLSZ"]+" = "+args[args.size()-1]+";\n");
	rt.push_back("\t\t"+CONSTS["KOAP_ERR"]+" =\n");
	rt.push_back("\tclEnqueueNDRangeKernel("+CONSTS["KOAP_CMDQ"]+", kernel_"
		+args[0]+", 1, NULL,\n");
	rt.push_back("\t\t&"+CONSTS["KOAP_GLBSZ"]+", &"
		+CONSTS["KOAP_LCLSZ"]+","+string("0, NULL, NULL);\n"));
	rt.push_back("\tif(!checkError("+CONSTS["KOAP_ERR"]+", \"$call("
		+args[0]+"):\")){exit(0);}\n");
	rt.push_back("\n");

	return rt;
}

// Generate code to write a host buffer to an OpenCL buffer
vector <string> genClWrite(string buf)
{
	string bufName = buf;
	if (buf[0] == '*' || buf[0] == '&')
	{bufName = buf.substr(1, buf.length());}
	vector <string> rt;
	rt.push_back("\t"+CONSTS["KOAP_ERR"]+" =\n");
	rt.push_back("\tclEnqueueWriteBuffer("+CONSTS["KOAP_CMDQ"]
		+", cl_"+bufName+", CL_TRUE, 0,\n");
	rt.push_back("\t\tcl_"+bufName+"_sz,"+buf+", 0, NULL, NULL);\n");
	rt.push_back("\tif(!checkError("+CONSTS["KOAP_ERR"]+",\"$clwrite(..."
		+buf+"...):\")){exit(0);}\n");
	rt.push_back("\n");
	return rt;
}

// an internal utility function to parse the permission spec
string getPerms(string perms)
{
	if (perms == "ro")
	{return "CL_MEM_READ_ONLY";}
	else if (perms == "wo")
	{return "CL_MEM_WRITE_ONLY";}
	// assume the programmer wants a read/write buffer
	else
	{return "CL_MEM_READ_WRITE";}
}

// generate code to allocate an OpenCL memory buffer
vector <string> genClalloc(vector <string> args)
{
	vector <string> rt;
	rt.push_back("\tcl_mem cl_"+args[0]+";\n");
	rt.push_back("\tint cl_"+args[0]+"_sz = (sizeof("
		+args[1]+")*"+args[2]+");\n");
	rt.push_back("\tcl_"+args[0]+"=");
	rt.push_back("clCreateBuffer("+CONSTS["KOAP_CONTEXT"]+","
		+getPerms(args[3])+",\n");
	rt.push_back("\t\tcl_"+args[0]+"_sz,NULL,&"+CONSTS["KOAP_ERR"]+");\n");
	rt.push_back("\tif(!checkError("+CONSTS["KOAP_ERR"]+", \"$clalloc("
		+args[0]+","+args[1]+","+args[2]+","+args[3]+"):+\")){exit(0);}\n");
	rt.push_back("\n");
	return rt;
}

// generate code to initialize the OpenCL environment
vector <string> genInit(string devType,vector <string> knames,string clcflags)
{
	vector <string> CLinit;
	CLinit.push_back(" /* OpenCL Environment Setup */\n");
	CLinit.push_back("\n");
	CLinit.push_back("\tprintf(\"Size of kernel code: %d bytes\\n\", (int)"
		+CONSTS["KOAP_KSZ"]+");\n");
	CLinit.push_back("\n");
	CLinit.push_back("\t\t"+CONSTS["KOAP_ERR"]+" =\n");
	CLinit.push_back("\tclGetPlatformIDs(0, NULL, &"
		+CONSTS["KOAP_NUM_PLATS"]+");\n");
	CLinit.push_back("\tif (!checkError("+CONSTS["KOAP_ERR"]
		+", \"clGetPlatformIDs:\")){exit(0);} \n");
	CLinit.push_back("\n");
	CLinit.push_back("\tprintf(\"Number of platforms: %d\\n\", "
		+CONSTS["KOAP_NUM_PLATS"]+");\n");
	CLinit.push_back("\n");
	CLinit.push_back("\t"+CONSTS["KOAP_ERR"]+" =\n");
	CLinit.push_back("\tclGetPlatformIDs(1, &"+CONSTS["KOAP_PLATID"]
		+", NULL);\n");
	CLinit.push_back("\tif (!checkError("+CONSTS["KOAP_ERR"]
		+", \"clGetPlatformIDs:\")){exit(0);} \n");
	CLinit.push_back("\n");
	CLinit.push_back("\t\t"+CONSTS["KOAP_ERR"]+" =\n");
	CLinit.push_back("\tclGetPlatformInfo("+CONSTS["KOAP_PLATID"]
		+", CL_PLATFORM_NAME, sizeof("+CONSTS["KOAP_PLATNAME"]+"),\n");
	CLinit.push_back("\t\t"+CONSTS["KOAP_PLATNAME"]+", NULL);\n");
	CLinit.push_back("\tif (!checkError("+CONSTS["KOAP_ERR"]
		+", \"clGetPlatformInfo:\")){exit(0);} \n");
	CLinit.push_back("\n");
	CLinit.push_back("\tprintf(\"Platform = %s\\n\", "
		+CONSTS["KOAP_PLATNAME"]+");\n");
	CLinit.push_back("\n");
	CLinit.push_back("\t\t"+CONSTS["KOAP_ERR"]+" =\n");
	CLinit.push_back("\tclGetDeviceIDs("+CONSTS["KOAP_PLATID"]+", "
		+devType+", 1, &"+CONSTS["KOAP_CLDEV"]+", NULL);\n");
	CLinit.push_back("\tif (!checkError("+CONSTS["KOAP_ERR"]
		+", \"clGetDeviceIDs:\")){exit(0);} \n");
	CLinit.push_back("\n");
	CLinit.push_back("\t"+CONSTS["KOAP_CONTEXT"]+" =\n");
	CLinit.push_back("\tclCreateContext(0, 1, &"+CONSTS["KOAP_CLDEV"]
		+", NULL, NULL, &"+CONSTS["KOAP_ERR"]+");\n");
	CLinit.push_back("\tif (!checkError("+CONSTS["KOAP_ERR"]
		+", \"clCreateContext:\")){exit(0);} \n");
	CLinit.push_back("\n");
	CLinit.push_back("\t"+CONSTS["KOAP_CMDQ"]+" =\n");
	CLinit.push_back("\tclCreateCommandQueue("+CONSTS["KOAP_CONTEXT"]+", "
		+CONSTS["KOAP_CLDEV"]+", 0, &"+CONSTS["KOAP_ERR"]+");\n");
	CLinit.push_back("\tif (!checkError("+CONSTS["KOAP_ERR"]
		+", \"clCreateCommandQueue:\")){exit(0);} \n");
	CLinit.push_back("\n");
	CLinit.push_back("\n");
	CLinit.push_back("\t"+CONSTS["KOAP_PROG"]+" =\n");
	CLinit.push_back("\tclCreateProgramWithSource("+CONSTS["KOAP_CONTEXT"]
		+", 1, (const char **)&"+CONSTS["KOAP_CLSRC"]+",\n");
	CLinit.push_back("\t\t&"+CONSTS["KOAP_KSZ"]+", &"
		+CONSTS["KOAP_ERR"]+");\n");
	CLinit.push_back("\tif (!checkError("+CONSTS["KOAP_ERR"]
		+", \"clCreateProgramWithSource:\")){exit(0);} \n");
	CLinit.push_back("\t\n");
	CLinit.push_back("\tint buildError = \n");
	CLinit.push_back("\tclBuildProgram("+CONSTS["KOAP_PROG"]+", 0, NULL, "
		+clcflags+" , NULL, NULL);\n");
	CLinit.push_back("\tcl_build_status stat;\n");
	CLinit.push_back("\tchar opt[1000];\n");
	CLinit.push_back("\tchar log[4000];\n");
	CLinit.push_back("\n");
	CLinit.push_back("\t\t"+CONSTS["KOAP_ERR"]+" =\n");
	CLinit.push_back("\tclGetProgramBuildInfo ("+CONSTS["KOAP_PROG"]+", "
		+CONSTS["KOAP_CLDEV"]+", CL_PROGRAM_BUILD_STATUS,\n");
	CLinit.push_back("\t\tsizeof(cl_build_status), &stat, NULL);\n");
	CLinit.push_back("\tif (!checkError("+CONSTS["KOAP_ERR"]
		+", \"clGetProgramInfo:\")){exit(0);} \n");
	CLinit.push_back("\n");
	CLinit.push_back("\t\t"+CONSTS["KOAP_ERR"]+" =\n");
	CLinit.push_back("\tclGetProgramBuildInfo ("+CONSTS["KOAP_PROG"]+", "
		+CONSTS["KOAP_CLDEV"]+", CL_PROGRAM_BUILD_OPTIONS,\n");
	CLinit.push_back("\t\tsizeof(opt), opt, NULL);\n");
	CLinit.push_back("\tif (!checkError("+CONSTS["KOAP_ERR"]
		+", \"clGetProgramInfo:\")){exit(0);} \n");
	CLinit.push_back("   \n");
	CLinit.push_back("\t\t"+CONSTS["KOAP_ERR"]+" =\n");
	CLinit.push_back("\tclGetProgramBuildInfo ("+CONSTS["KOAP_PROG"]+", "
		+CONSTS["KOAP_CLDEV"]+", CL_PROGRAM_BUILD_LOG,\n");
	CLinit.push_back("\t\tsizeof(log), log, NULL);\n");
	CLinit.push_back("\tif (!checkError("+CONSTS["KOAP_ERR"]
		+", \"clGetProgramBuildInfo:\")){exit(0);} \n");
	CLinit.push_back("\n");
	CLinit.push_back("\tprintf(\"Program Build Info:\\n\");\n");
	CLinit.push_back("\tprintf(\"Build Status\\n%d\\n\", stat);\n");
	CLinit.push_back("\tprintf(\"Build Flags\\n%s\\n\", opt);\n");
	CLinit.push_back("\tprintf(\"Build Log\\n%s\\n\", log);\n");
	CLinit.push_back("\tif (!checkError(buildError, \"clBuildProgram:\"))"
		"{exit(0);} \n");
	CLinit.push_back("\n");

	for (int i = 0; i < knames.size(); ++i)
	{
		CLinit.push_back("\tkernel_"+knames[i]+" =\n");
		CLinit.push_back("\tclCreateKernel("+CONSTS["KOAP_PROG"]+", \""
			+knames[i]+"\", &"+CONSTS["KOAP_ERR"]+");\n");
		CLinit.push_back("\tif(!checkError("+CONSTS["KOAP_ERR"]
			+", \"clCreateKernel("+knames[i]+"):\")){exit(0);}\n");
		CLinit.push_back("\n");
	}

	CLinit.push_back("\n");

	return CLinit;
}
