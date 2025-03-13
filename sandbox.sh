#!/bin/bash
read -r -e -n 1000 -p "problem filename: " filename
read -r -e -n 1000 -p "solution dirname: " dirname
mkdir -p $dirname
mkdir -p $dirname/frames
rm -f $dirname/frames/*.png
./bin/sandbox $dirname/frames gravity=9.8 timef=5.0 srate=10000.0 frate=60.0 fsize=1920x1080 "fcenter=(0.5 0.125)" fzoom=1.0 fscale=1.0 < $filename > $dirname/solution.txt
ffmpeg -r 60 -i $dirname/frames/%05d.png -y $dirname/video.mp4 -loglevel fatal
rm -f $dirname/frames/*.png
rmdir $dirname/frames
