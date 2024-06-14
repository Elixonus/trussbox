#!/bin/bash
mkdir -p bin
gcc -c src/dampspring.c -o bin/dampspring.o -std=c2x
gcc src/simulation.c -o bin/simulation bin/dampspring.o -lm $(pkg-config --cflags --libs cairo) -std=c2x
gcc src/test.c -o bin/test bin/dampspring.o -lm -std=c2x
