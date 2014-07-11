// koap.cpp
#include <cstdlib> // system
#include <stdio.h> // printf, fprintf
#include <string>
#include <iostream>
#include <fstream>
#include <string.h> // strcmp
using namespace std;

#include "koap.h"

#define STRIS(str, arg)	(0 == strcmp(str, arg))

void printHelp(char *name)
{
		printf("KOAP: A preprocessor for generating OpenCL host"
				" applications.\n");
		printf("\n");
		printf("Usage: %s <options> [koap file names ...]\n", name);
		printf("\n");
		printf("Options:\n");
		printf("\t-d, --device <device>\n");
		printf("\t\tdevice options:\n");
		printf("\t\tCPU -> CL_DEVICE_TYPE_CPU\n");
		printf("\t\tGPU -> CL_DEVICE_TYPE_GPU\n");
		printf("\t\tACC -> CL_DEVICE_TYPE_ACCELERATOR\n");
		printf("\t\tkoap defaults to CL_DEVICE_TYPE_DEFAULT\n");
		printf("\n");
		printf("\t-b, --build \"cmd\"\n");
		printf("\t\trun cmd after generating output files\n");
		printf("\n");
		printf("\t-u, --constFile <file>\n");
		printf("\t\tupdate internal constants used for code generation"
				" from file\n");
		printf("\n");
		printf("\t-f, --clcflags \"flags\"\n");
		printf("\t\tUse flags for OpenCL program compilation\n");
		printf("\n");
}

int main(int argc, char **argv)
{
	// make sure what the user is trying to do is sane
	/* 
		No odd-numbered combination of parameters makes sense,
		unless you REALLY want to generate a map of key-value
		pairs using your .koal file.
	*/
	if (argc < 2)
	{
		printHelp(argv[0]);
		exit(1);
	}

	// set up state related to arguments
	bool compile;
	string devType = "CL_DEVICE_TYPE_DEFAULT";
	string buildCmd = "";
	string clcflags = "\"\"";
	bool build = false;
	bool updateConsts = false;
	string constFile = "";
	ifstream fin;

	vector <string> koapFiles;

	// process arguments

#define NEWPARSE
#ifdef NEWPARSE
	for (int i = 1; i < argc; i++)
	{
		if (STRIS("-d", argv[i]) || 
			STRIS("--device", argv[i]))
		{
			i += 1;
			if (i < argc)
			{
				if (STRIS("CPU", argv[i]))
				{devType = "CL_DEVICE_TYPE_CPU";}
				else if (STRIS("GPU", argv[i]))
				{devType = "CL_DEVICE_TYPE_GPU";}
				else if (STRIS("ACC", argv[i]))
				{devType = "CL_DEVICE_TYPE_ACCELERATOR";}
				else
				{
					fprintf(stderr, "Invalid device: %s.\n", argv[i]);
					fprintf(stderr, "Using CL_DEVICE_TYPE_DEFAULT.\n");
				}
			} 
			else
			{
				fprintf(stderr, "Bad command line structure. ");
				fprintf(stderr, "Out of arguments.\n");
				printHelp(argv[0]);
			}
		} // if STRIS("-d", argv[i])
		else if (STRIS("-f", argv[i]) ||
				STRIS("--clcflags", argv[i]))
		{
			i += 1;
			if (i < argc)
			{
				clcflags = "\""+string(argv[i])+"\"";
			}
			else
			{
				fprintf(stderr, "Bad command line structure. ");
				fprintf(stderr, "Out of arguments.\n");
				printHelp(argv[0]);
			}
		}
		else if (STRIS("-b", argv[i]) ||
				STRIS("--build", argv[i]))
		{
			i += 1;
			if (i < argc)
			{
				build = true;
				buildCmd = argv[i];	
			}
			else
			{
				fprintf(stderr, "Bad command line structure. ");
				fprintf(stderr, "Out of arguments.\n");
				printHelp(argv[0]);
			}
		}
		else if (STRIS("-u", argv[i]) ||
				STRIS("--constfile", argv[i])) 
		{
			i += 1;
			if (i < argc)
			{
				updateConsts = true;
				constFile = argv[i];	
			}
			else
			{
				fprintf(stderr, "Bad command line structure. ");
				fprintf(stderr, "Out of arguments.\n");
				printHelp(argv[0]);
			}
		}
		else
		{
			koapFiles.push_back(argv[i]);
		}
	}
#else
	for (int i = 1; i < argc-1; i++)
	{
		if (0 == strcmp("-dev", argv[i]))
		{
			i += 1;
			if (0 == strcmp("CPU", argv[i]))
			{devType = "CL_DEVICE_TYPE_CPU";}
			else if (0 == strcmp("GPU", argv[i]))
			{devType = "CL_DEVICE_TYPE_GPU";}
			else if	(0 == strcmp("ACC", argv[i]))
			{devType = "CL_DEVICE_TYPE_ACCELERATOR";}
			else
			{
				fprintf(stderr, "Invalid device: %s.\n", argv[i]);
				fprintf(stderr, "Using CL_DEVICE_TYPE_DEFAULT.\n");
			}
		}
		else if (0 == strcmp("-clcflags", argv[i]))
		{
			i += 1;
			clcflags = "\""+string(argv[i])+"\"";
		}
		else if (0 == strcmp("-build", argv[i]))
		{
			build = true;
			i += 1;
			buildCmd = argv[i];
		}
		else if (0 == strcmp("-constFile", argv[i]))
		{
			updateConsts = true;
			i += 1;
			constFile = argv[i];	
		}
	}
	koapFiles.push_back(argv[argc-1]);
#endif

	fin.open(constFile.c_str());
	initCONSTS(updateConsts, fin);
	fin.close();

	for (int i = 0; i < koapFiles.size(); i++)
	{

		string filename = koapFiles[i];
		string prefix = get_f_prefix(filename);

		// open file, strip comments, process $includes
		string source = process_includes(filename).c_str();
		string clsource_str, csource_str;

		// collect everything appearing in ${ $} sections
		strip_cl(source, clsource_str, csource_str);	

		// detect kernels in the OpenCL code
		vector <string> kernelnames;
		get_kernelnames(clsource_str, kernelnames);

		/* 
			Now, turn these large, unruly strings into data structures that we
			can easily walk around in.
		*/
		vector <string> clsource;
		clsource = vectorFromStr(clsource_str);	
		vector <string> csource;
		csource = vectorFromStr(csource_str);

	#define DEBUG
	#ifdef DEBUG
		printf("clsource is %d lines\n", (int)clsource.size());
		printf("csource is %d lines\n", (int)csource.size());
		printf("Kernels detected:\n");
		printStrDeque(cout, kernelnames, "\n");
	#endif

		// create global environment
		vector <string> globalCL;
		globalCL = genGlobal(prefix, kernelnames);

		vector<string>::iterator line; // a temporary container
		directive_t dir; // used to store the result of parsing $ directives
		vector <string> output; // the .c file
		vector <string> defines; // $defines

		vector <string> temp; // another temporary container
		// the main processing loop
		// see if this line contains a directive
		// if so, add the requisite replacement to the output
		// if not, add this line to the output
		for (line = csource.begin(); line < csource.end(); line++)
		{
			dir = match(*line);
			switch (dir.type)
			{
				case (GLOBAL):
					output.insert(output.end(),globalCL.begin(),globalCL.end());
					break;
				case (INIT):
					temp = genInit(devType, kernelnames, clcflags);
					output.insert(output.end(), temp.begin(), temp.end());
					break;
				case (CLALLOC):
					temp = genClalloc(dir.args);
					output.insert(output.end(), temp.begin(), temp.end());
					break;
				case (CLWRITE):
					for (int xs = 0; xs < dir.args.size(); xs++)
					{
						temp = genClWrite(dir.args[xs]);
						output.insert(output.end(), temp.begin(), temp.end());
					}
					break;
				case (CALL):
					temp = genClCall(dir.args);
					output.insert(output.end(), temp.begin(), temp.end());
					break;
				case (CLREAD):
					for (int xs = 0; xs < dir.args.size(); xs++)
					{
						temp = genClRead(dir.args[xs]);
						output.insert(output.end(), temp.begin(), temp.end());
					}
					break;
				case (CLFREE):
					for (int xs = 0; xs < dir.args.size(); xs++)
					{
						temp = genClFree(dir.args[xs]);
						output.insert(output.end(), temp.begin(), temp.end());
					}
					break;
				case (CLCLEANUP):
					temp = genClCleanup(kernelnames);
					output.insert(output.end(), temp.begin(), temp.end());
					break;
				case (DEFINE):
					{ // introduce a new scope for initialization of this string
					string df = "#define "+dir.args[0]+" "+dir.args[1]+"\n";
					output.push_back(df);
					defines.push_back(df);
					} // close extra scope
					break;
				default: // line did not contain a directive
					output.push_back(*line);
					break;
			} // end switch
		} // end for

		// will contain the escaped OpenCL code
		vector <string> escapedCL = genKstrDecl();	

		// escape defines
		for (int i = 0; i < defines.size(); ++i)
		{
			string t = "\"";
			t.append(replace(defines[i], "\n", "\\n\"\n"));
			escapedCL.push_back(t);
		}
		// escape OpenCL source
		for (int i = 0; i < clsource.size(); ++i)
		{
			string t = "\"";
			t.append(replace(clsource[i], "\n", "\\n\"\n"));
			escapedCL.push_back(t);
		}
		escapedCL.push_back(";\n");

		// do output
		ofstream fout;
		fout.open((prefix+".c").c_str());
		printStrDeque(fout, output, "");	
		fout.close();

		fout.open((prefix+".h").c_str());
		printStrDeque(fout, escapedCL, "");
		fout.close();
	} // for file in koapFiles

	if (build)
	{system(buildCmd.c_str());}

	return 0;
}
 
