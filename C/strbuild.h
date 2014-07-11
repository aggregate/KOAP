#ifndef STRBUILD_H
#define STRBUILD_H
#include <stdio.h>
typedef struct
{
	char *s;
	int size;
	int used;
} str_builder;

extern void strbuild_init(str_builder *st, int size);
extern void strbuild_append(str_builder *st, char *app);
extern void strbuild_print(str_builder *st, FILE *stream);
extern void strbuild_free(str_builder *st);
#endif
