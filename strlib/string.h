typedef unsigned char byte;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;

#define KiB(s) (ulong)1024*s
#define MiB(s) (ulong)KiB(1024)*s
#define GiB(s) (ulong)MiB(1024)*s
#define TiB(s) (ulong)GiB(1024)*s

#define stdstrsize 128

typedef struct{
    void* mem;
    ulong size;
    void* start;
    ulong pagesize;
} ptable;

ptable createPtable(void* ptr, ulong size, ulong pagesize);
void* pallocate(ptable* self);
void pfree(ptable* self, void* ptr);

typedef struct{
    byte* orgStr;
    ulong oslen;
    byte* splitVal;
    byte* tempStr;
    ulong tslen;
    ulong pos;
} splitString;

enum base {
    hex=16,
    bin=2,
    oct=8,
    dec=10
};

ulong strlength(byte* str);
splitString split(byte* string, byte* keyword, ptable* ptable);
void splitDestroy(splitString* sstring, ptable* ptable);
byte* stringify(char* orgstr, ptable* ptable);
void next(splitString* obj);
byte strcomp(byte* strA, byte* strB);
byte strstartswith(byte* strA, byte* strB);
ulong strToUlong(byte* str,enum base numbase);
ulong strToUlongI(byte* str);
ulong power(ulong base, ulong n);
