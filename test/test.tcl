#!/usr/bin/env tclsh
package require Tcl 8.5

set N 0
set NF 0

proc run_test {exec output} {
  global N NF
  puts -nonewline "TEST [incr N]: "
  flush stdout
  catch {exec {*}$exec 2>@1} out
  if {$out == $output} {
    puts OK
  } {
    puts FAIL
    incr NF
    puts stderr $out
  }
}

run_test {
  ./unicantest << ""
} "0\t0"

run_test {
  ./unicantest << "comment"
} "0\t0"

run_test {
  ./unicantest << "(0.001) x 123#45"
} "Error 11 <CAN_MESSAGE_TOO_SHORT>\n1\t0"

run_test {
  ./unicantest << "(0.001) x 01234567#45"
} "Error 11 <CAN_MESSAGE_TOO_SHORT>\n1\t0"

run_test {
  ./unicantest << "(0.001) x 123#FEFF"
} "Error 15 <HEADER_TOO_SHORT>\n1\t0"

run_test {
  ./unicantest << "(0.001) x 7FF#1234"
} "Error 5 <DATA_WITHOUT_START>\n1\t0"

run_test {
  ./unicantest << "(0.001) x 1FFFFFFF#1234"
} "Error 5 <DATA_WITHOUT_START>\n1\t0"

run_test {
  ./unicantest << "
(0.001) x 0FFFFFFF#FEFF.3412.0300.0000
(0.002) x 1FFFFFFF#01.02.03
"
} "Error 3 <WRONG_CRC>\n1\t0"

run_test {
  ./unicantest << "
(0.001) x 0FFFFFFF#FEFF.3412.0300.0000
(0.002) x 1FFFFFFF#00
(0.003) x 1FFFFFFF#00
(0.004) x 1FFFFFFF#00
"
} "0\t1"

run_test {
  ./unicantest << "
(0.001) x 3FF#FEFF.3412.0300.0000
(0.002) x 7FF#00
(0.003) x 7FF#00
(0.004) x 7FF#00
"
} "0\t1"

run_test {
  ./unicanfuzz -n10 x | ./unicantest
} "0\t10"

run_test {
  ./unicanfuzz -e -n10 x | ./unicantest
} "0\t10"

run_test {
  ./unicanfuzz -m -n10 x | ./unicantest
} "0\t10"

run_test {
  ./unicanfuzz -p0 -n10 x | ./unicantest
} "0\t10"

run_test {
  ./unicanfuzz -p100 -n10 x | ./unicantest
} "0\t10"

run_test {
  ./unicanfuzz -N5 -n1000 x | ./unicantest
} "0\t1000"

run_test {
  ./unicanfuzz -e -N5 -n1000 x | ./unicantest
} "0\t1000"

run_test {
  ./unicanfuzz -m -N5 -n1000 x | ./unicantest
} "0\t1000"

run_test {
  ./unicanfuzz -m -N5 -M0 -n100 x | ./unicantest
} "0\t100"

run_test {
  ./unicanfuzz -m -N5 -M65533 -n10 x | ./unicantest
} "0\t10"

return $NF
