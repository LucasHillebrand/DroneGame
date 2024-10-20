#include "CPUEMU.h"

void ALU_add(Core* self);
void ALU_sub(Core* self);
void ALU_and(Core* self);
void ALU_or(Core* self);

void ALU_upd(Core* self);

ulong power(ulong base,ulong n);

ulong ALU_toUlong(Core* self, byte reg);
void ALU_pc_incr(Core* self);
