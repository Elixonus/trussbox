#!/bin/bash
echo "solving and rendering each of the pendulums"
mkdir -p tmp/pendulums
rm -rf tmp/pendulums/*
echo "solving and rendering normal pendulum"
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
echo "solving and rendering double pendulum"
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
echo "solving and rendering double pendulum (roller)"
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
echo "solving and rendering decuple pendulum"
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
echo "pendulum files can now be found in tmp/pendulums/"
