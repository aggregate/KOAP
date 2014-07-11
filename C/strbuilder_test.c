#include <stdio.h>
#include "strbuild.h"

int main(int argc, char ** argv)
{

	str_builder st;
	strbuild_init(&st, 100);
	strbuild_append(&st, "I am a string\n");

	int i = 0;
	for (i = 0; i < 1000; i += 1)
	{
		strbuild_append(&st, "I am a string");
	}

	strbuild_print(&st, stdout);
	strbuild_free(&st);
	return 0;
}
