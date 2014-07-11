#ifndef STRNODE_H
#define STRNODE_H

#include "strbuild.h"
#define STRINIT	128

typedef struct strnode strnode;
struct strnode
{
	str_builder sb;
	strnode *next;
};

extern void strnode_init(strnode *sn);
extern void strnode_appendNode(strnode *sn, char *st);
extern void strnode_print(strnode *sn, FILE *stream);
extern void strnode_free(strnode *sn);

#endif
