#!/bin/sh

../../bin/run HelloWorld.c nodebug > ../../dst/HelloWorld.s
gcc -c ../../dst/HelloWorld.s -o ../../dst/HelloWorld.o
gcc -o ../../bin/HelloWorld ../../dst/HelloWorld.o -no-pie
../../bin/HelloWorld
