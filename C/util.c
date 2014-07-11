#include <string.h>
#include <stdlib.h>

#include "strbuild.h"

char * get_prefix(char * filename)
{
	char *f_prefix;	

	int f_prefix_len = strlen(filename);
	while((f_prefix_len > 0) && 
		(filename[--f_prefix_len] != '.'))
	{} // find last '.' in filename

	if (f_prefix_len == 0)
	{f_prefix_len = strlen(filename);}

	f_prefix = (char *) malloc(sizeof(char) * (f_prefix_len + 1));
	if (f_prefix == NULL)
	{printf("malloc failed\nexiting...\n");exit(0);}
	// perhaps need a goto on exit?

	f_prefix[f_prefix_len] = '\0';

	int i; // iterator
	for (i = 0; i < f_prefix_len; i += 1)
	{f_prefix[i] = filename[i];}

	return f_prefix;
} // get_prefix
/*
char * getline(FILE *in)
{
	char *t = malloc(sizeof(char) * 2);
	t[1] = '\0';
	str_builder st;
	strbuild_init(&st, 128);
	int c;
	while (((c = getchar()) != EOF) && (c != '\n'))
	{
		t[0] = (char)c;
		strbuild_append(&st, t);
	} // end while
	if (c == '\n')
	{
		strbuild_append(&st, "\n");
	}
	return st.s;
} // getline
*/
