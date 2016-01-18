CFLAGS = -Wall -Wextra -Wno-strict-aliasing -g -O2 -I. -Iinclude

LDLIBS = libcan.a

LIB = libcan.a

LOBJS = lib.o canframelen.o rng64.o

PROGS = unicanfuzz unicantest

.PHONY: all
all: $(LIB) $(PROGS)

libcan.a: $(LOBJS)
	ar cru $@ $^
	ranlib $@

.PHONY: clean
clean:
	rm -f $(LIB) $(LOBJS) $(PROGS) unican.c unican.h

unican.c: make_amalgamation.sh
	sh make_amalgamation.sh

unicanfuzz: unicanfuzz.c
	$(CC) $(CFLAGS) $^ $(LDLIBS) -o $@

unicantest: unican.c unicantest.c
	$(CC) $(CFLAGS) $^ $(LDLIBS) -o $@

.PHONY: install
install: all
	cp unicanfuzz ~/bin

.PHONY: test
test: all
	@tclsh test.tcl
