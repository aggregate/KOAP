// koap.h
#ifndef KOAP_H
#define KOAP_H

#include <vector> // these get used everywhere

// koap directive types
#define INVALID 0
#define GLOBAL 1
#define INIT 2
#define CLALLOC 3
#define CLWRITE 4
#define CALL 5
#define CLREAD 6
#define CLFREE 7
#define CLCLEANUP 8
#define DEFINE 9 


// Functions defined in include.cpp:
extern string process_includes(string file);

// Functions defined in comments.cpp:
extern string rm_comments(string filename);

// Functions defined in kernels.cpp:
extern void strip_cl(string source, string &clsource, string &csource);
extern void get_kernelnames(string clsource, vector <string> &kernelnames);

// Functions defined in gen.cpp:
extern void initCONSTS(bool updateConsts, ifstream &fin);
extern vector <string> genKstrDecl();
extern vector <string> genGlobal(string prefix, vector <string> knames);
extern vector <string> genClCleanup(vector <string> knames);
extern vector <string> genClFree(string buf);
extern vector <string> genClRead(string buf);
extern vector <string> genClCall(vector <string> args);
extern vector <string> genClWrite(string buf);
extern vector <string> genClalloc(vector <string> args);
extern vector <string> genInit(string devType, vector <string> knames,
	string clcflags);

// Functions defined in util.cpp:
extern vector <string> vectorFromStr(string src);
extern string replace(string src, string pattern, string replacement);
extern string get_f_prefix(string filename);
extern void printStrDeque(ostream& out, vector <string>, string term);

// a package for dealing with parsed KOAP directives
typedef struct
{
	int type;
	vector <string> args;
} directive_t;

// also in util.cpp:
extern directive_t match(string);

#endif
