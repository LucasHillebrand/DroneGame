#include "./config.h"
typedef unsigned char byte;
typedef unsigned int uint ;
typedef unsigned long ulong ;
typedef unsigned short ushort ;

typedef struct {
    byte reg[17][architecture];
} Core;

typedef struct{
    byte* ram;
    Core cores[corecount];
    byte status;
} CPU;

#define SINSSIZE 4
// REGISTER
#define R(a) a
#define CA 8
#define CB 9
#define CRADD 10
#define CRSUB 11
#define CRAND 12
#define CROR 13
#define PC 14
#define CID 15
#define MR 16

// ISA
#define NOOP 0x00
#define CPR 0x01
#define SRB 0x02
#define RWM 0x03
#define MWR 0x04
#define JMP 0x10
#define JIZ 0x11
#define JNZ 0x12
#define HLT 0x13
#define UPD 0x20

void CoreStep(CPU* self, byte core);
CPU CPU_create(byte* arrptr);
void step(CPU* self);
