#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "../../EMU/CPUEMU.h"
#include "../assambler.h"

int main(int argc, char** argv){
    struct parameters params = {0};
    for (ulong i=1;i<argc;i++){
        if (strcomp((byte*)argv[i], (byte*)"-o") && i+1 < argc){
            params.output=(byte*)argv[i+1];
            params.cf=1;
            i++;
        }
        else if (strcomp((byte*)argv[i], (byte*)"-d") && i+1 < argc){
            params.debugfile=(byte*)argv[i+1];
            params.df=1;
            i++;
        }
        else{
            params.srcfiles[params.fc]=(byte*)argv[i];
            params.fc++;
        }
        }

    printf("Params:\n\
        debug file: (%s)\n\
        output file: (%s)\n\n",params.debugfile,params.output);
    for (ulong i=0;i<params.fc;i++){
        printf("\tinput file [%lu]: %s\n",i,params.srcfiles[i]);
    }

    if (params.cf){
        struct varfam fam = createvf();
        ulong pc=0;
        FILE* out = fopen((char*)params.output, "w");
        FILE* in = 0;
        for (ulong i=0;i<params.fc;i++){
            in=fopen((char*)params.srcfiles[i], "r");
            collectvars(in, &pc,&fam);
            resolveins(in, out, &fam);
            clrpriv(&fam);
            fclose(in);
        }
        fclose(out);
    }
    if (params.df){
        byte* mem = malloc(GiB(1));
        FILE* deb = fopen((char*)params.debugfile, "r");
        ushort ch = 0;
        for (ulong i=0;(short)ch!=EOF && i<GiB(1);i++){
            ch=fgetc(deb);
            mem[i]=ch%256;
        }
        CPU cpu = CPU_create(mem);
        while (cpu.status){
            step(&cpu);
        }

        for (ulong i=0;i<corecount;i++){
            printf("CORE (%lu)\n",i);
            for (ulong j=0;j<17;j++){
                printf("\t");
                for (ulong k=0;k<architecture;k++)
                    printf("%u, ",cpu.cores[i].reg[j][k]);
                printf("R%lu\n",j);
            }
        }

        fclose(deb);
        free(mem);
    }
}

struct varfam createvf(){
    return (struct varfam){
        .pstr=createPtable(malloc(maxvars*stdstrsize), maxvars*stdstrsize, stdstrsize),
        .pvv=createPtable(malloc(sizeof(struct varval)*maxvars),sizeof(struct varval)*maxvars, sizeof(struct varval)),
        .pubcount=0,
        .privcount=0
    };
}
void newvar(struct varfam* self, char* name, ulong value, enum vartype type){
    byte* stor;
    switch (type) {
        case priv:
            self->priv[self->privcount]=pallocate(&self->pvv);
            stor=self->priv[self->privcount]->val;
            self->priv[self->privcount]->name=stringify(name, &self->pstr);
            self->privcount++;
            break;
        case pub:
            self->pub[self->pubcount]=pallocate(&self->pvv);
            stor=self->pub[self->pubcount]->val;
            self->pub[self->pubcount]->name=stringify(name, &self->pstr);
            self->pubcount++;
            break;
    }
    for (ulong i=0; i < architecture;i++){
        stor[i]=value%256;
        value-=value%256;
        value/=256;
    }
}
struct varval* get(byte* str, struct varfam* fam){
    struct varval* ptr=0;
    for (ulong i=0;i<fam->privcount;i++)
            if (strcomp(fam->priv[i]->name,str))
                ptr=fam->priv[i];
    for (ulong i=0;i<fam->pubcount;i++)
            if (strcomp(fam->pub[i]->name,str))
                ptr=fam->pub[i];
    return ptr;
}
void clrpriv(struct varfam* fam){
    for (ulong i=0;i<fam->privcount;i++){
        pfree(&fam->pstr,fam->priv[i]->name);
        pfree(&fam->pvv,fam->priv[i]);
    }
    fam->privcount=0;
}
void vardestroy(struct varfam* fam){
    for (ulong i=0;i<fam->privcount;i++){
        pfree(&fam->pstr,fam->priv[i]->name);
        pfree(&fam->pvv,fam->priv[i]);
    }
    clrpriv(fam);
    free(fam->pstr.mem);
    free(fam->pvv.mem);
}

ushort gethex(byte* str){
    if (strstartswith(str, (byte*)"noop"))
        return NOOP;
    else if (strstartswith(str, (byte*)"cpr"))
        return CPR;
    else if (strstartswith(str, (byte*)"srb"))
        return SRB;
    else if (strstartswith(str, (byte*)"rwm"))
        return RWM;
    else if (strstartswith(str, (byte*)"mwr"))
        return MWR;
    else if (strstartswith(str, (byte*)"jmp"))
        return JMP;
    else if (strstartswith(str, (byte*)"jiz"))
        return JIZ;
    else if (strstartswith(str, (byte*)"jnz"))
        return JNZ;
    else if (strstartswith(str, (byte*)"hlt"))
        return HLT;
    else if (strstartswith(str, (byte*)"upd"))
        return UPD;
    else
        return 0x100; // command not found
}

byte* nextL(FILE* file, ptable* Pstr, byte* eof){
    byte* string=pallocate(Pstr);
    ushort ch = 1;
    for (ulong i=0;i<Pstr->pagesize-1 && (short)ch!=EOF && ch != '\n'; i++){
        ch=fgetc(file);
        if(ch != '\n'&&(short)ch !=EOF){
            string[i]=ch;
            string[i+1]=0;
        }
        if ((short)ch == EOF)
            *eof=1;
    }
    return string;
}

void collectvars(FILE* file, ulong *pc, struct varfam* fam){
    fseek(file, 0,0);
    byte temp[stdstrsize*3];
    ptable Pstr = createPtable(temp, stdstrsize*3, stdstrsize);
    byte* line = 0;
    byte eof=0;
    while (!eof){
        line = nextL(file, &Pstr,&eof);
        ushort start=gethex(line);
        if (start != 0x100)
            *pc+=4;
        splitString s = split(line, (byte*)", ",&Pstr);
        byte* name = 0;
        if (strcomp(s.tempStr,(byte*)"priv")){
            next(&s);
            name = stringify((char*)s.tempStr,&Pstr);
            next(&s);
            if (!strcomp(s.tempStr,(byte*)"."))
                newvar(fam, (char*)name, strToUlongI(s.tempStr), priv);
            if (strcomp(s.tempStr,(byte*)"."))
                newvar(fam, (char*)name, *pc, priv);
            pfree(&Pstr, name);
        }
        if (strcomp(s.tempStr,(byte*)"pub")){
            next(&s);
            name = stringify((char*)s.tempStr,&Pstr);
            next(&s);
            if (!strcomp(s.tempStr,(byte*)"."))
                newvar(fam, (char*)name, strToUlongI(s.tempStr), pub);
            if (strcomp(s.tempStr,(byte*)"."))
                newvar(fam, (char*)name, *pc, pub);
            pfree(&Pstr, name);
        }
        splitDestroy(&s, &Pstr);
        pfree(&Pstr, line);
    }
}

byte resolvestatement(byte* str, struct varfam* fam){
    if(strcomp(str, (byte*)"R0"))
        return 0;
    if(strcomp(str, (byte*)"R1"))
        return 1;
    if(strcomp(str, (byte*)"R2"))
        return 2;
    if(strcomp(str, (byte*)"R3"))
        return 3;
    if(strcomp(str, (byte*)"R4"))
        return 4;
    if(strcomp(str, (byte*)"R5"))
        return 5;
    if(strcomp(str, (byte*)"R6"))
        return 6;
    if(strcomp(str, (byte*)"R7"))
        return 7;
    if(strcomp(str, (byte*)"CA"))
        return CA;
    if(strcomp(str, (byte*)"CB"))
        return CB;
    if(strcomp(str, (byte*)"CRADD"))
        return CRADD;
    if(strcomp(str, (byte*)"CRSUB"))
        return CRSUB;
    if(strcomp(str, (byte*)"CRAND"))
        return CRAND;
    if(strcomp(str, (byte*)"CROR"))
        return CROR;
    if(strcomp(str, (byte*)"PC"))
        return PC;
    if(strcomp(str, (byte*)"CID"))
        return CID;
    if(strcomp(str, (byte*)"MR"))
        return MR;

    if(strstartswith(str, (byte*)"$")){
        byte mem[stdstrsize]={0};
        ptable Pstr=createPtable(mem, stdstrsize, stdstrsize);
        splitString s = split(str+1, (byte*)":", &Pstr);
        ulong pos=strToUlongI(s.tempStr);
        next(&s);
        struct varval* vv = get(s.tempStr, fam);
        splitDestroy(&s, &Pstr);
        if (vv != 0)
            return vv->val[pos];
        return 0;
    }
    return strToUlongI(str);
}

void resolveins(FILE* file, FILE* output, struct varfam* fam){
    fseek(file, 0, 0);
    byte buff[stdstrsize*2];
    ptable Pstr = createPtable(buff, stdstrsize*2, stdstrsize);
    byte eof=0;
    while (eof == 0){
        byte* line = nextL(file, &Pstr, &eof);
        splitString sl = split(line, (byte*)", ", &Pstr);
        ushort ins = gethex(sl.tempStr);
        if (ins != 0x100){
            fputc(ins%256, output);
            for (byte i=0;i<3;i++){
                next(&sl);
                fputc(resolvestatement(sl.tempStr, fam), output);
            }
        }
        splitDestroy(&sl, &Pstr);
        pfree(&Pstr, line);
    }
}
