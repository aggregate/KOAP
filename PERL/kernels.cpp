#include <iostream>
#include <fstream>
using namespace std;

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		printf("Usage: %s <filename> <output filename>\n", argv[0]);
		return 1;
	}

	FILE *fin;
	FILE *fout;
	fin = fopen(argv[1], "r");	
	fout = fopen(argv[2], "w");	

	int c;

s1:
	c = fgetc(fin);
	if (c == EOF) {goto ex;}
	switch (c)
	{
		case '$':
			goto s2;
		default:
			goto s7;
	}
s2:
	c = fgetc(fin);
	if (c == EOF) {goto ex;}
	switch (c)
	{
		case '{':
			goto s3;
		default:
			goto s6;
	}

s3:
	c = fgetc(fin);
	if (c == EOF) {goto ex;}
	switch (c)
	{
		case '$':
			goto s4;
		default:
			goto s5;
	}

s4:
	c = fgetc(fin);
	if (c == EOF) {goto ex;}
	switch (c)
	{
		case '}':
			goto s1;
		default:
			goto s5;
	}

s5:
	fprintf(fout, "%c", c);
	goto s3;

s6:
	putchar('$');
	goto s7;

s7:
	putchar(c);
	goto s1;

ex:
	fclose(fin);
	fclose(fout);
	return 0;
}
