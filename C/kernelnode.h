#ifndef KERNELNODE_H
#define KERNELNODE_H

#include "strbuild.h"
#include "strnode.h"

typedef struct kernelnode kernelnode;
struct kernelnode
{
	str_builder name;
	strnode argperms;
	strnode argtypes;
	strnode argsizes;
	int argc;	
	kernelnode *next;
};

extern void kernelnode_init(kernelnode *kn);
extern void kernelnode_free(kernelnode *kn);

#endif
