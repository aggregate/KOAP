#include "strnode.h"
#include <stdlib.h>
#include <string.h>
void strnode_init(strnode *sn)
{
	strnode *curr = sn;
	strbuild_init(&(curr->sb), STRINIT);
	curr->next = NULL;
} // strnode_init

void strnode_appendNode(strnode *sn, char *st)
{
	strnode *curr = sn;
	while (curr->next != NULL)
	{curr = curr->next;}
	strbuild_append(&(curr->sb), st);
	curr->next = (strnode*) malloc(sizeof(strnode));
	curr = curr->next;
	strbuild_init(&(curr->sb), STRINIT);
	curr->next = NULL;
} // strnode_appendNode

void strnode_print(strnode *sn, FILE *stream)
{
	while (sn != NULL)
	{
		strbuild_print(&(sn->sb), stream);
		printf("\n");
		sn = sn->next;
	}
} // strnode_print

void strnode_free(strnode *sn)
{
	if (sn != NULL)
	{
		strnode_free(sn->next);
		strbuild_free(&(sn->sb));
	}
} // strnode_free
