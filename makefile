cc = gcc
std = -std=c11
out = build/lib/libuboa.a

macro = \
	-DBUILD_DATE="\"12/19/2022, Monday\"" \
	-DGC_VERSION="\"0.0.0.dev4\"" \
	-DCOPYRIGHT="\"Copyright\(c\) 2022, Lizbing. All rights reserved.\"" \
	-DGC_PRODUCT_NAME="\"Uboa Garbage Collector\"" \
	-DFEEDBACK_EMAIL="\"feedback@relight.team\""

src = \
	src/os/linux/*.c \
	src/share/*.c \
	src/util/*.c

all:
	mkdir -p build
	mkdir -p build/include && mkdir -p build/lib
	cp README.md build/
	cp src/uboa.h build/include/
	$(cc) $(macro) $(std) -lpthread -c $(src)
	ar rcs $(out) *.o
	rm *.o
