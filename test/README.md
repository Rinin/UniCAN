# Testing UNICAN 2.0 Implementation #

## Testing Infrastructure

To make it, just type:

	make

It makes UNICAN amalgamation files

	unican.h
	unican.c

from UNICAN sources and a few executables

	unicanfuzz
	unicantest
	unicantest-gprof
	unicantest-trace

### unicanfuzz

This utility generates fuzz UNICAN traffic and outputs it in `canplayer`
format. Thus, its output can be played then with `canplayer` utility.

	Usage: ./unicanfuzz [options] <ifname>
	Options:

	  -e		(generate extended (EFF) CAN frames, default is 'no')
	  -g <ms>	(inter-frame gap in milli seconds, default is 1 ms)
	  -m		(generate mix of SFF and EFF CAN frames, default is 'no')
	  -M <bytes>	(MTU of UNICAN messages, default is 64 bytes)
	  -n <count>	(terminate after <count> UNICAN frames, default infinitive)
	  -N <number>	(<number> of simultaneous issuers, default is 1)
	  -p <percent>	(specify <percent> of long UNICAN messages, default is 20%)
	  -s <seed>	(specify seed to <seed> for PRNG, default is 1)


#### Usage examples

1. Generate 10 UNICAN messages (mix of "short" and "long" UNICAN messages)
   using two issuers (generators):

		% ./unicanfuzz -m -N 2 -n 10 vcan0
		./unicanfuzz -m -N 2 -n 10 vcan0
		(0.001) vcan0 0700D084#8E.0D.00.00.00.00
		(0.002) vcan0 082ECF93#0D.7A.00.00.00
		(0.003) vcan0 014#FE.FF.89.17.11.00.00.00
		(0.004) vcan0 414#00.01.02.03.04.05.06.07
		(0.005) vcan0 01092BB7#50.56.00.00.00.00.00
		(0.006) vcan0 414#08.09.0A.0B.0C.0D.0E.92
		(0.007) vcan0 414#9B
		(0.008) vcan0 0101FD82#44.8E.00.00.00.00
		(0.009) vcan0 053E5F5C#FB.B3.00.00.00
		(0.010) vcan0 0FAFD7AD#42.DA.00.00.00
		(0.011) vcan0 093#36.0F.00.00.00.00.00.00
		(0.012) vcan0 2CA#34.D9.00.00.00.00.00.00
		(0.013) vcan0 06F30792#2E.14.00

2. Generate "short" messages only:

		% ./unicanfuzz -n 10 -p 0 vcan0
		./unicanfuzz -n 10 -p 0 vcan0
		(0.001) vcan0 3A9#73.2F.00.00.00.00
		(0.002) vcan0 2D6#93.E8.00
		(0.003) vcan0 1FB#5B.DD.00
		(0.004) vcan0 1E5#5A.3F.00.00.00.00
		(0.005) vcan0 049#50.56.00.00.00.00.00
		(0.006) vcan0 0C3#71.7D.00.00
		(0.007) vcan0 244#59.F2
		(0.008) vcan0 072#B3.02.00.00
		(0.009) vcan0 3AD#F0.42
		(0.010) vcan0 1EB#93.F2.00

### unicantest

This utility builds itself with using built amalgamation of UNICAN driver. It
handles input text in `canplayer` or `unicanfuzz` format on `stdin` and passes
got CAN frames to UNICAN driver. Thus, it is easy to pipe `unicanfuzz` output
to `unicantest` input.

#### Usage example

	./unicanfuzz -n 50000 vcan0 | ./unicantest
	0	50000

At exit the utility outputs two counters, number of UNICAN errors and number of
received UNICAN messages.

	Usage: ./unicantest [options]

	Options:
	  -v		(increase verbose level)


#### More useful examples

1. Check memory leaks with `valgrind`

		% ./unicanfuzz -n 50000 vcan0 | valgrind ./unicantest
		==7333== Memcheck, a memory error detector
		==7333== Copyright (C) 2002-2012, and GNU GPL'd, by Julian Seward et al.
		==7333== Using Valgrind-3.8.1 and LibVEX; rerun with -h for copyright info
		==7333== Command: ./unicantest
		==7333==
		0	50000
		==7333==
		==7333== HEAP SUMMARY:
		==7333==     in use at exit: 0 bytes in 0 blocks
		==7333==   total heap usage: 150,000 allocs, 150,000 frees, 1,446,880 bytes allocated
		==7333==
		==7333== All heap blocks were freed -- no leaks are possible
		==7333==
		==7333== For counts of detected and suppressed errors, rerun with: -v
		==7333== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 11 from 6)

2. Reject every 500th CAN frame from `unicanfuzz` output with `sed`

		% ./unicanfuzz -n 5000 vcan0 | sed '1~500d' | ./unicantest
		Error 2 <WARNING_BUFFER_OVERWRITE>
		Error 2 <WARNING_BUFFER_OVERWRITE>
		Error 2 <WARNING_BUFFER_OVERWRITE>
		Error 2 <WARNING_BUFFER_OVERWRITE>
		Error 2 <WARNING_BUFFER_OVERWRITE>
		Error 2 <WARNING_BUFFER_OVERWRITE>
		Error 2 <WARNING_BUFFER_OVERWRITE>
		Error 5 <DATA_WITHOUT_START>
		Error 5 <DATA_WITHOUT_START>
		Error 5 <DATA_WITHOUT_START>
		Error 5 <DATA_WITHOUT_START>
		Error 5 <DATA_WITHOUT_START>
		Error 5 <DATA_WITHOUT_START>
		Error 5 <DATA_WITHOUT_START>
		Error 2 <WARNING_BUFFER_OVERWRITE>
		Error 5 <DATA_WITHOUT_START>
		Error 5 <DATA_WITHOUT_START>
		Error 5 <DATA_WITHOUT_START>
		Error 5 <DATA_WITHOUT_START>
		Error 5 <DATA_WITHOUT_START>
		Error 5 <DATA_WITHOUT_START>
		Error 2 <WARNING_BUFFER_OVERWRITE>
		22	4981

3. Run `unicantest` in *verbose* mode

		% ./unicanfuzz -n 10 -N 2 vcan0 | ./unicantest -v
		Event	mid=8e2f len=6
		Message	mid=8e2f len=6 buf=0x9cf2028
		Event	mid=0dcf len=4
		Message	mid=0dcf len=4 buf=0x9cf2028
		Event	mid=14dc len=4
		Message	mid=14dc len=4 buf=0x9cf2028
		Event	mid=b73d len=6
		Message	mid=b73d len=6 buf=0x9cf2028
		Event	mid=c3ef len=6
		Message	mid=c3ef len=6 buf=0x9cf2028
		Event	mid=8e44 len=4
		Message	mid=8e44 len=4 buf=0x9cf2028
		Event	mid=02b3 len=2
		Message	mid=02b3 len=2 buf=0x9cf2028
		Event	mid=42f0 len=0
		Message	mid=42f0 len=0 buf=0x9cf2028
		Event	mid=3672 len=4
		Message	mid=3672 len=4 buf=0x9cf2028
		Event	mid=ca5a len=5
		Message	mid=ca5a len=5 buf=0x9cf2028

		UNICAN summary

		UNICAN error(s): 0
		UNICAN message(s): 10

## Further testing

### unicantest-gprof

TBD.

### unicantest-trace

TBD.

