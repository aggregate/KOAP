#include "kernelnode.h"

void kernelnode_init(kernelnode *kn)
{
	strbuild_init(&(kn->name), STRINIT);
	strnode_init(&(kn->argperms));
	strnode_init(&(kn->argtypes));
	strnode_init(&(kn->argsizes));
	kn->argc = 0;
	kn->next = NULL;
}

void kernelnode_free(kernelnode *kn)
{
	if (kn != NULL)
	{
		kernelnode_free(kn->next);
		strnode_free(&(kn->argperms));
		strnode_free(&(kn->argtypes));
		strnode_free(&(kn->argsizes));
		strbuild_free(&(kn->name));
	}
}
