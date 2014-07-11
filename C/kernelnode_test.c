#include <stdio.h>
#include <stdlib.h>
#include "kernelnode.h"

int main()
{
	kernelnode kns;

	kernelnode_init(&kns);

	strbuild_append(&(kns.name), "mykernel");
	strnode_appendNode(&(kns.argperms), "ro");
	strnode_appendNode(&(kns.argperms), "rw");
	strnode_appendNode(&(kns.argperms), "wo");
	strnode_appendNode(&(kns.argtypes), "float");
	strnode_appendNode(&(kns.argtypes), "int");
	strnode_appendNode(&(kns.argtypes), "thingy");
	strnode_appendNode(&(kns.argsizes), "NPROC");
	strnode_appendNode(&(kns.argsizes), "1");
	strnode_appendNode(&(kns.argsizes), "10");
	kns.argc = 3;

	kns.next = (kernelnode *) malloc(sizeof(kernelnode));
	kernelnode_init(kns.next);

	strbuild_append(&(kns.next->name), "otherkernel");
	strnode_appendNode(&(kns.next->argperms), "rw");
	strnode_appendNode(&(kns.next->argperms), "wo");
	strnode_appendNode(&(kns.next->argtypes), "float");
	strnode_appendNode(&(kns.next->argtypes), "double");
	strnode_appendNode(&(kns.next->argsizes), "NPROC");
	strnode_appendNode(&(kns.next->argsizes), "50");
	kns.next->argc = 2;

	kernelnode *curr = &kns;
	while (curr != NULL)
	{
		strbuild_print(&(curr->name), stdout);
		strnode_print(&(curr->argperms), stdout);
		strnode_print(&(curr->argtypes), stdout);
		strnode_print(&(curr->argsizes), stdout);
		printf("argc: %d\n", curr->argc);
		curr = curr->next;
	}
	
	kernelnode_free(&kns);	
	return 0;
}
