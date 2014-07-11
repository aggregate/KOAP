#include <string>
#include <stdio.h>
using namespace std;

int main(int argc, char** argv)
{
	if (argc != 2)
	{printf("Usage: ./%s [string to parse]\n", argv[0]);}
	string input = argv[1];

	int i = 0;
	char c;
	int state = 0;
	string curr = "";

s0:
	c = input[i++];	
	switch(c)
	{
		case '(':
			goto s5;
		default:
			goto s1;
	}
s1:
	curr += c;
	goto s0;
s2:
	c = input[i++];
	switch (c)
	{
		case '(':
			goto s3;
		case ',':
			goto s5;
		case ')':
			goto exit;
		default:
			goto s4;
	}
s3:
	curr += c;
	c = input[i++];
	switch (c)
	{
		case ')':
			goto s4;
		default:
			goto s3;
	}
s4:
	curr += c;
	goto s2;
s5:
	printf("%s\n", curr.c_str());
	curr = "";
	goto s2;
exit:
	printf("%s\n", curr.c_str());
	return 0;	
}
