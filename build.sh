#!/bin/bash
echo "creating executables"
shopt -s expand_aliases
alias gcc="gcc-14"
mkdir bin
gcc -c src/msd.c -o bin/msd.o -std=c23
gcc src/solvetruss.c -o bin/solvetruss bin/msd.o -lm -std=c23
gcc src/rendertruss.c -o bin/rendertruss bin/msd.o -lm $(pkg-config --cflags --libs cairo) -std=c23
gcc src/forcediagram.c -o bin/forcediagram bin/msd.o -lm $(pkg-config --cflags --libs cairo) -std=c23
gcc src/trussutils.c -o bin/trussutils bin/msd.o -lm -std=c23
gcc src/feedback.c -o bin/feedback bin/msd.o -lm -std=c23
gcc src/pipeline.c -o bin/pipeline -lm -std=c23
gcc src/subtitles.c -o bin/subtitles -lm $(pkg-config --cflags --libs cairo) -std=c23
echo "executables can now be found in bin/"
