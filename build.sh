mkdir -p bin
gcc -c src/dampspring.c -o bin/dampspring.o -std=c2x
gcc src/main.c -o bin/main bin/dampspring.o -lm -std=c2x
gcc src/test.c -o bin/test bin/dampspring.o -lm -std=c2x
