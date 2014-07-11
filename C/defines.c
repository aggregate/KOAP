#include "koab.h"
#include <string.h>
#include <stdlib.h>

void handle_defines(char *source, strnode *worksizes, strnode *alldefs)
{
	char *pos;
	while ((pos = strstr(source, "$define")) != NULL)
	{
		*pos = '#';	
		if ((pos == strstr(pos, "#define SIMDS")) ||
			(pos == strstr(pos, "#define MAXBLOCK")))
		{
			// first, add to worksizes list
			int len = strstr(pos, "\n") - pos + 1;
			char *tmp = (char *) malloc(sizeof(char) * len);
			tmp[len] = '\0';
			strncpy(tmp, pos, len-1);
			strnode_appendNode(worksizes, tmp);
			free(tmp);
			// remove #define from source
			do {
				*pos = ' ';
			} while (*(++pos) != '\n');
		}
		else
		{
			// add to alldefs list
			int len = strstr(pos, "\n") - pos + 1;
			char *tmp = (char *) malloc(sizeof(char) * len);
			tmp[len] = '\0';
			strncpy(tmp, pos, len-1);
			strnode_appendNode(alldefs, tmp);
			free(tmp);
		} // end of if/else
	} // source processing while
} // void handle_defines
