// include.cpp
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>
using namespace std;

#include "koap.h"

// recursively defined, this is where all files get read in
string process_includes(string file)
{
	fstream fin;
	stringstream ncbuf;

	// ncbuf holds a comment filtered version of the file
	ncbuf << rm_comments(file);


	string line;
	string temp;
	stringstream s;
	string filename;
	string processed;
	int pos;

	getline(ncbuf, line);
	// add the newline, it was stripped off by getline
	line = line + "\n";
	while (ncbuf)
	{
		if (line.find("$include") != string::npos) 
		{
			// the next two lines tokenize the line on whitespace
			s << line;
			s >> temp;				
			/* 
				It really doesn't make much sense to have non-whitespace
				characters before the $include, but we'll write code
				for it anyway.
			*/
			// find the $include
			while (temp != "$include")
			{
				processed = processed + temp;
				s >> temp;
			}
			// put what we want to include in temp
			s >> temp;
			processed = processed 
				+ process_includes(temp.substr(1,temp.length()-2));
			getline(s, temp);
			// get the rest of the line
			processed = processed + temp;
		}
		else
		{processed = processed + line;}
		getline(ncbuf, line);
		// add the newline, it was stripped off by getline
		line = line + "\n";
	}
	// this is the entire source
	return processed;
}
