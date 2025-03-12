#!/bin/bash
mkdir tmp
mkdir tmp/frames
rm -f tmp/frames/*.png
./bin/sandbox tmp/frames gravity=9.8 timef=2.0 srate=10000 frate=60 "fsize=1920x1080" "fcenter=(0.5 0.125)" fzoom=1 fscale=1 < problem.txt
ffmpeg -r 60 -i "tmp/frames/%05d.png" -y "tmp/video.mp4"
rm -f tmp/frames/*.png
rmdir tmp/frames
