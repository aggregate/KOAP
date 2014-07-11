#include <string>
#include <cstdlib>
#include <stdio.h>
using namespace std;

#include "koab.h"
void handle_defines(string &source, deque <string> &worksizes, 
	deque <string> &alldefs)
{
	int pos = 0;
	int ls;
	bool invalid;
	while ((pos = source.find("$define", pos)) != string::npos)
	{

		ls = source.rfind("\n", pos);
		invalid = false;
		for (int i = ls+1; i < pos; i += 1)
		{
			if (source[i] != ' ' && source[i] != '\t')
			{invalid = true;}
		}
		if (!invalid)
		{
			source[pos] = '#';
			if (source.substr(pos, 13) == "#define SIMDS")
			{
				worksizes.push_back(source.substr(pos, 13));
				do {
					source[pos] = ' ';
				} while (source[++pos] != '\n');
			}
			else if (source.substr(pos, 16) == "#define MAXBLOCK")
			{
				worksizes.push_back(source.substr(pos, 16));
				do {
					source[pos] = ' ';
				} while (source[++pos] != '\n');
			}
			else
			{
				int temp = source.find("\n", pos);
				alldefs.push_back(
					source.substr(pos, source.find("\n", pos)-pos+1)
				);
			} // if/else
		} // valid/invalid if
		else
		{ pos += 1; }
		// no need to look at this index again in the next iteration
	} // while
} // void handle_defines
