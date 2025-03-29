#!/bin/bash
echo "solving and rendering each of the miscellaneous systems"
mkdir -p tmp/miscellaneous
rm -rf tmp/miscellaneous/*
echo "solving and rendering cantilever"
echo "\
cantilever.txt
9.8
3.0
100000.0
60.0
1920
1080
0.5
0.1
1.0
0.5
tmp/miscellaneous/cantilever
" | source custom.sh
echo "miscellaneous files can now be found in tmp/miscellaneous/"
