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
	
	  -e		(generate extended (EFF) CAN frames, default no)
	  -g <ms>	(inter-frame gap in milli seconds, default is 1 ms)
	  -m		(generate mix of SFF and EFF CAN frames, default no)
	  -M <bytes>	(MTU of UNICAN messages, default is 64 bytes)
	  -n <count>	(terminate after <count> UNICAN frames, default infinitive)
	  -N <number>	(<number> of simultaneous issuers, default is 1)
	  -p <percent>	(specify <percent> of long UNICAN messages, default is 20%)
	  -r		(seed internal random generator on start, default no)

#### Usage examples

1. Generate 10 UNICAN messages (mix of "short" and "long" UNICAN messages) using two issuers (generators):

		./unicanfuzz -m -N 2 -n 10 vcan0
		(0.001) vcan0 00000002#83.89.00.00.00.00
		(0.002) vcan0 28C#56.DF.00.00.00.00
		(0.003) vcan0 08B74A15#B8.7D
		(0.004) vcan0 0C6D1F68#17.23.00.00.00.00.00.00
		(0.005) vcan0 070#29.D9.00.00.00.00.00
		(0.006) vcan0 0C92C812#FE.FF.14.F8.2A.00.00.00
		(0.007) vcan0 09C96680#D8.DD.00
		(0.008) vcan0 1C92C812#00.01.02.03.04.05.06.07
		(0.009) vcan0 1C92C812#08.09.0A.0B.0C.0D.0E.0F
		(0.010) vcan0 1C92C812#10.11.12.13.14.15.16.17
		(0.011) vcan0 1C92C812#18.19.1A.1B.1C.1D.1E.1F
		(0.012) vcan0 351#6D.58.00.00.00
		(0.013) vcan0 066#77.6E.00.00.00
		(0.014) vcan0 1C92C812#20.21.22.23.24.25.26.27
		(0.015) vcan0 1C92C812#AF.81
		(0.016) vcan0 24D#F3.F0.00.00.00.00.00.00

2. Generate "short" messages only:

		./unicanfuzz -n 10 -p 0 vcan0
		(0.001) vcan0 001#0C.1F.00
		(0.002) vcan0 209#8C.1D.00.00.00.00
		(0.003) vcan0 3DC#72.CC.00.00.00.00
		(0.004) vcan0 2D9#7D.FF.00.00.00.00.00
		(0.005) vcan0 0DE#17.23.00.00.00.00.00.00
		(0.006) vcan0 0AC#75.C6.00.00.00
		(0.007) vcan0 1E7#F5.31
		(0.008) vcan0 114#7C.E7.00.00.00.00.00
		(0.009) vcan0 1D8#C3.C0.00
		(0.010) vcan0 1D5#50.51.00.00.00.00

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

		./unicanfuzz -n 50000 vcan0 | valgrind ./unicantest
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

		./unicanfuzz -n 5000 vcan0 | sed '1~500d' | ./unicantest
		Error 2 <WARNING_BUFFER_OVERWRITE>
		Error 2 <WARNING_BUFFER_OVERWRITE>
		Error 2 <WARNING_BUFFER_OVERWRITE>
		Error 2 <WARNING_BUFFER_OVERWRITE>
		Error 2 <WARNING_BUFFER_OVERWRITE>
		Error 2 <WARNING_BUFFER_OVERWRITE>
		Error 2 <WARNING_BUFFER_OVERWRITE>
		Error 2 <WARNING_BUFFER_OVERWRITE>
		Error 2 <WARNING_BUFFER_OVERWRITE>
		9	4980

3. Run `unicantest` in *verbose* mode

		./unicanfuzz -n 10 -N 2 vcan0 | ./unicantest -v
		Event	mid=831f len=1
		Message	mid=831f len=1 buf=0x9d68028
		Event	mid=651d len=4
		Message	mid=651d len=4 buf=0x9d68028
		Event	mid=9915 len=1
		Message	mid=9915 len=1 buf=0x9d68028
		Event	mid=68ad len=3
		Message	mid=68ad len=3 buf=0x9d68028
		Event	mid=70ac len=3
		Message	mid=70ac len=3 buf=0x9d68028
		Event	mid=30e7 len=1
		Message	mid=30e7 len=1 buf=0x9d68028
		Event	mid=1bc0 len=2
		Message	mid=1bc0 len=2 buf=0x9d68068
		Event	mid=929c len=2
		Message	mid=929c len=2 buf=0x9d68068
		Event	mid=1c7c len=20
		Message	mid=1c7c len=20 buf=0x9d68038
		Event	mid=a502 len=24
		Message	mid=a502 len=24 buf=0x9d68038

		UNICAN summary

		UNICAN error(s): 0
		UNICAN message(s): 10


## Further testing

### unicantest-gprof

TBD.


### unicantest-trace

TBD.

