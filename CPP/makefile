objdir = obj

koap: $(objdir)/koap.o $(objdir)/util.o $(objdir)/include.o $(objdir)/comments.o $(objdir)/kernels.o $(objdir)/gen.o
	g++ $(objdir)/koap.o $(objdir)/util.o $(objdir)/include.o $(objdir)/comments.o $(objdir)/kernels.o $(objdir)/gen.o -o koap
$(objdir)/koap.o: koap.cpp koap.h
	g++ -c koap.cpp -o $(objdir)/koap.o
$(objdir)/util.o: util.cpp koap.h
	g++ -c util.cpp -o $(objdir)/util.o
$(objdir)/include.o: include.cpp koap.h
	g++ -c include.cpp -o $(objdir)/include.o
$(objdir)/comments.o: comments.cpp
	g++ -c comments.cpp -o $(objdir)/comments.o
$(objdir)/kernels.o: kernels.cpp koap.h
	g++ -c kernels.cpp -o $(objdir)/kernels.o
$(objdir)/gen.o: gen.cpp gen.h
	g++ -c gen.cpp -o $(objdir)/gen.o

clean:
	rm $(objdir)/*.o koap

export:
	svn export svn+sshdisk://super.ece.engr.uky.edu/raid/svn/KOAP/CPP/
	mv CPP KOAP-`date +%Y%m%d`
	tar -czvf KOAP-`date +%Y%m%d`.tgz KOAP-`date +%Y%m%d`
	rm -r KOAP-`date +%Y%m%d`
