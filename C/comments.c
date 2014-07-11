#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "koabdefs.h"
#include "strbuild.h"

char * rm_comments(char *filename, char *prefix)
{
	FILE *fin;
	fin = fopen(filename, "r");

	str_builder sb;
	strbuild_init(&sb, 128);	

	int c;

s1:
	c = fgetc(fin);
	if (c == EOF) {goto ex;}
	switch (c)
	{
		case '/':
			goto s2;
		default:
		{
			char *t = malloc(sizeof(char)+1);
			sprintf(t, "%c", c);
			strbuild_append(&sb, t);
			free(t);	
			goto s1;
		}
	} // end switch

s2:
	c = fgetc(fin);
	if (c == EOF) {goto ex;}
	switch (c)
	{
		case '*':
			goto s3;
		case '/':
			goto s5;
		default:
			goto s6;
	} // end switch

s3:
	c = fgetc(fin);
	if (c == EOF) {goto ex;}
	switch (c)
	{
		case '*':
			goto s4;
		case '\n':
		{	
			char *t = malloc(sizeof(char)+1);
			sprintf(t, "%c", c);
			strbuild_append(&sb, t);
			free(t);	
			goto s3;
		}
		default:
			goto s3;
	} // switch

s4:
	c = fgetc(fin);
	if (c == EOF) {goto ex;}
	switch (c)
	{
		case '/': 
			goto s1;
		case '\n': 
		{
			char *t = malloc(sizeof(char)+1);
			sprintf(t, "%c", c);
			strbuild_append(&sb, t);
			free(t);	
			goto s3;
		}
		default: 
			goto s3;
	} // switch

s5:
	c = fgetc(fin);
	if (c == EOF) {goto ex;}
	switch (c)
	{
		case '\n': 
		{
			char *t = malloc(sizeof(char)+1);
			sprintf(t, "%c", c);
			strbuild_append(&sb, t);
			free(t);	
			goto s1; 
		}
		default: 
			goto s5;
	} // switch

s6:
	strbuild_append(&sb, "/");
	char *t = malloc(sizeof(char)+1);
	sprintf(t, "%c", c);
	strbuild_append(&sb, t);
	free(t);	
	goto s1;

ex:
	fclose(fin);
	return sb.s;
} // rm_comments
