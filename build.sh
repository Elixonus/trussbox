#!/bin/bash
shopt -s expand_aliases
alias gcc="gcc-14"
mkdir bin
gcc -c src/dampspring.c -o bin/dampspring.o -std=c23
gcc src/sandbox.c -o bin/sandbox bin/dampspring.o -lm $(pkg-config --cflags --libs cairo) -std=c23
