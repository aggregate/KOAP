#for the C program
koab: koab.o strbuild.o util.o include.o comments.o kernels.o strnode.o defines.o
	gcc -Wall koab.o strbuild.o strnode.o util.o include.o comments.o kernels.o defines.o -o koab
koab.o: koab.c koab.h koabdefs.h strnode.h strbuild.h
	gcc -c -Wall koab.c
strbuild.o: strbuild.h strbuild.c
	gcc -c -Wall strbuild.c
util.o: util.c strbuild.h 
	gcc -c -Wall util.c
include.o: include.c strbuild.h
	gcc -c -Wall include.c
comments.o: comments.c koabdefs.h strbuild.h
	gcc -c -Wall comments.c
kernels.o: kernels.c koab.h strbuild.h strnode.h
	gcc -c -Wall kernels.c
defines.o: defines.c koab.h strbuild.h strnode.h
	gcc -c -Wall defines.c
strnode.o: strnode.c strnode.h
	gcc -c -Wall strnode.c

