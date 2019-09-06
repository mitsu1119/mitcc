#!/bin/sh

../../bin/run fibonacci.c nodebug > ../../dst/fibonacci.s
gcc -c ../../dst/fibonacci.s -o ../../dst/fibonacci.o
gcc -c print.c -o ../../dst/print.o
gcc -o ../../bin/fibonacci ../../dst/fibonacci.o ../../dst/print.o
../../bin/fibonacci
