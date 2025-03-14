#!/bin/bash
echo "warren.txt
9.8
3.0
1000.0
60.0
1920
1080
0.5
0.125
1.0
1.0
tmp/bridges/warren" | source deformtruss.sh
echo "pratt.txt
9.8
3.0
1000.0
60.0
1920
1080
0.5
0.125
1.0
1.0
tmp/bridges/pratt" | source deformtruss.sh
echo "howe.txt
9.8
3.0
1000.0
60.0
1920
1080
0.5
0.125
1.0
1.0
tmp/bridges/howe" | source deformtruss.sh
