#include "strbuild.h"
#include "strnode.h"
extern char * get_prefix(char * filename);
extern char * rm_comments(char * filename, char * prefix);
extern char * handle_includes(char *filename, char * prefix);
extern void strip_cl(char *source,str_builder *clout,str_builder *srcout);
extern void get_kernelnames(str_builder *sb, strnode *kernelNames);
extern void get_kernelinfo(kernelnode *kn, strnode *kernelNames, 
	str_builder *src);
extern void handle_defines(char *source, 
				strnode *worksizes, strnode *alldefs);

