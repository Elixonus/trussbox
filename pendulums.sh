#!/bin/bash
mkdir -p tmp/pendulums
rm -rf tmp/pendulums/*
echo "\
pendulum.txt
9.8
10.0
10000.0
60.0
1920
1080
0.0
0.0
1.5
1.0
tmp/pendulums/pendulum
" | source custom.sh
echo "\
doublependulum.txt
9.8
10.0
10000.0
60.0
1920
1080
0.0
0.0
0.75
1.0
tmp/pendulums/doublependulum
" | source custom.sh
echo "\
doublependulumroller.txt
9.8
10.0
10000.0
60.0
1920
1080
0.0
0.0
0.75
1.0
tmp/pendulums/doublependulumroller
" | source custom.sh
echo "\
decuplependulum.txt
9.8
10.0
10000.0
60.0
1920
1080
0.0
-1.35
0.25
1.0
tmp/pendulums/decuplependulum
" | source custom.sh
