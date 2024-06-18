#!/bin/bash
mkdir bin
gcc -c src/dampspring.c -o bin/dampspring.o -std=c2x
gcc src/simulation.c -o bin/simulation bin/dampspring.o -lm $(pkg-config --cflags --libs cairo) -std=c2x
gcc src/testfunctions.c -o bin/testfunctions bin/dampspring.o -lm $(pkg-config --cflags --libs cairo) -std=c2x
