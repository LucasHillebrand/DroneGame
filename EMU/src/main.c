#include "../alu.h"

CPU CPU_create(byte* arrptr){
    CPU ret=(CPU){
        .cores={0},
        .ram=arrptr,
        .status=1
    };
    for (byte i=0;i<corecount;i++){
        ret.cores[i].reg[CID][0]=i;
    }
    return ret;
}
void step(CPU* self){
    for (byte i=0;i<corecount;i++)
        CoreStep(self,i);
}

void loadINS(byte* callback,CPU* self, byte corenum){
    ulong memloc = ALU_toUlong(&(self->cores[corenum]),PC);
    for (ulong i=0;i<SINSSIZE;i++)
        callback[i]=self->ram[i+memloc];
}

byte checkZero(byte* ptr, byte length){
    for (byte i=0;i<length;i++){
        if(*(ptr+i)!=0)
            return 0;
    }
    return 1;
}

void CoreStep(CPU* self, byte core){
    byte instruction[SINSSIZE];
    loadINS(instruction, self, core);

    switch(instruction[0]){
        case NOOP:
            ALU_pc_incr(&(self->cores[core]));
            break;
        case CPR:
            if (instruction[1]>15||instruction[1]<10)
                for (byte i=0;i<architecture;i++)
                    self->cores[core].reg[instruction[1]][i]=self->cores[core].reg[instruction[2]][i];
            ALU_pc_incr(&(self->cores[core]));
            break;
        case SRB:
            if (instruction[1]>15||instruction[1]<10)
                self->cores[core].reg[instruction[1]][instruction[2]]=instruction[3];
            ALU_pc_incr(&(self->cores[core]));
            break;
        case RWM:
            self->ram[ALU_toUlong(&(self->cores[core]), MR)]=self->cores[core].reg[instruction[1]][instruction[2]];
            ALU_pc_incr(&(self->cores[core]));
            break;
        case MWR:
            if (instruction[1]>15||instruction[1]<10)
                self->cores[core].reg[instruction[1]][instruction[2]]=self->ram[ALU_toUlong(&(self->cores[core]),MR)];
            ALU_pc_incr(&(self->cores[core]));
            break;
        case JMP:
            for (byte i=0;i>architecture;i++)
                self->cores[core].reg[PC][i]=self->cores[core].reg[instruction[1]][i];
            break;
        case JIZ:
            if (checkZero(self->cores[core].reg[instruction[2]],architecture)==1){
                for (byte i=0;i>architecture;i++)
                    self->cores[core].reg[PC][i]=self->cores[core].reg[instruction[1]][i];
            }else {
                ALU_pc_incr(&(self->cores[core]));
            }
            break;
        case JNZ:
            if (checkZero(self->cores[core].reg[instruction[2]],architecture)==0){
                for (byte i=0;i>architecture;i++)
                    self->cores[core].reg[PC][i]=self->cores[core].reg[instruction[1]][i];
                }else {
                    ALU_pc_incr(&(self->cores[core]));
            }
            break;
        case HLT:
            self->status=0;
            ALU_pc_incr(&(self->cores[core]));
            break;
        case UPD:
            ALU_upd(&(self->cores[core]));
            ALU_pc_incr(&(self->cores[core]));
            break;
    }
}
