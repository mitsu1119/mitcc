#!/bin/sh

../../bin/run process.c nodebug > ../../dst/process.s
gcc -c ../../dst/process.s -o ../../dst/process.o
gcc -o ../../bin/process ../../dst/process.o -no-pie
../../bin/process
