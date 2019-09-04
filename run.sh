#!/bin/sh

./bin/run $1 > dst/tmp.s
gcc -c dst/tmp.s -o dst/tmp.o
gcc -o bin/tmp dst/tmp.o
./bin/tmp
echo $?

