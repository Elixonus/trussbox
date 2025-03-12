#!/bin/bash
mkdir out
mkdir out/img
./bin/sandbox < problem.txt
ffmpeg -r 60 -i "out/img/%05d.png" -y "out/video.mp4"
rm -f out/img/*.png
rmdir out/img
