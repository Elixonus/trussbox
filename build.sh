mkdir -p bin
mkdir -p bin/lib
gcc -c src/lib/dampspring.c -o bin/lib/dampspring.o -std=c2x
gcc src/main.c -o bin/main bin/lib/dampspring.o -lm -std=c2x
gcc src/test.c -o bin/test bin/lib/dampspring.o -lm -std=c2x
