#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "koabdefs.h"
#include "koab.h"

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		printf("Usage: %s <options> [koab file name]\n", argv[0]);
		printf("Options:\n\t-cpu\tuse DEVICE_TYPE_CPU\n");
		exit(0);
	}

	char *filename = argv[argc-1];

	///////
	// process options; write this code later
	//////

	char * f_prefix = get_prefix(filename);

	printf("file prefix = %s\n", f_prefix);

	char *source = handle_includes(filename, f_prefix);

	printf("Handled $includes\n");
	
	str_builder clsource;
	str_builder csource;
	strbuild_init(&clsource, 5000);
	strbuild_init(&csource, 5000);

	strip_cl(source, &clsource, &csource); 

	strnode kernelNames;
	strnode_init(&kernelNames);
	get_kernelnames(&clsource, &kernelNames);

	// print kernel names 
	strnode_print(&kernelNames, stdout);

	// handle $defines
	strnode worksizes;
	strnode alldefs;
	strnode_init(&worksizes);
	strnode_init(&alldefs);
	handle_defines(csource.s, &worksizes, &alldefs);

	strnode_print(&worksizes, stdout);
	strnode_print(&alldefs, stdout);
	// cleanup
	strbuild_free(&clsource);
	strbuild_free(&csource);
	free(source);
	free(f_prefix);
	printf("done.\n");

	return 0;
}

