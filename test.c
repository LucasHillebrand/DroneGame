#include <stdio.h>
#include "./strlib/string.h"

void printany(void* bytes,byte length){
    for (byte i=0;i<length;i++){
        printf("byte %u: %u,(%c)\n",i,*((byte*)(bytes+i)),*((byte*)(bytes+i)));
    }
}

int main(){
    printf("%lu\n",strToUlongI((byte*)"11"));
}
