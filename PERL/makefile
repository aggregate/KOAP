all: koab koab.pl
# for the perl script
koab.pl: plinclude plcomments plkernels plcallargs

plinclude: include.cpp
	g++ include.cpp -o include

plcomments: comments.cpp
	g++ comments.cpp -o comments

plkernels: kernels.cpp
	g++ kernels.cpp -o kernels

plcallargs: callargs.cpp
	g++ callargs.cpp -o callargs


