#!/bin/sh

../../bin/run ptrcal.c nodebug > ../../dst/ptrcal.s
gcc -c ../../dst/ptrcal.s -o ../../dst/ptrcal.o
gcc -c allocate.c -o ../../dst/allocate.o
gcc -o ../../bin/ptrcal ../../dst/allocate.o ../../dst/ptrcal.o
../../bin/ptrcal
echo $?
