CFLAGS = -Wall -Werror -g -O2 -I. -Iinclude

LDLIBS = libcan.a

LIB = libcan.a

LOBJS = lib.o canframelen.o rng64.o

PROGS = unicanfuzz unicantest

all: $(LIB) $(PROGS)

libcan.a: $(LOBJS)
	ar cru $@ $^
	ranlib $@

clean:
	rm -f $(LIB) $(LOBJS) $(PROGS) unican.c unican.h

unican.c: make_amalgamation.sh
	sh make_amalgamation.sh

unicanfuzz: unicanfuzz.c
	$(CC) $(CFLAGS) $^ $(LDLIBS) -o $@

unicantest: unican.c unicantest.c
	$(CC) $(CFLAGS) $^ $(LDLIBS) -o $@

test: all
	@tclsh test.tcl
