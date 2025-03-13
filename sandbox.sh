#!/bin/bash
mkdir tmp

mkdir tmp/warren
mkdir tmp/warren/frames
rm -f tmp/warren/frames/*.png
./bin/sandbox tmp/warren/frames gravity=9.8 timef=5.0 srate=10000.0 frate=60.0 "fsize=1920x1080" "fcenter=(0.5 0.125)" fzoom=1.0 fscale=1.0 < warren.txt > tmp/warren/solution.txt
ffmpeg -r 60 -i "tmp/warren/frames/%05d.png" -y "tmp/warren/video.mp4"
rm -f tmp/warren/frames/*.png
rmdir tmp/warren/frames

mkdir tmp/pratt
mkdir tmp/pratt/frames
rm -f tmp/pratt/frames/*.png
./bin/sandbox tmp/pratt/frames gravity=9.8 timef=5.0 srate=10000.0 frate=60.0 "fsize=1920x1080" "fcenter=(0.5 0.125)" fzoom=1.0 fscale=1.0 < pratt.txt > tmp/pratt/solution.txt
ffmpeg -r 60 -i "tmp/pratt/frames/%05d.png" -y "tmp/pratt/video.mp4"
rm -f tmp/pratt/frames/*.png
rmdir tmp/pratt/frames

mkdir tmp/howe
mkdir tmp/howe/frames
rm -f tmp/howe/frames/*.png
./bin/sandbox tmp/howe/frames gravity=9.8 timef=5.0 srate=10000.0 frate=60.0 "fsize=1920x1080" "fcenter=(0.5 0.125)" fzoom=1.0 fscale=1.0 < howe.txt > tmp/howe/solution.txt
ffmpeg -r 60 -i "tmp/howe/frames/%05d.png" -y "tmp/howe/video.mp4"
rm -f tmp/howe/frames/*.png
rmdir tmp/howe/frames
