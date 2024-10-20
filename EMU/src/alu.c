#include "../alu.h"

void ALU_add(Core* self){
    ushort res=0;
    for (byte i=0;i<architecture;i++){
        res=self->reg[CA][i]+self->reg[CB][i]+(res>>8);
        self->reg[CRADD][i]=res%256;
    }
}
void ALU_sub(Core* self){
    byte carr=0; // 1 / 0
    ushort temp=0;
    for (byte i=0;i<architecture;i++){
        temp=carr+self->reg[CB][i];
        carr=0;
        if (temp>self->reg[CA][i])
            carr=1;
        self->reg[CRSUB][i]=self->reg[CA][i]-temp%256;
    }
}

void ALU_and(Core* self){
    for (byte i=0;i<architecture;i++)
            self->reg[CRAND][i]=self->reg[CA][i]&self->reg[CB][i];
    }
void ALU_or(Core* self){
    for (byte i=0;i<architecture;i++)
        self->reg[CROR][i]=self->reg[CA][i]|self->reg[CB][i];
}

void ALU_upd(Core* self){
    ALU_add(self);
    ALU_sub(self);
    ALU_and(self);
    ALU_or(self);
}

ulong power(ulong base,ulong n){
    ulong res=1;
    for (ulong i=0;i<n;i++)
        res*=base;
    return res;
}

ulong ALU_toUlong(Core* self, byte reg){
    ulong res=0;
    for (byte i=0;i<architecture;i++)
        res+=self->reg[reg][i]*power(256, i);
    return res;
}

void ALU_pc_incr(Core* self){
    ushort res=0;
    res=SINSSIZE;
    res<<=8;
    for (byte i=0;i<architecture;i++){
        res=self->reg[PC][i]+(res>>8);
        self->reg[PC][i]=res%256;
    }
}
