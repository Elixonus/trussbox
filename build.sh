#!/bin/bash
shopt -s expand_aliases
alias gcc="gcc-14"
mkdir bin
gcc -c src/dampspring.c -o bin/dampspring.o -std=c23
gcc src/trusstructure.c -o bin/trusstructure bin/dampspring.o -lm $(pkg-config --cflags --libs cairo) -std=c23
gcc src/testfunctions.c -o bin/testfunctions bin/dampspring.o -lm -std=c23
