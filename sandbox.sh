#!/bin/bash
mkdir out
mkdir out/img
./bin/sandbox gravity=9.8 timef=2.0 srate=10000 frate=60 "fsize=<1920 1080>" "fcenter=(0.5 0.125)" fzoom=1 fscale=1 < problem.txt
ffmpeg -r 60 -i "out/img/%05d.png" -y "out/video.mp4"
rm -f out/img/*.png
rmdir out/img
