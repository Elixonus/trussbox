#!/bin/bash
mkdir out
mkdir out/img
./bin/simulation
ffmpeg -i "out/img/%05d.png" -y "out/video.mp4" -framerate 60
rm -f out/img/*.png
rmdir out/img
