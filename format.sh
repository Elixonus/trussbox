#!/bin/bash
filelist=$(
	find . -type f -name "*.c" &&
	find . -type f -name "*.h" &&
	find . -type f -name "*.sh" -not -path "./tmp/*" &&
	find . -type f -name "*.txt" -not -path "./tmp/*" &&
	echo "LICENSE"
)
dos2unix $filelist
for file in $filelist
do
	iconv -t US-ASCII "$file" | sponge "$file"
done
