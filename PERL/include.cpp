#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>
using namespace std;

string processIncludes(string file, string temp_ext);

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		printf("Usage: %s <filename> <temp file extension>\n", argv[0]);
		return 0;
	}
	string filename = argv[1];
	string temp_ext = argv[2];

	string processed = processIncludes(filename, temp_ext);
	cout << processed;
	return 0;
}


string processIncludes(string file, string temp_ext)
{
	fstream fin;
	stringstream s;
	string command = "comments " + file + " > " + file + "." + temp_ext;

	system(command.c_str());
	file = file + "." + temp_ext;
	fin.open(file.c_str());

	string line;
	string temp;
	string filename;
	string processed;
	int pos;

	getline(fin, line);
	line = line + "\n";
	while (fin)
	{
		if (line.find("$include") != string::npos) 
		{
			// the next two lines tokenize the string on whitespace
			s << line;
			s >> temp;				
			while (temp != "$include")
			{
				processed = processed + temp;
				s >> temp;
			}
			s >> temp;
			processed = processed 
				+ processIncludes(temp.substr(1,temp.length()-2), temp_ext);
			getline(s, temp);
			processed = processed + temp;
		}
		else
		{processed = processed + line;}
		getline(fin, line);
		line = line + "\n";
	}
	fin.close();
	return processed;
}
