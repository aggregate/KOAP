#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "koab.h"

void strip_cl(char *source, str_builder *clout, str_builder *srcout)
{
	char t[2];
	t[1] = '\0';
	int c;
	int pos = 0;

s1:
	c = source[pos++];
	if (c == '\0') {goto ex;}
	switch (c)
	{
		case '$':
			goto s2;
		default:
			goto s7;
	}
s2:
	c = source[pos++];
	if (c == '\0') {goto ex;}
	switch (c)
	{
		case '{':
			goto s3;
		default:
			goto s6;
	}

s3:
	c = source[pos++];
	if (c == '\0') {goto ex;}
	switch (c)
	{
		case '$':
			goto s4;
		default:
			goto s5;
	}

s4:
	c = source[pos++];
	if (c == '\0') {goto ex;}
	switch (c)
	{
		case '}':
			goto s1;
		default:
			goto s5;
	}

s5:
	t[0] = c;
	strbuild_append(clout, t);
	goto s3;

s6:
	strbuild_append(srcout, "$");
	goto s7;

s7:
	t[0] = c;
	strbuild_append(srcout, t);
	goto s1;

ex:
	return;
	// no cleanup to do
	// just exit
}

void get_kernelnames(str_builder *sb, strnode *kernelNames)
{
	// look at beginning, to match __kernel
	char *pos = sb->s;
	while (*pos != '\0')
	{
		while (*pos == '\n' || *pos == '\t' || *pos == ' ')
		{pos++;} // skip whitespace
		// if match
		if (strncmp(pos, "__kernel", strlen("__kernel")) == 0)
		{
			char *lastWS = pos;
			while (*pos	!= '(')
			{
				if (*pos == '\n' || *pos == '\t' || *pos == ' ')
				{lastWS = pos;}
				pos++;
			} // parens while
			// should have pointers to character before and character after
			// the kernel name.
			int len = pos-lastWS;
			char *tmp = (char *) malloc(sizeof(char) * len);
			// that allocation DOES leave room for the '\0'
			tmp[len] = '\0';
			lastWS++; // now points to beginning of kernel name
			strncpy(tmp, lastWS, len-1);
			printf("Kernel found: %s\n", tmp);
			strnode_appendNode(kernelNames, tmp);
			free(tmp);
		} // __kernel if 
		while (*pos != '\n'  && *pos != '\0')	
		{pos++;} // go to end of line
	} // clsource processing while 
} // void get_kernelnames

void get_kernelinfo(kernelnode *kn, strnode *kernelNames, str_builder *src)
{
	// find an instance of $kernel
	// Now, check to make sure it's the first thing on the line.
	// Yes, this will allow poorly written multi-line string literalls
	// to be parsed, but that's the programmer's fault.
	int i;
	int skip;
	char *pos;
	do	
	{
		pos = strstr(src, "$kernel");
		i = 0;
		skip = 0;
		while (((pos-i) != src) &&
				(*(pos-i) != '\n'))
		{
			if ((*(pos-i) != '\t') &&
				(*(pos-i) != ' '))
			{skip = 1;}
			i++;
		}
	} while (skip);
	// check to see if we have kernel code for this kernel
	str_builder kname;
	strbuild_init(&kname, 32);
	
	strnode *knpos = kn;
	while knpos->next != NULL;
	{
		knpos->sb.s
	}
	// if so:
	//		add to used kernels array
	// if not found:
	//		error and exit
	// replace $kernel directive w/ newline
	// go to next source line
	// if the line begins w/ permissions spec, enter loop
	// while this line starts w/ a permissions spec:
	//		break the line into tokens on whitespace
	//		first token is permissions
	//		second token is type
	//		third token is size, if no third, default to 1
	//		go to next source line, repeat loop
	// keep a count of lines processed, this is argc for the kernel
}
