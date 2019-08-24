#!/bin/sh

./bin/run $@ > dst/tmp.s
gcc -o bin/tmp dst/tmp.s
./bin/tmp
echo $?

