// comments.cpp
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

// a state machine to detect and strip comments from the source
string rm_comments(string filename)
{
	FILE* fin;
	fin = fopen(filename.c_str(), "r");
	stringstream rt;	
	int c;

s1:
	c = fgetc(fin);
	if (c == EOF) {goto ex;}
	switch (c)
	{
		case '/': 
			goto s2;
		default: 
			rt <<  (char)c;
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
			rt <<  (char)c;
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
			rt <<  (char)c;
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
			rt <<  (char)c;
			goto s1; 
		default: 
			goto s5;
	}

s6:
	rt << '/';
	rt <<  (char)c;
	goto s1;

ex:
	fclose(fin);
	return rt.str();
}
