#include <iostream>
#include <fstream>
using namespace std;

int main(int argc, char **argv)
{
	if (argc != 2)
	{printf("Usage: %s <filename>\n", argv[0]);}

	FILE *fin;
	fin = fopen(argv[1], "r");	

	int c;

s1:
	c = fgetc(fin);
	if (c == EOF) {goto ex;}
	switch (c)
	{
		case '/': 
			goto s2;
		default: 
			printf("%c",c);
			goto s1;
	}

s2:
	c = fgetc(fin);
	if (c == EOF) {goto ex;}
	switch (c)
	{
		case '*': 
			goto s3;
		case '/': 
			goto s5;
		default: 
			goto s6;
	}

s3:
	c = fgetc(fin);
	if (c == EOF) {goto ex;}
	switch (c)
	{
		case '*': 
			goto s4;
		case '\n': 
			putchar(c);
			goto s3;
		default: 
			goto s3;
	}

s4:
	c = fgetc(fin);
	if (c == EOF) {goto ex;}
	switch (c)
	{
		case '/': 
			goto s1;
		case '\n': 
			putchar(c);
			goto s3;
		default: 
			goto s3;
	}

s5:
	c = fgetc(fin);
	if (c == EOF) {goto ex;}
	switch (c)
	{
		case '\n': 
			putchar(c);
			goto s1; 
		default: 
			goto s5;
	}

s6:
	putchar('/');
	putchar(c);
	goto s1;

ex:
	fclose(fin);
	return 0;
}
