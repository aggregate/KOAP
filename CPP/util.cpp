#include <iostream> // I/O operators, ostream
#include <string>
#include <vector>
#include <stdio.h> // fprintf
#include <cstdlib> // exit
#include <sstream>
using namespace std;
#include "koap.h"


// break the sting on newlines (\n)
// insert pieces into vector
vector <string> vectorFromStr(string src)
{
	stringstream s;
	s.str(src);

	vector <string> d;
	string line;
	while (s)
	{
		getline(s, line);
		d.push_back(line+"\n");	
	}	

	return d;
}

// replace all instances of pattern with replacement
// returns a new string
// note: pattern is a literal, not a regex
string replace(string src, string pattern, string replacement)
{
	string rt;
	int pos = 0;
	int newpos = 0;
	while ((newpos = src.find(pattern, pos)) != string::npos)
	{
		rt.append(src.substr(pos, newpos));
		rt.append(replacement);	
		pos = newpos + 1;
	}
	if (pos == 0 && newpos == string::npos)
	{
		return src;
	}
	return rt;
}

// take everything that isn't after the last '.'
string get_f_prefix(string filename)
{
	int extstart = filename.find_last_of('.');

	if (extstart == string::npos)
	{extstart = filename.length();}

	return filename.substr(0, extstart);
}

// print the elements 
void printStrDeque(ostream& out, vector <string> d, string term)
{
	for (int i = 0; i < d.size(); i++)
	{out << d[i] << term;}
}

// a somewhat loose definition of white space
bool isWs(char c)
{
	return (c == ' ' || c == '\t');
}

// this really doesn't need explaination
int ignoreWs(string line, int pos)
{
	while (pos < line.length() && isWs(line[pos]))
	{pos++;}
	return pos;	
}

/* 
	Return the next thing from the argument list (assumes we're in one).
	I think this accounts for most of what can appear in an argument list,
	but I could be wrong. It should at least handle function calls inside
	the list.
*/
string takeArg(string line, int &i)
{
	string rt;
	int nestdepth = 0;
	while (i < line.size() &&
			((line[i] != ',' && line[i] != ')') || nestdepth > 0))
	{
		if (line[i] == '(') {nestdepth++;}
		else if (line[i] == ')') {nestdepth--;}
		rt.append(1, line[i]);
		i += 1;
	}
	return rt;
}


// Does this line contain a directive?
// If so, parse it, and return the result.
// Otherwise, return a directive with INVALID type
directive_t match(string line)
{
	// ignore whitespace at the beginning of the line
	int i = ignoreWs(line, 0);

	int dirStart = i; // beginning of the directive, hopefully

	// get index up to '(', if it exists
	while (i < line.length() && !isWs(line[i]) && line[i] != '(')
	{i++;}

	directive_t d;
	d.type = INVALID;

	// define the directives to be parsed 
	// 0 is INVALID, so set that index to blank
	#define NUMDIRECTIVES 10 
	const string directives[NUMDIRECTIVES] = 
	{"", "$global", "$init", "$clalloc", "$clwrite", "$call", "$clread",
		"$clfree", "$clcleanup", "$define"};

	// if this directive is recognized, set the type
	for (int j = 0; j < NUMDIRECTIVES; ++j)
	{
		if (line.find(directives[j]) == dirStart)
		{d.type = j;}	
	}

	switch (d.type)
	{
		case (CLALLOC):
			// parse args
			while (i < line.length() && line[i] != ')')
			{
				i++;
				i = ignoreWs(line, i);
				d.args.push_back(takeArg(line, i));
			}
			if (d.args.size() != 4)
			{
				fprintf(stderr, "Malformed $clalloc directive: Expecting");
				fprintf(stderr, " %d arguments, got %d\n",
						4,(int) d.args.size());
				fprintf(stderr, "Parsing: %s\n", line.c_str());
				exit(0);
			}
			break;
		case (CLWRITE):
			// parse args
			while (i < line.length() && line[i] != ')')
			{
				i++;
				i = ignoreWs(line, i);
				d.args.push_back(takeArg(line, i));
			}
			break;
		case (CALL):
			// parse kernel name from text
			d.args.push_back("");
			i = ignoreWs(line, i);
			while (i < line.length() && line[i] != '(')
			{
				d.args[d.args.size()-1].append(1, line[i]);
				i++;
			}
			// parse args
			while (i < line.length() && line[i] != ')')
			{
				i++;
				i = ignoreWs(line, i);
				d.args.push_back(takeArg(line, i));
			}
			break;
		case (CLREAD):
			// parse args
			while (i < line.length() && line[i] != ')')
			{
				i++;
				i = ignoreWs(line, i);
				d.args.push_back(takeArg(line, i));
			}
			break;
		case (CLFREE):
			// parse args
			while (i < line.length() && line[i] != ')')
			{
				i++;
				i = ignoreWs(line, i);
				d.args.push_back(takeArg(line, i));
			}
			break;
		case (DEFINE):
			/*
				i is no good in this case, as we moved it up to the first
				instance of '(', which won't appear in the right place
				for a $define, if it appears at all.
			*/ 
			i = ignoreWs(line, 0);
			while (!isWs(line[i]))
			{i++;} // ignore $define
			i = ignoreWs(line, i);	
			d.args.push_back("");
			/*
				There should be exactly two more tokens, get them, ignore 
				the rest.
			*/
			while (line[i] != '\n')
			{
				d.args[d.args.size()-1].append(1, line[i]);
				i++;
			}
			i = ignoreWs(line, i);	
			d.args.push_back("");
			while (!isWs(line[i]) && line[i] != '\n')
			{
				d.args[d.args.size()-1].append(1, line[i]);	
				i++;
			}
			break;
		default: // for all directives that have no arguments
			// parse nothing
			break;
	} // end switch

	return d;
}
