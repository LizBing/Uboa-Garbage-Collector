cc = gcc
std = -std=c11
out = test

src = \
	../src/os/linux/*.c \
	../src/share/*.c \
	../src/util/*.c \
	../demo/test.c

all:
	$(cc) $(std) -lpthread -o $(out) $(src) -g

	
