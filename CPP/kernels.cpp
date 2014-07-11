// kernels.cpp
#include <sstream>
#include <stdio.h>
#include <cstdlib>
using namespace std;

#include "koap.h"

/* 
	Another state machine; this one separates the content between ${ and $}
	from the normal code.
*/
void strip_cl(string source, string &clsource, string &csource)
{
	int c;
	int pos = 0;

s1:
	c = source[pos++];
	if (c == '\0') {goto ex;}
	switch (c)
	{
		case '$':
			goto s2;
		default:
			goto s7;
	}
s2:
	c = source[pos++];
	if (c == '\0') {goto ex;}
	switch (c)
	{
		case '{':
			goto s3;
		default:
			goto s6;
	}

s3:
	c = source[pos++];
	if (c == '\0') {goto ex;}
	switch (c)
	{
		case '$':
			goto s4;
		default:
			goto s5;
	}

s4:
	c = source[pos++];
	if (c == '\0') {goto ex;}
	switch (c)
	{
		case '}':
			goto s1;
		default:
			goto s5;
	}

s5:
	clsource += (char)c;
	goto s3;

s6:
	csource += "$";
	goto s7;

s7:
	csource += (char)c;
	goto s1;

ex:
	return;
	// no cleanup to do
	// just exit
}

/* 
	Look for __kernel keyword in OpenCL source, collect the names
	of the kernels.
*/
void get_kernelnames(string clsource, vector <string> &kernelnames)
{
	stringstream s;
	s << clsource;

	string line;
	string name;
	getline(s, line);
	while (s.good())
	{
		int pos = 0;
		while ((line[pos] == ' ') || (line[pos] == '\t'))
		{pos++;}
		// 8 is length of __kernel
		if (line.substr(pos, 8) == "__kernel")
		{
			pos += 8;
			while (line[pos] == ' ' || line[pos] == '\t')
			{pos++;}
			while (line[pos] != ' ' && line[pos] != '\t')
			{pos++;} // pass over type (usually void)
			while (line[pos] == ' ' || line[pos] == '\t')
			{pos++;}
			while (line[pos] != ' ' && 
					line[pos] != '\t' &&
					line[pos] != '(')
			{
				name += line[pos];
				pos++;
			} 
			
			kernelnames.push_back(name);
			name = "";
		}
		getline(s, line);
	}
} // void get_kernelnames
