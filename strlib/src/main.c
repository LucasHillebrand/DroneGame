#include "../string.h"

ptable createPtable(void* ptr, ulong size, ulong pagesize){
    size-=size%pagesize;
    ptable table = {
        .pagesize=pagesize,
        .mem=ptr,
        .size=size,
        .start=0
    };

    for (ulong i=0;i<table.size;i+=table.pagesize){
        for (byte j=0;j<sizeof(table.start);j++){
            ((byte*)table.mem)[i+j]=(byte)(((ulong)table.start>>j*8)%256);
        }
        table.start=table.mem+(i);
    }

    return table;
}
void* pallocate(ptable* self){
    void* ptr=self->start;
    for (byte i=0;i<sizeof(void*);i++){
        ((byte*)&self->start)[i]=((byte*)ptr)[i];
        ((byte*)ptr)[i]=0;
    }
    return ptr;
}
void pfree(ptable* self, void* ptr){
    for (byte i=0;i<sizeof(void*);i++){
        ((byte*)ptr)[i]=((byte*)&self->start)[i];
    }
    self->start=ptr;
}


ulong strlength(byte* str){
    ulong len=0;
    for (len=0;str[len]!=0;len++);
    return len;
}

splitString split(byte* string, byte* keyword, ptable* ptable){
    splitString out={
        .orgStr=string,
        .oslen=strlength(string),
        .splitVal=keyword,
        .tempStr=pallocate(ptable),
        .tslen=ptable->pagesize,
        .pos=0,
    };
    next(&out);
    return out;
}

void next(splitString* obj){
    ulong i=0;
    for (i=0; i<obj->tslen;i++)
        obj->tempStr[i]=0;
    for (i=obj->pos;!strstartswith(obj->orgStr+i, obj->splitVal) && i<obj->oslen;i++)
        obj->tempStr[i-obj->pos]=obj->orgStr[i];
    obj->pos=i+strlength(obj->splitVal);
}
void splitDestroy(splitString* sstring, ptable* ptable){
    pfree(ptable,sstring->tempStr);
}

byte* stringify(char* orgstr, ptable* ptable){
    byte* string = pallocate(ptable);
    for (ulong i=0;i<ptable->pagesize;i++)
        string[i]=0;
    ulong legnth = strlength((byte*)orgstr);
    for (ulong i=0;i<legnth;i++)
        string[i]=orgstr[i];
    return string;
}

byte strcomp(byte* strA, byte* strB){
    if (strlength(strA)!=strlength(strB))
           return 0;
    return strstartswith(strA, strB);
}
byte strstartswith(byte* strA, byte* strB){
    ulong lengthB=strlength(strB);
    if (strlength(strA)<strlength(strB))
        return 0;
    for (ulong i=0;i<lengthB;i++){
        if (strA[i]!=strB[i])
            return 0; //false
    }
    return 1; // true
}

ulong power(ulong base, ulong n){
    ulong res=1;
    for (ulong i=0;i<n;i++,res*=base);
    return res;
}

ulong strToUlong(byte* str,enum base numbase){
    ulong length=strlength(str);
    ulong n = 0;
    ulong res = 0;

    for (ulong i=length;i>0;i--)
        switch (str[i-1]) {
            case '0':
                res+=0*power(numbase,n);
                n++;
                break;
            case '1':
                res+=1*power(numbase,n);
                n++;
                break;
            case '2':
                res+=2*power(numbase,n);
                n++;
                break;
            case '3':
                res+=3*power(numbase,n);
                n++;
                break;
            case '4':
                res+=4*power(numbase,n);
                n++;
                break;
            case '5':
                res+=5*power(numbase,n);
                n++;
                break;
            case '6':
                res+=6*power(numbase,n);
                n++;
                break;
            case '7':
                res+=7*power(numbase,n);
                n++;
                break;
            case '8':
                res+=8*power(numbase,n);
                n++;
                break;
            case '9':
                res+=9*power(numbase,n);
                n++;
                break;
            case 'A':
            case 'a':
                res+=10*power(numbase,n);
                n++;
                break;
            case 'B':
            case 'b':
                res+=11*power(numbase,n);
                n++;
                break;
            case 'C':
            case 'c':
                res+=12*power(numbase,n);
                n++;
                break;
            case 'D':
            case 'd':
                res+=13*power(numbase,n);
                n++;
                break;
            case 'E':
            case 'e':
                res+=14*power(numbase,n);
                n++;
                break;
            case 'F':
            case 'f':
                res+=15*power(numbase,n);
                n++;
                break;

        }

    return res;
}
ulong strToUlongI(byte* str){
    if (strstartswith(str, (byte*)"0x"))
        return strToUlong(str+2, hex);
    if (strstartswith(str, (byte*)"0b"))
        return strToUlong(str+2, bin);
    if (strstartswith(str, (byte*)"0o"))
        return strToUlong(str+2, oct);
    return strToUlong(str, dec);

}
