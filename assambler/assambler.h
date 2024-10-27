#include "../strlib/string.h"
#include "../EMU/config.h"
#include <stdio.h>
#include <stdlib.h>

#define maxsrcfiles 128
#define maxvars 8192

struct parameters {
    byte* output; // output filename
    byte* debugfile; // when you want to run a compiled file
    byte* srcfiles[128]; // all the source code files
    ulong fc; // file count

    byte df; // debug flag
    byte cf; // compile flag
};

struct varval { // access $[name]:[0-7]
    byte* name;
    byte val[architecture];
};

struct varfam{
    ptable pstr; // strings
    ptable pvv; // vartype

    struct varval* pub[maxvars/2];
    ulong pubcount;
    struct varval* priv[maxvars/2];
    ulong privcount;
};

enum vartype {
    pub=0,
    priv=1
};

struct varfam createvf();
void newvar(struct varfam* self, char* name, ulong value, enum vartype type);
struct varval* get(byte* str, struct varfam* fam);
void clrpriv(struct varfam* fam);
void vardestroy(struct varfam* fam);

void collectvars(FILE* file, ulong* pc, struct varfam* fam);
void resolveins(FILE* file, FILE* output, struct varfam* fam);

byte resolvestatement(byte* str, struct varfam* fam);
