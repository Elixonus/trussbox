#!/bin/bash
shopt -s expand_aliases
alias gcc="gcc-14"
mkdir bin
gcc -c src/dampspring.c -o bin/dampspring.o -std=c23
gcc src/solvetruss.c -o bin/solvetruss bin/dampspring.o -lm -std=c23
gcc src/rendertruss.c -o bin/rendertruss bin/dampspring.o -lm $(pkg-config --cflags --libs cairo) -std=c23
gcc src/forcediagram.c -o bin/forcediagram bin/dampspring.o -lm $(pkg-config --cflags --libs cairo) -std=c23
gcc src/subtitles.c -o bin/subtitles -lm $(pkg-config --cflags --libs cairo) -std=c23
