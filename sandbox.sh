#!/bin/bash
mkdir solution
mkdir solution/frames
rm -f solution/frames/*.png
./bin/sandbox solution/frames gravity=9.8 timef=2.0 srate=10000 frate=60 "fsize=1920x1080" "fcenter=(0.5 0.125)" fzoom=1 fscale=1 < problem.txt
ffmpeg -r 60 -i "solution/frames/%05d.png" -y "solution/video.mp4"
rm -f solution/frames/*.png
rmdir solution/frames
