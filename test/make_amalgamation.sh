#!/bin/sh
#
# This script outputs unican amalgamation for [59da225c0a] stuff.

amalgam_header=`pwd`/unican.h
amalgam_source=`pwd`/unican.c

cd ..

do_strip(){
	sed 's/\s\+$//g;/^$/d'
}

fix_types(){
	sed 's/\<\(u\?int\)\(8\|16\|32\|64\)\>/\1\2_t/g'
}

dprintf(){
	sed 's/\<printf\>/dprintf/g'
}

exec >$amalgam_header

echo '#ifndef UNICAN_H_'
echo '#define UNICAN_H_'
echo '#include <stdint.h>'

file=unican_setup.h
echo "/* {{{ $file */"
cat $file | sed -ne '$d;23,$p' | do_strip
echo "/* $file }}} */"


for file in 				\
	unican_errcodes.h		\
	unican_msg_id.h			\
	unican_hardware.h.example	\
	;
do
	echo "/* {{{ $file */"
	cat $file | sed -ne '$d;3,$p' | fix_types | do_strip
	echo "/* $file }}} */"
done

file=unican_user.h
echo "/* {{{ $file */"
cat $file | sed -ne '$d;12,13p' | fix_types | do_strip
echo "/* $file }}} */"

file=unican.h
echo "/* {{{ $file */"
cat $file | sed -ne '$d;16,$p' | fix_types | do_strip
echo "/* $file }}} */"

echo '#endif /* UNICAN_H_ */'

exec >$amalgam_source

echo '#include <stdlib.h>'
echo '#include <stdio.h>'
echo '#include <stdint.h>'
echo '#include "unican.h"'
cat<<EOF
#if DEBUG
#define dprintf(...) printf(__VA_ARGS__)
#else
#define dprintf(...)
#endif
EOF

file=unican.c
echo "/* {{{ $file */"
cat $file | sed -ne '4,$p' | fix_types | do_strip | dprintf
echo "/* $file }}} */"

exit 0
