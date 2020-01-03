#!/bin/bash -eux
./9cc "foo();" > tmp.s
gcc -o tmp tmp.s foo.o
./tmp
