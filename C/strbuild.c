#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "strbuild.h"

void strbuild_init(str_builder *st, int size)
{
	st->s = (char *) malloc(sizeof(char) * size);
	st->s[0] = '\0';
	st->size = size;
	st->used = 0;
} // init

void strbuild_append(str_builder *st, char *app)
{
	if ((st->used + strlen(app) + 1) > st->size)
	{
		st->size *= 2;
		char *t = (char *) malloc(sizeof(char) * st->size);
		strcpy(t, st->s);
		strcat(t, app);
		free(st->s);
		st->s = t;
		t = NULL; // DO NOT FREE t!
	}
	else
	{
		strcat(st->s, app);
	}
	st->used += strlen(app);
} // append

void strbuild_print(str_builder *st, FILE *stream)
{
	fprintf(stream, "%s", st->s);	
}
void strbuild_free(str_builder *st)
{
	free(st->s);
	st->size = 0;
	st->used = 0;
}

