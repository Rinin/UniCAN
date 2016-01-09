CFLAGS = -Wall -Werror -g -O2 -I. -Iinclude

LDLIBS = libcan.a

LIB = libcan.a

LOBJS = lib.o canframelen.o rng64.o

PROGS = unicanfuzz unicantest unicantest-gprof unicantest-trace

all: $(LIB) $(PROGS)

libcan.a: $(LOBJS)
	ar cru $@ $^
	ranlib $@

clean:
	rm -f $(LIB) $(LOBJS) $(PROGS) unican.c unican.h gmon.out trace.out

unican.c: make_amalgamation.sh
	sh make_amalgamation.sh

unicanfuzz: unicanfuzz.c
	$(CC) $(CFLAGS) $^ $(LDLIBS) -o $@

unicantest: unican.c unicantest.c
	$(CC) $(CFLAGS) $^ $(LDLIBS) -o $@

unicantest-gprof: unican.c unicantest.c trace.c
	$(CC) $(CFLAGS) -pg $^ $(LDLIBS) -o $@

# https://balau82.wordpress.com/2010/10/06/trace-and-profile-function-calls-with-gcc/
unican.o: CFLAGS += -finstrument-functions

unicantest-trace: unican.o unicantest.c trace.c
	$(CC) $(CFLAGS) $^ $(LDLIBS) -o $@
	rm -f $<
