CC = gcc

CPU-CARGS = -m32 -shared
CPU-CIN = ./EMU/src/*
CPU-COUT = ./OUT/cpuemu.so

STR-CARGS = -m32 -shared
STR-CIN = ./strlib/src/*
STR-COUT = ./OUT/string.so

ASS-CARGS = -m32
ASS-CIN = ./assambler/src/* $(STR-COUT) $(CPU-COUT)
ASS-COUT = ./OUT/as

TEST-CARGS = -m32
TEST-CIN = $(STR-COUT) ./test.c
TEST-COUT = ./OUT/testcpu

cpu-build:
	$(CC) $(CPU-CARGS) $(CPU-CIN) -o $(CPU-COUT)

str-build:
	$(CC) $(STR-CARGS) $(STR-CIN) -o $(STR-COUT)

ass-build: str-build cpu-build
	$(CC) $(ASS-CARGS) $(ASS-CIN) -o $(ASS-COUT)

ass-test: ass-build
	$(ASS-COUT) ./test/test.asm -o ./OUT/test.bin

build-test: str-build
	$(CC) $(TEST-CARGS) $(TEST-CIN) -o $(TEST-COUT)

test: build-test
	$(TEST-COUT)

clean: build-test
	rm ./OUT/*
