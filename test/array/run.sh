#!/bin/sh

../../bin/run array.c nodebug > ../../dst/array.s
gcc -c ../../dst/array.s -o ../../dst/array.o
gcc -o ../../bin/array ../../dst/array.o
../../bin/array
echo $?
