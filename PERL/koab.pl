#!/usr/bin/perl
use strict;
use Switch;

if (@ARGV < 1)
{
    print "Usage: koab [koal file name]\n";
    die;
}

my $filename = $ARGV[@ARGV-1];
my $temp_ext = "ktemp";
my $devType = "GPU";
my $doCompile = 0;

if (@ARGV == 2)
{
	if ($ARGV[0] eq "-cpu")
	{$devType = "CPU";}	
	elsif ($ARGV[0] eq "-c")
	{$doCompile = 1;}
}
elsif (@ARGV == 3)
{
	if ($ARGV[0] eq "-cpu")
	{$devType = "CPU";}	
	elsif ($ARGV[0] eq "-c")
	{$doCompile = 1;}

	if ($ARGV[1] eq "-cpu")
	{$devType = "CPU";}	
	elsif ($ARGV[1] eq "-c")
	{$doCompile = 1;}
}

# setting the prefix for the output files to be the same as the
# name of the kernel file, sans the file extension
my @splitname = split(/\./, $filename);
my $prefix;

for (my $i = 0; $i < @splitname - 1; ++$i)
{$prefix = $prefix.$splitname[$i];}
# prefix does not include "." 


# handles comments and includes
`include $filename $temp_ext > $prefix.ic.$temp_ext`;

my @source;
@source = `kernels $prefix.ic.$temp_ext $prefix.cl.$temp_ext`;


my $line;
my @clsource;
my @kernelnames;
my %kernels;
my @alldefs;
my @worksizes;
my @tmp;

open(INFILE, "<$prefix.cl.$temp_ext");
while(defined($line = <INFILE>))
{ push(@clsource, $line);}

for (my $i = 0; $i < @clsource; $i++)
{
	if ($clsource[$i] =~ /^__kernel/)
	{
		@tmp = split(/ /, $clsource[$i]);
		for (my $j = 0; $j < length($tmp[2]); $j++)
		{
			if ("(" eq substr($tmp[2], $j, 1))
			{push(@kernelnames, substr($tmp[2], 0, $j));}
		}
	}
} # end of kernel-search for
close (INFILE);


my $currKernel;
my @kernelsUsed;
my $getargs;
my $argc;

for (my $i = 0; $i < @source; $i++)
{
	if ($source[$i] =~ s/\$define/#define/g)
	{
		# $define SIMDS converted to #define, watch for # not $
		if ($source[$i] =~ /(#define\sSIMDS\s\d+)|(#define\sMAXBLOCK\s\d+)/)
		{
			push(@worksizes, $source[$i]);
			$source[$i] = "";
		}
		else
		{push(@alldefs, $source[$i]);}
	}
	elsif ($source[$i] =~ /^\$kernel/)
	{
		my $found = 0;
		@tmp = split(/\s/, $source[$i]);
		for (my $j = 0; $j < @kernelnames; $j++)
		{
			if ($tmp[1] eq $kernelnames[$j])
			{
				$found = 1;
				print "Using kernel $kernelnames[$j] ...\n";
				$currKernel = $kernelnames[$j];
				push(@kernelsUsed, $currKernel);
			}
		}
		if (!$found)
		{
			print "Kernel ".$tmp[1]." not found in OpenCL code!\n";
			die;
		}

		$getargs = 0;
		$argc = 0;
		$source[$i] = "";
		$i++;
		
		if ($source[$i] =~ /^(\$ro)|(\$rw)|(\$wo)/)	
		{$getargs = 1;}

		while ($getargs)
		{
			$getargs = 0;
			@tmp = split(/\s/, $source[$i]);
			$tmp[0] =~ s/^\$//g;
			# set kernel arg permissions, type, and size			
			$kernels{$currKernel}{"arg_$argc"}{"perm"} = $tmp[0];
			$kernels{$currKernel}{"arg_$argc"}{"type"} = $tmp[1];

			if (@tmp > 2)
			{$kernels{$currKernel}{"arg_$argc"}{"size"} = $tmp[2];}
			else
			{$kernels{$currKernel}{"arg_$argc"}{"size"} = 1;}

			$source[$i] = "";
			$i++;
			if ($source[$i] =~ /^(\$ro)|(\$rw)|(\$wo)/)	
			{$getargs = 1;}
			$argc++;
		}
		$kernels{$currKernel}{"argc"} = $argc;
	}
} # end of source transform loop

my @argdecs;
my $size;
my $type;
my $name;
my $clcleanup =
    "\tif(clprog)clReleaseProgram(clprog);\n".
    "\tif(comQ)clReleaseCommandQueue(comQ);\n".
    "\tif(cpuContext)clReleaseContext(cpuContext);\n";

# generate declarations of necessary cl memory objects and pointers
for (my $i = 0; $i < @kernelnames; ++$i)
{

	$argc = $kernels{$kernelnames[$i]}{"argc"};
	for (my $j = 0; $j < $argc; $j++)
	{
		$type = $kernels{$kernelnames[$i]}{"arg_$j"}{"type"};
		$size = $kernels{$kernelnames[$i]}{"arg_$j"}{"size"};
		$name = "$kernelnames[$i]_arg$j";
		push(@argdecs, "\tvoid *".$name."p;\n");
		push(@argdecs, "\tcl_mem cl_$name;\n\n");
		$clcleanup =
	 	$clcleanup."\tif(cl_$name)clReleaseMemObject(cl_$name);\n";
	}	
}

my $kerneldecs = "";
my $kernelcleanup = "";

for (my $i = 0; $i < @kernelnames; ++$i)
{
	$kerneldecs = $kerneldecs."\tcl_kernel kernel_".$kernelnames[$i].";\n";
	$kernelcleanup = $kernelcleanup."\tif(kernel_".$kernelnames[$i].")".
					 "clReleaseKernel(kernel_".$kernelnames[$i].");\n";
					
}

$clcleanup = $kernelcleanup.$clcleanup;

my $defaultIncs = 
"
#include <CL/opencl.h>
#include <stdio.h>
\n";

my $kernelInc = 
"#include \"$prefix.h\"\n";

my $cFuncs = 
"
int checkError(cl_int errCode, char *fun)
{
	if (errCode != CL_SUCCESS)
	{
		printf(\"Error in %s %d\\n\", fun, errCode);
		return 0;
	}
	return 1;
}\n";

my $clDecs =
"
	cl_platform_id platid = NULL;
	cl_device_id cldev = NULL;
	cl_context cpuContext;
	cl_command_queue comQ;
	cl_program clprog;
	cl_int errCode = 0;
	cl_uint np;
\n";

my $cDecs = 
"	char *source = kernelstr;
	char cBuffer[128];
	size_t ksize = sizeof(kernelstr);
\n";
my $clEnvSetup = 
"	/* OpenCL Environment Setup */

	printf(\"Size of kernel code: %d bytes\\n\", (int)ksize);

		errCode =
	clGetPlatformIDs(0, NULL, &np);
	if (!checkError(errCode, \"clGetPlatformIDs:\")){return 0;}

	printf(\"Number of platforms: %d\\n\", np);

	errCode =
	clGetPlatformIDs(1, &platid, NULL);
	if (!checkError(errCode, \"clGetPlatformIDs:\")){return 0;}

		errCode =
	clGetPlatformInfo(platid, CL_PLATFORM_NAME, sizeof(cBuffer),
		cBuffer, NULL);
	if (!checkError(errCode, \"clGetPlatformInfo:\")){return 0;}

	printf(\"Platform = %s\\n\", cBuffer);

		errCode =
	clGetDeviceIDs(platid, CL_DEVICE_TYPE_$devType, 1, &cldev, NULL);
	if (!checkError(errCode, \"clGetDeviceIDs:\")){return 0;}

	cpuContext =
	clCreateContext(0, 1, &cldev, NULL, NULL, &errCode);
	if (!checkError(errCode, \"clCreateContext:\")){return 0;}

	comQ =
	clCreateCommandQueue(cpuContext, cldev, 0, &errCode);
	if (!checkError(errCode, \"clCreateCommandQueue:\")){return 0;}
\n";

my $clBuildFlags = "";
my $clProgramCompile = 
"	clprog =
	clCreateProgramWithSource(cpuContext, 1, (const char **)&source,
		&ksize, &errCode);
	if (!checkError(errCode, \"clCreateProgramWithSource:\")){return 0;}
	
	int buildError = 
	clBuildProgram(clprog, 0, NULL, \"$clBuildFlags\", NULL, NULL);
	cl_build_status stat;
	char opt[1000];
	char log[4000];

		errCode =
	clGetProgramBuildInfo (clprog, cldev, CL_PROGRAM_BUILD_STATUS,
		sizeof(cl_build_status), &stat, NULL);
	if (!checkError(errCode, \"clGetProgramInfo:\")){return 0;}

		errCode =
	clGetProgramBuildInfo (clprog, cldev, CL_PROGRAM_BUILD_OPTIONS,
		sizeof(opt), opt, NULL);
	if (!checkError(errCode, \"clGetProgramInfo:\")){return 0;}
   
		errCode =
	clGetProgramBuildInfo (clprog, cldev, CL_PROGRAM_BUILD_LOG,
		sizeof(log), log, NULL);
	if (!checkError(errCode, \"clGetProgramBuildInfo:\")){return 0;}

	printf(\"Program Build Info:\\n\");
	printf(\"Build Status\\n%d\\n\", stat);
	printf(\"Build Flags\\n%s\\n\", opt);
	printf(\"Build Log\\n%s\\n\", log);
	if (!checkError(buildError, \"clBuildProgram:\")){return 0;}
\n";

my %createKernel;
my $createBuffers;
my $setArgs;
for (my $i = 0; $i < @kernelnames; $i++)
{
	$argc = $kernels{$kernelnames[$i]}{"argc"};
	for (my $j = 0; $j < $argc; $j++)
	{
		my $size = $kernels{$kernelnames[$i]}{"arg_$j"}{"size"}."\n";
		my $perms = $kernels{$kernelnames[$i]}{"arg_$j"}{"perm"}."\n";
		my $type = $kernels{$kernelnames[$i]}{"arg_$j"}{"type"}."\n";

		chomp($size);
		chomp($perms);
		chomp($type);

		switch($perms)
		{
			case "ro" {$perms = "CL_MEM_READ_ONLY"}
			case "wo" {$perms = "CL_MEM_WRITE_ONLY"}
			case "rw" {$perms = "CL_MEM_READ_ONLY"}
		}
		

		$createBuffers = $createBuffers.
			"\tcl_".$kernelnames[$i]."_arg".$j."=\n".
			"\tclCreateBuffer(cpuContext, $perms,\n".
			"\t\t( sizeof($type) * $size ), NULL, &errCode);\n".
			"\tif (!checkError(errCode, \"clCreateBuffer(".
			$kernelnames[$i].
			"_$j):\")){return 0;}\n\n";
	
	} # end buffer generation for-loop

	$createKernel{$kernelnames[$i]} = 
		"\tkernel_".$kernelnames[$i]." =\n".
		"\tclCreateKernel(clprog, \"".$kernelnames[$i]."\", &errCode);\n".
		"\tif (!checkError(errCode, \"clCreateKernel:\")){return 0;}\n";

	for (my $j = 0; $j < $argc; $j++)
	{
		$setArgs = $setArgs.
		"\t\terrCode =\n".
		"\tclSetKernelArg(kernel_".$kernelnames[$i].", $j, sizeof(cl_mem),".
		" (void*)&cl_".$kernelnames[$i]."_arg".$j.");\n".
		"\tif (!checkError(errCode, \"clSetKernelArg($j):\")){return 0;}\n".
		"\n";
	}
}

my $init = $createBuffers;
 for (my $i = 0; $i < @kernelnames; $i++)
{$init = $init.$createKernel{$kernelnames[$i]}."\n";}
$init = $init.$setArgs;

my %call;
my $launchKernel = "";
my $enqueueWrites = "";
my $enqueueReads = "";
for (my $i = 0; $i < @kernelnames; $i++)
{
	$argc = $kernels{$kernelnames[$i]}{"argc"};
	for (my $j = 0; $j < $argc; $j++)
	{
		my $size = $kernels{$kernelnames[$i]}{"arg_$j"}{"size"}."\n";
		my $perms = $kernels{$kernelnames[$i]}{"arg_$j"}{"perm"}."\n";
		my $type = $kernels{$kernelnames[$i]}{"arg_$j"}{"type"}."\n";

		chomp($size);
		chomp($perms);
		chomp($type);

		if ($perms eq "ro" or $perms eq "rw")
		{
			$enqueueWrites = $enqueueWrites.	
			"\terrCode =\n".
			"\tclEnqueueWriteBuffer(comQ, cl_".$kernelnames[$i]."_arg$j,".
			"CL_TRUE, 0,\n\t(sizeof($type) * $size),".
			" ".$kernelnames[$i]."_arg".$j."p, 0, NULL, NULL);\n".
			"\tif (!checkError(errCode, \"clEnqueueWriteBuffer(".
			$kernelnames[$i]."_".$j."):\")){return 0;}\n\n";
		}	
		if ($perms eq "wo" or $perms eq "rw")
		{
			$enqueueReads = $enqueueReads.	
			"\terrCode =\n".
			"\tclEnqueueReadBuffer(comQ, cl_".$kernelnames[$i]."_arg$j,".
			"CL_TRUE, 0,\n\t(sizeof($type) * $size),".
			" ".$kernelnames[$i]."_arg".$j."p, 0, NULL, NULL);\n".
			"\tif (!checkError(errCode, \"clEnqueueReadBuffer(".
			$kernelnames[$i]."_".$j."):\")){return 0;}\n\n";
		}
	} # End of read and write enqueue loop

	$launchKernel = 
	"\terrCode =\n".
	"\tclEnqueueNDRangeKernel(comQ, kernel_".$kernelnames[$i].", 1, NULL, ".
	"(const size_t *) &gws,\n\t\t(const size_t *) &lws, 0, NULL, NULL);\n".
	"\tif (!checkError(errCode, \"clEnqueueNDRangeKernel(".
	$kernelnames[$i]."):\")){return 0;}\n\n";

	$call{$kernelnames[$i]} = $enqueueWrites.$launchKernel.$enqueueReads;
	$enqueueWrites = "";
	$enqueueReads = "";
	$launchKernel = "";
}

my $once = "$clDecs\n$kerneldecs\n$cDecs\n".
			join("", @argdecs).
			"$clEnvSetup\n$clProgramCompile".
			"\tint lws = LOCAL_WORK_SIZE;\n".
			"\tint gws = GLOBAL_WORK_SIZE;\n";

for (my $i = 0; $i < @source; $i++)
{
	my $id = "(_|[A-Z]|[a-z])(_|[A-Z]|[a-z]|[0-9])*";
	$source[$i] =~ s/\$once/$once/;
#	$source[$i] =~ s/\$init $id/$init/;
	$source[$i] =~ s/\$init/$init/;
	$source[$i] =~ s/\$clcleanup/\n$clcleanup/;
	
	if ($source[$i] =~ /\$call/)
	{
		my @splitcall = split(/ /, $source[$i]);
		shift(@splitcall);	
		my $inp = join("", @splitcall);
		@splitcall = `callargs "$inp"`;
		chomp($splitcall[0]);
		for (my $j = 0; $j < @kernelnames; $j++)
		{
			if ($splitcall[0] eq $kernelnames[$j])
			{
				$argc = $kernels{$kernelnames[$j]}{"argc"};
				for (my $k = 0; $k < $argc; $k++)
				{
					chomp($splitcall[$k+1]);
					$call{$kernelnames[$j]} =
					"\t".$kernelnames[$j]."_arg".$k."p = ".
					$splitcall[$k+1].";\n".
					$call{$kernelnames[$j]};
				} # end k loop
				$source[$i] =~ s/\$call $id\(.*\)/$call{$kernelnames[$j]}/;
			} # end splitcall if
		} # end j loop
		
	} # end if source[i] matches $call
} # end source replacement loop

# handle special defines
my @specialdefs;
push(@specialdefs, "#define GLOBAL_WORK_SIZE SIMDS * MAXBLOCK\n");
push(@specialdefs, "#define LOCAL_WORK_SIZE MAXBLOCK\n");

open(CFILE, ">$prefix.c");
open(HFILE, ">$prefix.h");

#### DO OUTPUT ####
print CFILE $defaultIncs;
print CFILE $kernelInc;

print CFILE @worksizes;
print CFILE @specialdefs;
# no need to print alldefs to C file, $ replaced by # in source
print HFILE @worksizes;
print HFILE @specialdefs;
print HFILE @alldefs;

print CFILE
"/*\nThe above defines are defined for both this host code and for the OpenCL\ncode. Defines following this comment are local to this file.\n\n */";

print HFILE
"/*\nThe above defines are defined for both this host code and for the OpenCL\ncode. Defines following this comment are local to this file.\n\n */";

print CFILE $cFuncs;

print CFILE @source;
print HFILE @clsource;

close CFILE;
close HFILE;

open(HFILE, "<$prefix.h");
@clsource = (); # empty array
while (defined($line = <HFILE>))
{push(@clsource, $line);}
close HFILE;

# do the necessary escapes
for (my $i = 0; $i < @clsource; ++$i)
{
    $clsource[$i] =~ s/\\/\\\\/g;
    $clsource[$i] =~ s/\"/\\\"/g;
    $clsource[$i] =~ s/\'/\\\'/g;
    $clsource[$i] =~ s/^/\"/;
    $clsource[$i] =~ s/\n/\\n\"\n/;
}


open(HFILE, ">$prefix.h");
print HFILE "char kernelstr[] = \n";
print HFILE @clsource;
print HFILE ";\n";
close HFILE;
`rm *.ktemp`;
if ($doCompile)
{`gcc $prefix.c -lOpenCL -o $prefix`;}
