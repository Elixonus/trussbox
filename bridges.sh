#!/bin/bash
echo "solving and rendering each of the bridges"
mkdir -p tmp/bridges
rm -rf tmp/bridges/*
echo "solving and rendering warren bridge"
echo "\
warren.txt
9.8
3.0
10000.0
60.0
1920
1080
0.5
0.125
1.0
1.0
tmp/bridges/warren
" | source custom.sh
echo "solving and rendering pratt bridge"
echo "\
pratt.txt
9.8
3.0
10000.0
60.0
1920
1080
0.5
0.125
1.0
1.0
tmp/bridges/pratt
" | source custom.sh
echo "solving and rendering howe bridge"
echo "\
howe.txt
9.8
3.0
10000.0
60.0
1920
1080
0.5
0.125
1.0
1.0
tmp/bridges/howe
" | source custom.sh
echo "solving and rendering parker bridge"
echo "\
parker.txt
9.8
3.0
10000.0
60.0
1920
1080
0.5
0.125
1.0
1.0
tmp/bridges/parker
" | source custom.sh
echo "solving and rendering cambridge bridge"
echo "\
cambridge.txt
9.8
3.0
10000.0
60.0
1920
1080
0.5
0.125
1.0
1.0
tmp/bridges/cambridge
" | source custom.sh
echo "bridge files can now be found in tmp/bridges/"
