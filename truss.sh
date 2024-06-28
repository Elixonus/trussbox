#!/bin/bash
mkdir out
mkdir out/img
./bin/trusstructure
ffmpeg -r 60 -i "out/img/%05d.png" -y "out/video.mp4"
rm -f out/img/*.png
rmdir out/img
