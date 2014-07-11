#include <stdio.h>
#include "strnode.h"

int main(int argc, char ** argv)
{
	strnode head;
	strnode_init(&head);

	strnode *curr = &head;

	int i;
	for (i = 0; i < 5; i += 1)
	{
		strnode_appendNode(&head, "Strings\n");
	}

	curr = &head;
	while (curr != NULL)
	{
		strbuild_print(&(curr->sb), stdout);
		curr = curr->next;
	}

	for (i = 0; i < 5; i += 1)
	{
		strnode_appendNode(&head, "Added\n");
	}
	printf("Next Printing:\n");

	curr = &head;
	while (curr != NULL)
	{
		strbuild_print(&(curr->sb), stdout);
		curr = curr->next;
	}
	strnode_free(&head);
	return 0;
}
