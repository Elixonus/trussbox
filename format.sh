#!/bin/bash
dos2unix *
find . -type f -name '*.c' -print -exec iconv -f '*' -t US-ASCII {} -o {} \;
find . -type f -name '*.sh' -print -exec iconv -f '*' -t US-ASCII {} -o {} \;
