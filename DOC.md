# DroneCPU

- type Big Endian
- instruction size 4B

## Register plan

REGISTER | type | Name | Permissions
---|---|---|---
0-7 | GPR | R0 - R7 | RW
8 | ALU (A) | CA | RW
9 | ALU (B) | CB | RW
10 | ALU RESULT ADD | CRADD | RO
11 | ALU RESULT SUB | CRSUB | RO
12 | ALU RESULT AND | CRAND | RO
13 | ALU RESULT OR | CROR | RO
14 | PC | PC | RO
15 | CORE ID | CID | RO
16 | MEM ADDR | MR | RW

## supported instructions

type | INST | DESC | A | B | C
---|---|---|---|---|---
 \- | 0x00 | NOOP |
REG | 0x01 | CPR | dst | reg
REG | 0x02 | SRB | dst | byte | value
REG | 0x03 | RWM (Register Write \[to\] Memory)| reg | byte
REG | 0x04 | MWR  (Memory Write \[to\] Register)| dst | byte
CPU | 0x10 | JMP | reg
CPU | 0x11 | JIZ | reg | check
CPU | 0x12 | JNZ | reg | check
CPU | 0x13 | HLT
ALU | 0x20 | UPD

## Assambler

- priv // private variable
- pub   // public variable

- \[.\] = current position

- access:
> \[byte (0-7)\]:\[varname\]
