#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "koabdefs.h"
#include "strbuild.h"
extern char * rm_comments(char *filename, char *prefix);
char * process_includes(char *filename, char *prefix);

char * handle_includes(char *filename, char *prefix)
{
	char *processed = process_includes(filename, prefix);

	return processed;
}

char * process_includes(char *filename, char *prefix)
{
	char *nc;
	str_builder ln;
	str_builder rt;
	strbuild_init(&ln, 128);
	strbuild_init(&rt, 1024);

	nc = rm_comments(filename, prefix);

	int pos = 0;
	char *t = malloc(sizeof(char) * 2);
	t[1] = '\0';
	
	while ((nc[pos] != '\n') && (nc[pos] != '\0'))	
	{
		t[0] = nc[pos];
		strbuild_append(&ln, t);	
		pos += 1;
	}
	if (nc[pos] == '\n')
	{
		strbuild_append(&ln, "\n");
	}
	char *p = NULL;
	char *curr = ln.s;
	// TODO: this probably isn't the condition you want to use
	while (nc[pos] != '\0')
	{
		pos += 1;
		if ((p = strstr(ln.s, "$include")) != NULL)
		{
			printf("Found include: %s\n", p);
			while (curr != p)
			{
				t[0] = *curr;
				strbuild_append(&rt, t);
				curr++;
			} // while
			// everything up to $ has been eaten	
			while (*curr != '\0' && *curr != '\"')
			{curr++;} // while
			if (*curr == '\0')
			{printf("Bad syntax on include.\n"); exit(0);}
			// now at beginning of filename
			char *fnst = ++curr;
			int l = 0;
			while (*curr != '\0' && *curr != '\"')
			{l += 1;curr++;}
			// now at double quote after filename
			char *hfilename = (char *) malloc((sizeof(char) * l) + 1);
			strncpy(hfilename, fnst, l);
			hfilename[l] = '\0';
			char *processedhfile = process_includes(hfilename, prefix);	
			strbuild_append(&rt, processedhfile);
			strbuild_append(&rt, ++curr);
			free(hfilename);
			free(processedhfile);
		} // if
		else
		{strbuild_append(&rt, ln.s);}
		// now, reset line w/o reallocating
		ln.s[0] = '\0';
		ln.used = 0;
		while ((nc[pos] != '\n') && (nc[pos] != '\0'))	
		{
			t[0] = nc[pos];
			strbuild_append(&ln, t);	
			pos += 1;
		}
		if (nc[pos] == '\n')
		{
			strbuild_append(&ln, "\n");
		}
	} // while
	strbuild_free(&ln);
	free(nc);
	free(t);
	return rt.s;
} // process_includes
